//!
//! language-independent definitions (and definitions) for MDB_MeshSet
//!

#include <stdio.h>  // define NULL
#include <vector>   // define STL vectors
#include <algorithm>


#include "MDB_Mesh_QueryInterface.hpp"
#ifdef KARL_COMPILE
#include "MDB_MeshSet.hpp"
#include "MDB_RMeshSet.hpp"
#endif
#include "MDBEntityHandle.hpp"

MDB_Mesh_QueryInterface* MDB_Mesh_QueryInterface::instance_ = 0;
#define TESTING 0
#ifdef TESTING

// Some constants for creating a 10x10x2 mapped mesh
const int i_val = 10;
const int j_val = 10;
const int k_val = 2;

//! Simple hard-coded load routine to test interface
void  MDB_Mesh_QueryInterface::
TSTT_Mesh_Load(char *name, int *info)
{
/*  int counter = 0;

  // define k_val layers of i_val x j_val nodes
  for (int k=0; k < k_val; k++)
  {
    for (int i=0; i < i_val; i++)
    {
      for (int j=0; j < j_val; j++)
      {
        MDBNode* node = new MDBNode((double) i,(double) j,(double) k);
        node->set_id (counter++);
        g_mdb_node.push_back(node);
      }
    }
  }

  counter = 0;
  // create beams between two layers.

  int offset = i_val*j_val;
  for (int i=0; i < i_val; i++)
  {
    for (int j=0; j < j_val; j++)
    {
      MDBBar* bar = new MDBBar(counter, counter+offset);
      bar->set_id(counter++);
      g_mdb_bar.push_back(bar);
    }
  }

  // create one layer of triangles
  // loop to the next to the last row, next to the last node
  int kk = 0;
  for (int i=0; i < i_val-1; i++)
  {
    for (int j=0; i < j_val-1; i++)
    {
      MDBTri* tri = new MDBTri(kk, kk+1, kk+i_val);
      tri->set_id(counter++);
      g_mdb_tri.push_back(tri);

      tri = new MDBTri(kk+1, kk+1+i_val, kk+i_val);
      tri->set_id(counter++);
      g_mdb_tri.push_back(tri);

      kk++;
    }
    kk++;
  }

  // create one layer of quads
  kk = 0;
  for (int i=0; i < i_val-1; i++)
  {
    for (int j=0; i < j_val-1; i++)
    {
      MDBQuad* quad = new MDBQuad(kk, kk+1, kk+1+i_val, kk+i_val);
      quad->set_id(counter++);
      g_mdb_quad.push_back(quad);

      kk++;
    }
    kk++;
  }

  // create one layer of hexes
  kk = 0;
  offset = i_val*j_val;
  for (int i=0; i < i_val-1; i++)
  {
    for (int j=0; i < j_val-1; i++)
    {
      int conn[8];
      conn[0] = kk;
      conn[1] = kk+1;
      conn[2] = kk+1+i_val;
      conn[3] = kk+i_val;
      conn[4] = kk+offset;
      conn[5] = kk+1+offset;
      conn[6] = kk+1+i_val+offset;
      conn[7] = kk+i_val+offset;
      MDBHex* hex = new MDBHex(conn);
      hex->set_id(counter++);
      g_mdb_hex.push_back(hex);

      kk++;
    }
    kk++;
  }

  // create some random tets
  kk = 0;
  for (int i=0; i < i_val-1; i++)
  {
    for (int j=0; i < j_val-1; i++)
    {
      MDBTet* tet = new MDBTet(kk, kk+1, kk+i_val, kk+offset);
      tet->set_id(counter++);
      g_mdb_tet.push_back(tet);

      tet = new MDBTet(kk+1, kk+1+i_val, kk+i_val, kk+1+offset);
      tet->set_id(counter++);
      g_mdb_tet.push_back(tet);

      kk++;
    }
    kk++;
  }

  // create one pyramid
  MDBPyramid* pyramid = new MDBPyramid(0,1,11,10,100);
  pyramid->set_id(counter++);
  g_mdb_pyramid.push_back(pyramid);

  // create one knife
  int conn[7];
  conn[0] = 0;
  conn[0] = 2;
  conn[0] = 12;
  conn[0] = 10;
  conn[0] = 100;
  conn[0] = 101;
  conn[0] = 102;
  MDBKnife* knife = new MDBKnife(conn);
  knife->set_id(counter++);
  g_mdb_knife.push_back(knife);

  *info = 0;
  */
}

