//
//-------------------------------------------------------------------------
// Filename      : MDBInterface.hpp 
// Creator       : Ray Meyers
//
// Date          : 01/03/02
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef MDB_INTERFACE_HPP
#define MDB_INTERFACE_HPP

#include "MDBUnknownInterface.h"

#define MDB_INTERFACE_VERSION "1.0.0"

static const MDBuuid IDD_MDBVerde = MDBuuid( 0x8956e0a, 0xc300, 0x4005, 
   0xbd, 0xf6, 0xc3, 0x4e, 0xf7, 0x1f, 0x5a, 0x52 );


/* some additional uuid's when needed
 * 
 * (0xf39a901c, 0xbb36, 0x4cac, 0xbb, 0xe5, 0xf6, 0x65, 0x0f, 0x25, 0x9e, 0x76)
 * (0xac4e2c6c, 0x7512, 0x47e5, 0xaf, 0xd4, 0x5c, 0xbf, 0xfd, 0x03, 0x6a, 0xc5)
 * (0x9a2d3715, 0xb83e, 0x4954, 0x96, 0x9d, 0xb6, 0xa6, 0xc2, 0x5b, 0x91, 0xcb)
 * (0xf8236088, 0xcdd0, 0x45b7, 0xb9, 0x60, 0xc8, 0xd0, 0x4b, 0x2c, 0xd0, 0xdf)
 */


typedef int MDBInt;
typedef float MDBFloat;
typedef double MDBDouble;

typedef MDBInt MDBEntityHandle;

enum MDBErrorCode { MDB_SUCCESS = 0,
                    MDB_INDEX_OUT_OF_RANGE};

enum MDBEntityDimension { DIM_0_ENTITY = 0,
                       DIM_1_ENTITY,
                       DIM_2_ENTITY,
                       DIM_3_ENTITY,
                       HIGHEST_DIMENSION };

enum MDBEntityType { VERTEX = 0,
                     EDGE,
                     FACE,
                     REGION,
                     HIGHEST_TYPE};

//! Pure Virtual Interface for Mesh Object Interface
class MDBInterface : public MDBUnknownInterface
{
public:
   
  //! Constructor
  MDBInterface() {};

  //! Destructor
  virtual ~MDBInterface() {};

  //! Load num_vertices Mesh Vertices into the Mesh Object
  virtual MDBErrorCode MDB_IDECL load_mesh_vertices(MDBInt num_vertices,
                                          MDBDouble *x_coords,
                                          MDBDouble *y_coords,
                                          MDBDouble *z_coords) = 0;

  //! Load num_entities Mesh Elements into the Mesh Object
  virtual MDBErrorCode MDB_IDECL load_mesh_entities(MDBInt num_entities,
                                          MDBInt starting_id,
                                          MDBInt vertices_per_entity,
                                          MDBEntityHandle *connectivity,
                                          MDBInt *original_ids) = 0;

  //! Extends length of coordinate arrays
  virtual MDBErrorCode MDB_IDECL extend_xyz_arrays( MDBInt old_array_size,
                                          MDBInt new_array_size,
                                          MDBDouble *x_coords,
                                          MDBDouble *y_coords,
                                          MDBDouble *z_coords) = 0;
  //! Reset the Mesh Object data
  virtual MDBErrorCode MDB_IDECL reset() = 0;

  //! return the coordinates of a mesh vertex
  virtual MDBErrorCode MDB_IDECL GetCoords(MDBEntityHandle vertex_handle,
                                MDBDouble *coords) = 0;

  virtual MDBErrorCode MDB_IDECL GetNumAdjacencies(MDBEntityHandle element_handle,
                                         MDBEntityType calling_type,
                                         MDBEntityDimension dimension,
                                         int *num_entities) = 0;

  virtual MDBErrorCode MDB_IDECL GetAdjacencies(MDBEntityHandle element_handle,
                                      MDBEntityType calling_type,
                                      MDBEntityDimension dimension,
                                      MDBInt *num_adjacent,
                                      MDBEntityHandle *adj_entities_ptr) = 0;


};


#endif
