#include "MDB_RMeshSet.hpp"
#include "SparseTagServer.hpp"
#include <string>
#include <utility>
#include <assert.h>
#include <stdio.h>

std::map<int, MDB_RMeshSet*> MDB_RMeshSet::globalRMeshSets[TSTT_LAST_TOPOLOGY];
SparseTagServer MDB_RMeshSet::sparseTagServer;

  //! nodal form of constructor
MDB_RMeshSet::MDB_RMeshSet(int num_nodes,
                           double *node_x, double *node_y, double *node_z,
                           const int node_id_offset,
                           const int prev_node_offset) 
    : entityIdOffset(node_id_offset), 
      connectivityTag(-1),
      xcoordTag(-1),
      ycoordTag(-1),
      zcoordTag(-1)
{

  assert(0 != num_nodes);
  
    // this is a constructor for a vertex
  entityType = TSTT_VERTEX;
  entityTopology = TSTT_LAST_TOPOLOGY;

  numEntities = num_nodes;

    // load the nodes; creates tags for them at the same time
  set_node_x(node_id_offset, node_id_offset+num_nodes-1,
             node_x, num_nodes);
  set_node_y(node_id_offset, node_id_offset+num_nodes-1,
             node_y, num_nodes);
  set_node_z(node_id_offset, node_id_offset+num_nodes-1,
             node_z, num_nodes);

  add_rmeshset(this);
}

  //! element form of constructor
MDB_RMeshSet::MDB_RMeshSet(int num_elems,
                           int *connect_array,
                           const int elem_id_offset,
                           EntityType entity_type,
                           EntityTopology entity_topology) 
    : entityIdOffset(elem_id_offset),
      connectivityTag(-1),
      xcoordTag(-1),
      ycoordTag(-1),
      zcoordTag(-1),
      entityType(entity_type),
      entityTopology(entity_topology)
{
  numEntities = num_elems;

  verticesPerElement = TSTT_Mesh_QueryInterface::VerticesPerElement[entity_topology];
  assert(verticesPerElement > 0);

    // load the element data
  bool result = set_elem_connectivity(entityIdOffset, entityIdOffset+num_elems-1,
                                      connect_array, num_elems);

  add_rmeshset(this);
}

bool MDB_RMeshSet::add_rmeshset(MDB_RMeshSet *rmeshset) 
{
  int index = (rmeshset->entity_topology() == TSTT_LAST_TOPOLOGY ? rmeshset->entity_type() : 
               rmeshset->entity_topology());
  (globalRMeshSets[index])[rmeshset->entity_id_offset()] =
    rmeshset;
  
  return true;
}

MDB_RMeshSet::~MDB_RMeshSet() 
{
  int index = (entityTopology == TSTT_LAST_TOPOLOGY ? entityType : entityTopology);
  
  int result = globalRMeshSets[index].erase(entityIdOffset);
  assert(result == 1);
}

int MDB_RMeshSet::make_coord_tag(const char *name) const
{
  int this_handle = (int) this;
  char tag_name[80];
  sprintf(tag_name, "%s%d", name, this_handle);
  
  int this_tag = 
    const_cast<MDB_RMeshSet*>(this)->sparseTagServer.add_tag(tag_name,
                                                             entityType,
                                                             sizeof(double),
                                                             SparseTagInfo::TAGPROP_SPARSE);
  if (-1 == this_tag)
    std::cout << "Problems creating " << tag_name << " tag!" << std::endl;

  return this_tag;
}

int MDB_RMeshSet::make_connectivity_tag() const
{
  int this_handle = (int) this;
  char tag_name[80];
  sprintf(tag_name, "connect%s%d", 
          TSTT_Mesh_QueryInterface::EntityTypeNames[entityType], this_handle);
  
  int this_tag = 
    const_cast<MDB_RMeshSet*>(this)->sparseTagServer.add_tag(tag_name,
                                                             entityType,
                                                             sizeof(int)*verticesPerElement,
                                                             SparseTagInfo::TAGPROP_SPARSE);
  if (-1 == this_tag)
    std::cout << "Problems creating " << 
      TSTT_Mesh_QueryInterface::EntityTypeNames[entityType] << " tag!" << std::endl;

  return this_tag;
}

/*
VerdeVector MDB_RMeshSet::get_coordinates(const int index) const
{
  int one = 1;
  double resultx, resulty, resultz;
  double *resultxp = &resultx;
  double *resultyp = &resulty;
  double *resultzp = &resultz;
  
  bool status = get_coordinate(index, index, &resultxp, &one, xcoordTag);
  assert(status);
  status = get_coordinate(index, index, &resultyp, &one, ycoordTag);
  assert(status);
  status = get_coordinate(index, index, &resultzp, &one, zcoordTag);
  assert(status);
  return VerdeVector(resultx, resulty, resultz);
}
*/