#endif

// Begin implementation of TSTT mesh query routines
void MDB_Mesh_QueryInterface::
  TSTT_Mesh_Get(char* key, int *value, int *info) {}

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_Get(char* key, float *value, int *info){}

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_Get(char* key, double *value, int *info){}

//! I guess this is supposed to get all the coordinates in the database. 
void MDB_Mesh_QueryInterface::
  TSTT_Mesh_GetCoords(int entity_type, char *storage_order, 
                                    double *coords, int *info){};

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_AddTag(int entity_type, char* name, int tag_size, 
                                 void **value, int *info){};

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_DeleteTag(int entity_type, char* name, int *info){};


//! Request ALL of the specified entity type in the database
void MDB_Mesh_QueryInterface::
  TSTT_Mesh_GetEntities(int entity_type, int *num_entities,
                        TSTT_Entity_Handle *entity_handles, int *info)
{
  //! For right now require someone to be able allocate space for
  //! all the entities in the database before they can make this 
  //! call.  Don't try to allocate the memory in the database.
  if (!entity_handles)
  {
    *info = 1;
    return;
  }

  switch( entity_type)
  {
    case TSTT_VERTEX:
    {
      //! Get the number of vertices

      //! Copy the ENTIRE list of nodes into the array
      
      *info = 0;
      break; 
    }
    case TSTT_EDGE:
    {
      //! Get the number of bar elements
      
      //! Copy the ENTIRE list of nodes into the array

      *info = 0;
      break;
    }
    case TSTT_FACE:
    case TSTT_REGION:
    default:
      *info = 1;
      break;
  }
}

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_GetEntityAdjacency(int entity_type, int dimension, 
                               TSTT_Entity_Handle **entity_handles, int **csr_pointer, 
                               int **csr_data, int *info)
{};

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_FreeEntities(int entity_type, 
                         TSTT_Entity_Handle *entity_handles,
                         int *info)
{};

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_FreeEntityAdjacency(int entity_type, int dimension, 
                          TSTT_Entity_Handle *entity_handles, int *csr_pointer, 
                          int *csr_data, int *info)
{};

void MDB_Mesh_QueryInterface::
  TSTT_Mesh_Destroy(int *info)
{};

//has this been removed?
void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetDimension(TSTT_Entity_Handle entity_ptr, 
                           int *dim, int *info)
{
  //How do we ensure that this table stays updated?
  static int dimension_table[] = {
    TSTT_0D_ENTITY,    //MDBVertex
    TSTT_2D_ENTITY,    //MDBFace
    TSTT_3D_ENTITY,    //MDBRegion
    TSTT_0D_ENTITY,    //MDBVertex
    TSTT_1D_ENTITY,    //MDBBar
    TSTT_2D_ENTITY,    //MDBTri
    TSTT_2D_ENTITY,    //MDBQuad
    TSTT_3D_ENTITY,    //MDBTet
    TSTT_3D_ENTITY,    //MDBHex
    TSTT_3D_ENTITY,    //MDBPyramid
    TSTT_3D_ENTITY,    //MDBKnife ???
    TSTT_3D_ENTITY,    //MDBWedge
  };

  MDB_Entity_Handle mdb_entity = reinterpret_cast<MDB_Entity_Handle> (entity_ptr);

  if (mdb_entity)
  {
    int type = TYPE_FROM_HANDLE(mdb_entity);
    *dim = dimension_table[type];
    *info = 0;
  }
  else
    *info = 1;
}

