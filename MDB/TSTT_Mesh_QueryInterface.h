#ifndef TSTT_MESH_QUERYINTERFACE
#define TSTT_MESH_QUERYINTERFACE

typedef void * TSTT_Entity_Handle;
typedef const void * TSTT_cEntity_Handle;

typedef void * TSTT_Mesh_Handle;
typedef const void * TSTT_cMesh_Handle;

typedef void * TSTT_MeshSet_Handle;
typedef const void * TSTT_cMeshSet_Handle;

typedef int P_INT;
//typedef int TSTT_Entity_Handle;

enum EntityDimension {
  TSTT_0D_ENTITY = 0,
  TSTT_1D_ENTITY,
  TSTT_2D_ENTITY,
  TSTT_3D_ENTITY,
  HIGHEST_DIMENSION
};

enum EntityType {
  TSTT_VERTEX = 0,
  TSTT_EDGE,
  TSTT_FACE,
  TSTT_REGION,
  NUMBER_OF_ENTITY_TYPE
};

enum EntityTopology {
  TSTT_POLYGON = 7, 
  TSTT_TRIANGLE, 
  TSTT_QUADRILATERAL, 
  TSTT_POLYHEDRON, 
  TSTT_TETRAHEDRON, 
  TSTT_HEXAHEDRON, 
  TSTT_PRISM, 
  TSTT_PYRAMID, 
  TSTT_SEPTAHEDRON,
  TSTT_LAST_TOPOLOGY
};
       
class TSTT_Mesh_QueryInterface {

public:
  virtual void  TSTT_Mesh_Load(char *name, int *info)=0;

  virtual void  TSTT_Mesh_Get(char* key, int *value, int *info)=0;
  virtual void  TSTT_Mesh_Get(char* key, float *value, int *info)=0;
  virtual void  TSTT_Mesh_Get(char* key, double *value, int *info)=0;

  virtual void  TSTT_Mesh_GetCoords(int entity_type, char *storage_order, 
                                    double *coords, int *info)=0;

  virtual void  TSTT_Mesh_AddTag(int entity_type, char* name, int tag_size, 
                                 void **value, int *info)=0;

  virtual void  TSTT_Mesh_DeleteTag(int entity_type, char* name, int *info)=0;

  virtual void  TSTT_Mesh_GetEntities(int entity_type, int *num_entities,
                                      TSTT_Entity_Handle *entity_handles, int *info)=0;

  virtual void  TSTT_Mesh_GetEntityAdjacency(int entity_type, int dimension, 
                                             TSTT_Entity_Handle **entity_handles, int **csr_pointer, 
                                             int **csr_data, int *info)=0;

  virtual void  TSTT_Mesh_FreeEntities(int entity_type, 
                                       TSTT_Entity_Handle *entity_handles,
                                       int *info)=0;

  virtual void  TSTT_Mesh_FreeEntityAdjacency(int entity_type, int dimension, 
                                              TSTT_Entity_Handle *entity_handles, int *csr_pointer, 
                                              int *csr_data, int *info)=0;

  virtual void  TSTT_Mesh_Destroy(int *info)=0;

  virtual void  TSTT_Entity_GetDimension(TSTT_Entity_Handle entity_ptr, 
                                         int *dim, int *info)=0;

  virtual void  TSTT_Entity_GetType(TSTT_Entity_Handle entity_ptr, int *type, 
                                    int *info)=0;

  virtual void  TSTT_Entity_GetTopology(TSTT_Entity_Handle entity_ptr, int *topology, 
                                        int *info)=0;

  virtual void  TSTT_Entity_GetID(TSTT_Entity_Handle entity_ptr, int *ID, int *info)=0;

  virtual void  TSTT_Entity_GetCoords(TSTT_Entity_Handle entity_ptr, double *coords, 
                                      int *info)=0;

  virtual void  TSTT_Entity_GetTag(TSTT_Entity_Handle entity_ptr, char *name, 
                                   void **tag_data, int *info)=0;

  virtual void  TSTT_Entity_AddTag(TSTT_Entity_Handle entity_handle, char *name, 
                                   int tag_size, void *value, int *info)=0;

  virtual void  TSTT_Entity_DeleteTag(TSTT_Entity_Handle entity_handle, 
                                      char *name, int *info)=0;

  virtual void  TSTT_Entity_GetNumAdjacencies(TSTT_Entity_Handle entity_ptr, 
                                              int dimension, int *num_entities, int *info)=0;

  virtual void  TSTT_Entity_GetAdjacencies(TSTT_Entity_Handle entity_ptr, 
                                           int dimension, int *num_adjacent, 
                                           TSTT_Entity_Handle **adj_entities_ptr, 
                                           int *info)=0;

  static const char *EntityTypeNames[];
  static const int VerticesPerElement[];

#ifdef KARL_COMPILE
#include "TSTT_MeshSet_QueryInterface.h"
#endif

};

#endif