bool MDB_RMeshSet::get_coordinate(const int begin_handle, const int end_handle,
                                  double **vec, int *size_vec, const int tag_handle) const
{
  int num_handles = end_handle - begin_handle + 1;
  assert(num_handles <= numEntities && begin_handle <= entityIdOffset);
  CHECK_SIZE(*vec, *size_vec, num_handles, double);
  
  int i;
  bool result;
  for (i = 0; i < num_handles; i++) {
    result = sparseTagServer.get_data(tag_handle, begin_handle+i, &((*vec)[i]));
    assert(result == true);
  }

  return true;
}

bool MDB_RMeshSet::set_coordinate(const int begin_handle, const int end_handle,
                                  const double *vec, const int size_vec, 
                                  const int tag_handle)
{
  int num_handles = end_handle - begin_handle + 1;
  assert(num_handles <= numEntities && begin_handle >= entityIdOffset &&
         size_vec == num_handles);
  
  int i;
  bool result;
  for (i = 0; i < num_handles; i++) {
    result = sparseTagServer.set_data(tag_handle, begin_handle+i, &(vec[i]));
    assert(result == true);
  }

  return true;
}

bool MDB_RMeshSet::elem_connectivity(const int begin_handle,
                                     const int end_handle,
                                     int **vec, int *vec_length) const
{
  assert(begin_handle <= entityIdOffset &&
         end_handle-begin_handle+1 <= numEntities);
  
  int target_size = (end_handle-begin_handle+1)*verticesPerElement;
  CHECK_SIZE(*vec, *vec_length, target_size, int);
  
  int i;
  bool result;
  for (i = begin_handle; i <= end_handle; i++) {
    result = sparseTagServer.get_data(connectivity_tag(), i,
                                      &((*vec)[(i-entityIdOffset)*verticesPerElement]));
    assert(result == true);
  }

  return true;
}

bool MDB_RMeshSet::set_elem_connectivity(const int begin_handle,
                                         const int end_handle,
                                         const int *vec, 
                                         const int vec_length) const
{
  assert(begin_handle <= entityIdOffset &&
         end_handle-begin_handle+1 <= numEntities &&
         vec_length >= (end_handle-begin_handle+1));
  
  int i;
  bool result;
  for (i = begin_handle; i <= end_handle; i++) {
    result = sparseTagServer.set_data(connectivity_tag(), i,
                                      &(vec[(i-entityIdOffset)*verticesPerElement]));
    assert(result == true);
  }

  return true;
}
  
  //! finds if the entity_handle is in the RMeshSet
bool MDB_RMeshSet::is_in_rmeshset (TSTT_cEntity_Handle entity_handle, 
                                   const MDB_RMeshSet *rmeshset)
{
  const int this_handle = reinterpret_cast<const int>(entity_handle);
  if (this_handle >= rmeshset->entity_id_offset() &&
      this_handle < rmeshset->entity_id_offset()+rmeshset->num_entities()) return true;
  else return false;
}

  //! finds if the entity_handle is in the RMeshSet
bool MDB_RMeshSet::is_in_rmeshset (TSTT_cEntity_Handle entity_handle)
{
  const int this_handle = reinterpret_cast<const int>(entity_handle);
  if (this_handle >= entity_id_offset() &&
      this_handle < entity_id_offset()+num_entities()) return true;
  else return false;
}

MDB_RMeshSet *MDB_RMeshSet::find_rmeshset(const EntityType my_type,
                                          const EntityTopology my_topology,
                                          TSTT_cEntity_Handle entity_handle)
{
  int index = (my_type == TSTT_VERTEX ? TSTT_VERTEX : my_topology);
  const int this_handle = reinterpret_cast<const int>(entity_handle);
  std::map<int,MDB_RMeshSet*>::iterator 
    this_it = globalRMeshSets[index].lower_bound(this_handle);

    // lower_bound returned the lowest possible point in the map that index could
    // occupy; if it's at the beginning of the map, that means this index would be
    // smaller than the offsets of all rms's of this type, meaning this index doesn't
    // occur in any rms's
  if (this_it == globalRMeshSets[index].begin()) return NULL;

    // otherwise, this index might be equal to the offset of an rms in the list
  else if ((*this_it).first == this_handle) return (*this_it).second;
  
    // otherwise, the previous rms has the next-lower offset, so we need to check
    // to see if index is in that rms's range
  this_it--;
  
  MDB_RMeshSet *this_set = (*this_it).second;
  if (this_handle >= this_set->entity_id_offset() &&
      this_handle < this_set->entity_id_offset()+this_set->num_entities())
    return this_set;
  else return NULL;
}