void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetType(TSTT_Entity_Handle entity_ptr, int *type, 
                      int *info)
{
  //How do we ensure that this table stays updated?
  static int type_table[] = {
    TSTT_VERTEX,       //MDBVertex
    TSTT_FACE,         //MDBFace
    TSTT_REGION,       //MDBRegion
    TSTT_VERTEX,       //MDBVertex
    TSTT_EDGE,         //MDBBar
    TSTT_2D_ENTITY,    //MDBTri
    TSTT_2D_ENTITY,    //MDBQuad
    TSTT_3D_ENTITY,    //MDBTet
    TSTT_3D_ENTITY,    //MDBHex
    TSTT_3D_ENTITY,    //MDBPyramid
    TSTT_3D_ENTITY,    //MDBKnife ???
    TSTT_3D_ENTITY,    //MDBWedge
  };

  MDB_Entity_Handle mdb_entity = reinterpret_cast<MDB_Entity_Handle> (entity_ptr);

  if (mdb_entity)
  {
    int mdb_type = TYPE_FROM_HANDLE(mdb_entity);
    *type = type_table[mdb_type];
    *info = 0;
  }
  else
    *info = 1;
}

void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetTopology(TSTT_Entity_Handle entity_ptr, int *topology, 
                          int *info)
{
  //How do we ensure that this table stays updated?
  static int topology_table[] = {
    TSTT_VERTEX,          //MDBVertex
    TSTT_FACE,            //MDBFace
    TSTT_REGION,          //MDBRegion
    TSTT_VERTEX,          //MDBVertex
    TSTT_EDGE,            //MDBBar
    TSTT_TRIANGLE,        //MDBTri
    TSTT_QUADRILATERAL,   //MDBQuad
    TSTT_TETRAHEDRON,     //MDBTet
    TSTT_HEXAHEDRON,      //MDBHex
    TSTT_PYRAMID,         //MDBPyramid
    TSTT_PRISM,           //MDBKnife ???
    TSTT_PRISM,           //MDBWedge
  };

  MDB_Entity_Handle mdb_entity = reinterpret_cast<MDB_Entity_Handle> (entity_ptr);

  if (mdb_entity)
  {
    int type = TYPE_FROM_HANDLE(mdb_entity);
    // map from type to topology table.
    *topology = topology_table[type];
    *info = 0;
  }
  else
    *info = 1;
}

/*! ID's in the MDB interface are slightly different than in the
 * TSTT database.  Each entity type has its own id space.  This
 * id is part of the MDB_Entity_Handle.  The ids are not guaranteed
 * to be contiguous, but they are guaranteed to be constant.  There
 * may be an external id attached to the entity.  We will have to
 * decide which one to return through this interface.
 */
void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetID(TSTT_Entity_Handle entity_ptr, int *ID, int *info)
{
  MDB_Entity_Handle mdb_entity = reinterpret_cast<MDB_Entity_Handle> (entity_ptr);

  if (mdb_entity)
  {
    *ID = ID_FROM_HANDLE(mdb_entity);
    *info = 0;
  }
  else
    *info = 1;
}

void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetCoords(TSTT_Entity_Handle entity_ptr, double *coords, 
                        int *info)
{
  MDB_Entity_Handle mdb_entity = reinterpret_cast<MDB_Entity_Handle> (entity_ptr);

  if (mdb_entity)
  {
    int err_code;// = mdb_entity->coordinates((double*) coords);
    
    if (err_code != 1) // need some real error code
    {
      *info = 0;
    }
    else
      *info = 1;
  }
  else
    *info = 1;
}

void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetTag(TSTT_Entity_Handle entity_ptr, char *name, 
                     void **tag_data, int *info)
{};

void MDB_Mesh_QueryInterface::
  TSTT_Entity_AddTag(TSTT_Entity_Handle entity_handle, char *name, 
                     int tag_size, void *value, int *info)
{};

void MDB_Mesh_QueryInterface::
  TSTT_Entity_DeleteTag(TSTT_Entity_Handle entity_handle, 
                        char *name, int *info)
{};

void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetNumAdjacencies(TSTT_Entity_Handle entity_ptr, 
                                int dimension, int *num_entities, int *info)
{};

