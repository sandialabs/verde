#ifndef MDB_MESH_QUERYINTERFACE
#define MDB_MESH_QUERYINTERFACE

#include <map>

#include "MDB.hpp"
#include "TSTT_Mesh_QueryInterface.h"
#include "SparseTagServer.hpp"

class MDB_Mesh_QueryInterface : public TSTT_Mesh_QueryInterface
{

public:
  static MDB_Mesh_QueryInterface *instance() 
  {
    if (0 == instance_) 
      instance_ = new MDB_Mesh_QueryInterface();
    return instance_;
  }
  
  
  virtual void  TSTT_Mesh_Load(char *name, int *info);

  virtual void  TSTT_Mesh_Get(char* key, int *value, int *info);
  virtual void  TSTT_Mesh_Get(char* key, float *value, int *info);
  virtual void  TSTT_Mesh_Get(char* key, double *value, int *info);

  virtual void  TSTT_Mesh_GetCoords(int entity_type, char *storage_order, 
                                    double *coords, int *info);

  virtual void  TSTT_Mesh_AddTag(int entity_type, char* name, int tag_size, 
                                 void **value, int *info);

  virtual void  TSTT_Mesh_DeleteTag(int entity_type, char* name, int *info);

  virtual void  TSTT_Mesh_GetEntities(int entity_type, int *num_entities,
                                      TSTT_Entity_Handle *entity_handles, int *info);

  virtual void  TSTT_Mesh_GetEntityAdjacency(int entity_type, int dimension, 
                                             TSTT_Entity_Handle **entity_handles, int **csr_pointer, 
                                             int **csr_data, int *info);

  virtual void  TSTT_Mesh_FreeEntities(int entity_type, 
                                       TSTT_Entity_Handle *entity_handles,
                                       int *info);

  virtual void  TSTT_Mesh_FreeEntityAdjacency(int entity_type, int dimension, 
                                              TSTT_Entity_Handle *entity_handles, int *csr_pointer, 
                                              int *csr_data, int *info);

  virtual void  TSTT_Mesh_Destroy(int *info);

  virtual void  TSTT_Entity_GetDimension(TSTT_Entity_Handle entity_ptr, 
                                         int *dim, int *info);

  virtual void  TSTT_Entity_GetType(TSTT_Entity_Handle entity_ptr, int *type, 
                                    int *info);

  virtual void  TSTT_Entity_GetTopology(TSTT_Entity_Handle entity_ptr, int *topology, 
                                        int *info);

  virtual void  TSTT_Entity_GetID(TSTT_Entity_Handle entity_ptr, int *ID, int *info);

  virtual void  TSTT_Entity_GetCoords(TSTT_Entity_Handle entity_ptr, double *coords, 
                                      int *info);

  virtual void  TSTT_Entity_GetTag(TSTT_Entity_Handle entity_ptr, char *name, 
                                   void **tag_data, int *info);

  virtual void  TSTT_Entity_AddTag(TSTT_Entity_Handle entity_handle, char *name, 
                                   int tag_size, void *value, int *info);

  virtual void  TSTT_Entity_DeleteTag(TSTT_Entity_Handle entity_handle, 
                                      char *name, int *info);

  virtual void  TSTT_Entity_GetNumAdjacencies(TSTT_Entity_Handle entity_ptr, 
                                              int dimension, int *num_entities, int *info);

  virtual void  TSTT_Entity_GetAdjacencies(TSTT_Entity_Handle entity_ptr, 
                                           int dimension, int *num_adjacent, 
                                           TSTT_Entity_Handle **adj_entities_ptr, 
                                           int *info);

    //! get/set the number of nodes
  int total_num_nodes();
  void total_num_nodes(const int val);
  
    //! get/set the number of elements
  int total_num_elements();
  void total_num_elements(const int val);
  
    //! just use a sparse tag server for all tags for now
  SparseTagServer sparseTagServer;

private:

  static MDB_Mesh_QueryInterface *instance_;

  MDB_Mesh_QueryInterface(){};

    //! store the total number of elements defined in this interface
  int totalNumElements;
  
    //! store the total number of nodes defined in this interface
  int totalNumNodes;
};

inline int MDB_Mesh_QueryInterface::total_num_nodes()
{
  return totalNumNodes;
}

inline void MDB_Mesh_QueryInterface::total_num_nodes(const int val)
{
  totalNumNodes = val;
}

inline int MDB_Mesh_QueryInterface::total_num_elements()
{
  return totalNumElements;
}

inline void MDB_Mesh_QueryInterface::total_num_elements(const int val)
{
  totalNumElements = val;
}
#endif

