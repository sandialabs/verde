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

/*! This structure is used to hold information for Tag entities which are
defined for every element or vertex in the model.  The tag_size gives the
size of each tag.  The data is stored in the data array
*/
struct TagInfo
{
  unsigned char *data;
  int tag_size;

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

  //! given the element handle, gets the number of adjacent entities
  virtual MDBErrorCode MDB_IDECL GetNumAdjacencies(MDBEntityHandle element_handle,
                                         MDBEntityType calling_type,
                                         MDBEntityDimension dimension,
                                         int *num_entities);

  //! retrieve the handles to the adjacency information for this handle
  virtual MDBErrorCode MDB_IDECL GetAdjacencies(MDBEntityHandle element_handle,
                                      MDBEntityType calling_type,
                                      MDBEntityDimension dimension,
                                      MDBInt *num_adjacent,
                                      MDBEntityHandle *adj_entities_ptr);
  
 
  //! specifies a tag to be added to the database.  It is associated with the
  //! entity_type (actually, only either elements or nodes).
  virtual MDBErrorCode MDB_IDECL AddTag(MDBEntityType entity_type,
                              MDBInt tag_size,
                              MDBInt *tag_id);


  //! for the given entity_handle and tag_id, sets the tag_value
  virtual MDBErrorCode MDB_IDECL SetTag(MDBEntityHandle *entity_handle,
                              MDBInt tag_id,
                              MDBVoid *tag_value);

  //! for the given entity_handle and tag_id, returns the tag_value
  virtual MDBErrorCode MDB_IDECL GetTag(MDBEntityHandle *entity_handle,
                              MDBInt tag_id,
                              MDBVoid *tag_value);

  virtual int MDB_IDECL QueryInterface( const MDBuuid& uuid, MDBUnknownInterface** iface );

private:

  //! number of mesh vertices in MDB
  int numberVertices;

  //! number of elements in the MDB
  int numberElements;

  //! pointer to x coordinates of nodes
  double *xCoords;

    //! pointer to y coordinates of nodes
  double *yCoords;

  //! pointer to z coordinates of nodes
  double *zCoords;

  //! this may contain a hint to the MDB as to where to look to get the connectivity
  //! information for an EntityHandle.  It may be NULL.
  BlockInfo *currentBlockHint;

  //! list containing block information for use in finding elements in this MDB
  std::vector<BlockInfo> blockInfo;

  //! std::vector containing pointers to the connectivity lists of each block
  std::vector<MDBInt*> connectivityList;

  //! std::vector containing pointers to any data associated with the entities
  //! in the database
  std::vector<TagInfo> tagData;

  //! finds if the entity_handle is in the block.  
  bool is_in_block (MDBEntityHandle entity_handle, 
                    BlockInfo *block_info);

  //! finds the block corresponding to this entity_handle.  Returns 0 if the 
  //! entity_handle is not valid or not found in the database
  BlockInfo *find_block(MDBEntityHandle entity_handle);

};



#endif