void MDB_Mesh_QueryInterface::
  TSTT_Entity_GetAdjacencies(TSTT_Entity_Handle entity_ptr, 
                             int dimension, int *num_adjacent, 
                             TSTT_Entity_Handle **adj_entities_ptr, 
                             int *info)
{
  MDB_Entity_Handle mdb_entity = reinterpret_cast<MDB_Entity_Handle> (entity_ptr);

  if (mdb_entity)
  {
    //very interesting.  The connectivity array should be a list of Handles not ids
    int err_code;// = mdb_entity->connectivity( int* conn );
    
    if (err_code != 1) // some failure value
    {
      *info = 0;
    }
    else
      *info = 1;
  }
  else
    *info = 1;

}

#ifdef KARL_COMPILE
  //! get the value of a tag with the specified name on this meshset;
  //! return *info != 0 if the tag doesn't exist on this meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetTag(const MDB_MeshSet* meshset_handle, const char* name, 
                    int *tag_size, 
                    unsigned char **value, int *info) const
{
  std::string temp_name(name);
  TagInfo temp_tag;
  meshset_handle->GetTag(temp_name, temp_tag);

  CHECK_SIZE(value, *tag_size, temp_tag.tag_size, unsigned char);
  
  memcpy(*value, temp_tag.data, *tag_size);
}

  //! add a tag with the specified name on this meshset;
  //! return *info != 0 if the addition isn't successful
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_AddTag(MDB_MeshSet* meshset_handle, const char* name, 
                    const int tag_size, 
                    const void * const value, int *info) 
{
  const std::string temp_name(name, tag_size);
  const struct TagInfo temp_tag = {(unsigned char * const)value, tag_size};
  meshset_handle->AddTag(temp_name, temp_tag, info);
}

  //! delete a tag with the specified name from this meshset;
  //! return *info != 0 if the addition isn't successful
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_DeleteTag(MDB_MeshSet* meshset_handle, const char* name, 
                       int *info) 
{
  const std::string temp_name(name);
  meshset_handle->DeleteTag(temp_name, info);
}

  //! return handles to all the meshsets which have been defined
  //! (should this really be here, or should it be a function/data on
  //! the TSTT_Mesh instance?)
  //! also, this should return a language-indep. iterator, not an array
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetMeshSets(MDB_MeshSet* **meshset_handles, int *num_entities,
                         int *info) const
{
  CHECK_SIZE(meshset_handles, *num_entities, MDB_MeshSet::GlobalMeshSets().size(), MDB_MeshSet*);
  std::copy(MDB_MeshSet::GlobalMeshSets().begin(), 
            MDB_MeshSet::GlobalMeshSets().end(), 
            *meshset_handles);
}

  //! return the meshsets contained in this meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetMeshSets(MDB_MeshSet* meshset_handle, 
                         MDB_MeshSet* **set_handles, 
                         int *num_sets, int *info) const
{
  CHECK_SIZE(set_handles, *num_sets, meshset_handle->MeshSetMeshSets().size(), MDB_MeshSet*);
  std::copy(meshset_handle->MeshSetMeshSets().begin(), 
            meshset_handle->MeshSetMeshSets().end(), *set_handles);
  *num_sets = meshset_handle->MeshSetMeshSets().size();
}

  //! like GetMeshSets, but recurses down meshsets
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetAllMeshSets(MDB_MeshSet* meshset_handle, 
                            MDB_MeshSet* **set_handles, 
                            int *num_sets, int *info) const
{
  std::vector<MDB_MeshSet*> these_meshsets;
  meshset_handle->GetAllMeshSets(these_meshsets, 1, info);
  CHECK_SIZE(set_handles, *num_sets, these_meshsets.size(), MDB_MeshSet*);
  std::copy(these_meshsets.begin(), these_meshsets.end(), *set_handles);
  *num_sets = these_meshsets.size();
}

  //! return handles to all the entities contained in this meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetEntities(MDB_MeshSet* meshset_handle, 
                         TSTT_Entity_Handle **entity_handles, 
                         int *num_entities, int *info) const
{
  std::vector<TSTT_Entity_Handle> these_entities;
  CHECK_SIZE(entity_handles, *num_entities, meshset_handle->MeshSetEntities().size(), 
             TSTT_Entity_Handle);
  std::copy(meshset_handle->MeshSetEntities().begin(), 
            meshset_handle->MeshSetEntities().end(), 
            *entity_handles);
  *num_entities = meshset_handle->MeshSetEntities().size();
}

  //! like GetEntities, but recurses down contained MeshSets
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetAllEntities(MDB_MeshSet* meshset_handle, int *num_entities,
                            TSTT_Entity_Handle **entity_handles, int *info) const
{
  std::vector<TSTT_Entity_Handle> these_entities;
  meshset_handle->GetAllEntities(these_entities, info);
  CHECK_SIZE(entity_handles, *num_entities, these_entities.size(),
             TSTT_Entity_Handle);
  std::copy(these_entities.begin(), these_entities.end(), *entity_handles);
  *num_entities = these_entities.size();
}

  //! return the entities with the specified type in this meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetEntitiesByType(MDB_MeshSet* meshset_handle, 
                               const int entity_type,
                               TSTT_Entity_Handle **entity_handles, 
                               int *num_entities, int *info) const
{
  std::vector<TSTT_Entity_Handle> these_entities;
  meshset_handle->GetEntitiesByType(entity_type, these_entities, info);
  CHECK_SIZE(entity_handles, *num_entities, these_entities.size(),
             TSTT_Entity_Handle);
  std::copy(these_entities.begin(), these_entities.end(), *entity_handles);
  *num_entities = these_entities.size();
}
                            
  //! return the entities with the specified topology in this meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetEntitiesByTopology(MDB_MeshSet* meshset_handle, 
                                   const int entity_topology,
                                   TSTT_Entity_Handle **entity_handles, 
                                   int *num_entities, int *info) const
{
  std::vector<TSTT_Entity_Handle> these_entities;
  meshset_handle->GetEntitiesByTopology(entity_topology, these_entities, info);
  CHECK_SIZE(entity_handles, *num_entities, these_entities.size(),
             TSTT_Entity_Handle);
  std::copy(these_entities.begin(), these_entities.end(), *entity_handles);
  *num_entities = these_entities.size();
}
                            
  //! get relations between sets; assumes hierarchical (parent/child) relationships; if
  //! num_hops = -1, gets *all* parents or children over all hops
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetChildren(MDB_MeshSet* from_meshset, const int num_hops,
                         int *num_sets, MDB_MeshSet* **set_handles, int *info) const
{
  std::vector<MDB_MeshSet*> these_meshsets;
  from_meshset->GetChildren(num_hops, these_meshsets, info);
  CHECK_SIZE(set_handles, *num_sets, these_meshsets.size(),
             MDB_MeshSet*);
  std::copy(these_meshsets.begin(), these_meshsets.end(), *set_handles);
  *num_sets = these_meshsets.size();
}

