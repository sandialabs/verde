//
//-------------------------------------------------------------------------
// Filename      : MDBVerde.hpp 
// Creator       : Ray Meyers
//
// Date          : 01/03/02
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef MDB_VERDE_HPP
#define MDB_VERDE_HPP


#include "MDBInterface.hpp"
#include <vector>


/*! This structure is used to hold information to facilitate the lookup of
the element connectivity information from the EntityHandle, which for this
implementation is an int...
*/
struct BlockInfo
{
  int block_number;
  int starting_id;
  int number_elements;
  int number_vertices_per_element;
};


//! Initial Verde implementation of Mesh DataBase Interface
class MDBVerde : public MDBInterface
{
public:
     
  //! Constructor
  MDBVerde() ;

  //! Destructor
  virtual ~MDBVerde();

  //! Load num_vertices Mesh Vertices into the Mesh Object
  virtual MDBErrorCode MDB_IDECL load_mesh_vertices(MDBInt num_vertices,
                                          MDBDouble *x_coords,
                                          MDBDouble *y_coords,
                                          MDBDouble *z_coords);

  //! Load num_entities Mesh Elements into the Mesh Object
  virtual MDBErrorCode MDB_IDECL load_mesh_entities(MDBInt num_entities,
                                          MDBInt starting_id,
                                          MDBInt vertices_per_entity,
                                          MDBEntityHandle *connectivity,
                                          MDBInt *original_ids);

  //! Extends length of coordinate arrays
  virtual MDBErrorCode MDB_IDECL extend_xyz_arrays( MDBInt old_array_size,
                                          MDBInt new_array_size, 
                                          MDBDouble *x_coords,
                                          MDBDouble *y_coords,
                                          MDBDouble *z_coords);

  //! Reset the Mesh Object data
  virtual MDBErrorCode MDB_IDECL reset();

 //! return the coordinates of a mesh vertex
  virtual MDBErrorCode MDB_IDECL GetCoords(MDBEntityHandle vertex_handle,
                        MDBDouble *coords);

  virtual MDBErrorCode MDB_IDECL GetNumAdjacencies(MDBEntityHandle element_handle,
                                         MDBEntityType calling_type,
                                         MDBEntityDimension dimension,
                                         int *num_entities);

  virtual MDBErrorCode MDB_IDECL GetAdjacencies(MDBEntityHandle element_handle,
                                      MDBEntityType calling_type,
                                      MDBEntityDimension dimension,
                                      MDBInt *num_adjacent,
                                      MDBEntityHandle *adj_entities_ptr);
  
  virtual int MDB_IDECL QueryInterface( const MDBuuid& uuid, MDBUnknownInterface** iface );

private:

  //! number of mesh vertices in MDB
  int numberVertices;

  //! pointer to x coordinates of nodes
  double *xCoords;

    //! pointer to y coordinates of nodes
  double *yCoords;

  //! pointer to z coordinates of nodes
  double *zCoords;

  //! list containing block information for use in finding elements in this MDB
  std::vector<BlockInfo> blockInfo;

  //! std::vector containing pointers to the connectivity lists of each block
  std::vector<int*> connectivityList;

  //! finds BlockInfo in which entity_handle resides 
  BlockInfo *find_block(MDBEntityHandle entity_handle);

  //! the last block that was accessed
  BlockInfo mLastBlock;

};



#endif