void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetParents(MDB_MeshSet* from_meshset, const int num_hops,
                        int *num_sets,
                        MDB_MeshSet* **set_handles, int *info) const
{
  std::vector<MDB_MeshSet*> these_meshsets;
  from_meshset->GetParents(num_hops, these_meshsets, info);
  CHECK_SIZE(set_handles, *num_sets, these_meshsets.size(),
             MDB_MeshSet*);
  std::copy(these_meshsets.begin(), these_meshsets.end(), *set_handles);
  *num_sets = these_meshsets.size();
}

  //! create an empty meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_Create(MDB_MeshSet* *meshset_created, int *info) 
{
  *meshset_created = new MDB_MeshSet();
  if (NULL == *meshset_created) *info = 1;
}

  //! add num_entities entities to a meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_AddEntities(MDB_MeshSet* meshset, 
                         const TSTT_Entity_Handle * const entity_handles,
                         const int num_entities, int *info) 
{
  meshset->AddEntities(entity_handles, num_entities, info);
}

  //! add num_meshsets meshsets to a meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_AddMeshSets(MDB_MeshSet* meshset, 
                         MDB_MeshSet **meshset_handles,
                         const int num_meshsets, int *info)
{
  meshset->AddMeshSets(meshset_handles, num_meshsets, info);
}

  //! remove num_entities entities from the MeshSet; returns non-zero if
  //! some entities were not in the meshset to begin with
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_RemoveEntities(MDB_MeshSet* meshset, 
                            const TSTT_Entity_Handle *const entity_handles,
                            const int num_entities,
                            int *info)
{
  meshset->RemoveEntities(entity_handles, num_entities, info);
}

  //! remove num_meshsets meshsets from the meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_RemoveMeshSets(MDB_MeshSet* meshset, 
                            MDB_MeshSet **meshset_handles,
                            const int num_meshsets, int *info)
{
  meshset->RemoveMeshSets(meshset_handles, num_meshsets, info);
}

  //! return the number of entities in the meshset, NOT including meshsets
const int MDB_Mesh_QueryInterface::
TSTT_MeshSet_NumEntities(const MDB_MeshSet* meshset, int *info) const
{
  return meshset->NumEntities(info);
}

  //! return the number of meshsets in the meshset
const int MDB_Mesh_QueryInterface::
TSTT_MeshSet_NumMeshSets(const MDB_MeshSet* meshset, int *info) const
{
  return meshset->NumMeshSets(info);
}

  //! return the number of entities in the meshset with the specified type and/or topology
  //! (NOT including meshsets)
const int MDB_Mesh_QueryInterface::
TSTT_MeshSet_NumEntitiesByType(const MDB_MeshSet* meshset, 
                               const int entity_type,
                               int *info) const
{
  return meshset->NumEntitiesByType(entity_type, info);
}
  
const int MDB_Mesh_QueryInterface::
TSTT_MeshSet_NumEntitiesByTopology(const MDB_MeshSet* meshset, 
                                   const int entity_topology,
                                   int *info) const
{
  return meshset->NumEntitiesByTopology(entity_topology, info);
}

  //! return the number of child/parent relations for this meshset
const int MDB_Mesh_QueryInterface::
TSTT_MeshSet_NumChildren(const MDB_MeshSet* meshset, 
                         int *info) const
{
  return meshset->NumChildren(info);
}

const int MDB_Mesh_QueryInterface::
TSTT_MeshSet_NumParents(const MDB_MeshSet* meshset, 
                        int *info) const
{
  return meshset->NumParents(info);
}

  //! get/set the name of this meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetName(const MDB_MeshSet* meshset,
                     char **name_string, int *name_length,
                     int *info) const
{
  const std::string this_name = meshset->GetName(info); 
  CHECK_SIZE(name_string, *name_length, this_name.size(), char);
  strcpy(*name_string, this_name.c_str());
}
    
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_SetName(MDB_MeshSet* meshset,
                     const char *name_string, const int name_length,
                     int *info) 
{
  meshset->SetName(std::string(name_string, name_length), info);
}
    
  //! get/set the id of this meshset
void MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetId(const MDB_MeshSet* meshset,
                   int *id, int *info) const
{
  *id = meshset->GetId(info);
}

void MDB_Mesh_QueryInterface::
TSTT_MeshSet_SetId(MDB_MeshSet* meshset,
                   const int id, int *info) 
{
  meshset->SetId(id, info);
}

const int MDB_Mesh_QueryInterface::
TSTT_MeshSet_GetMark(const MDB_MeshSet* meshset, int *info) const
{
  return meshset->GetMark(info);
}

void MDB_Mesh_QueryInterface::
TSTT_MeshSet_SetMark(MDB_MeshSet *meshset, const int flag, int *info) 
{
  meshset->SetMark(flag, info);
}

bool MDB_Mesh_QueryInterface::is_in_rmeshset (TSTT_Entity_CHandle entity_handle, 
                                              const MDB_RMeshSet *rmeshset) 
{
    // compare entity handle to rmeshset offset & range
  return (entity_handle > rmeshset->elem_id_offset() &&
          entity_handle <= rmeshset->elem_id_offset()+rmeshset->num_elements());
}


MDB_RMeshSet *MDB_Mesh_QueryInterface::find_rmeshset(TSTT_Entity_CHandle element_handle)
{
  if (0 > element_handle || totalNumElements <= element_handle)
    return NULL;

  std::map<int,MDB_RMeshSet*>::iterator pos = mdbRMeshSets.find(element_handle);
  if (pos == mdbRMeshSets.end())
    return NULL;
  else
    return (*pos).second;
}

#endif

