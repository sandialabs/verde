//
//-------------------------------------------------------------------------
// Filename      : MDB_MeshSet.cpp 
// Creator       : Tim Tautges
//
// Date          : 02/01/02
//
// Owner         : Tim Tautges
//
// Description   : MDB_MeshSet is the MDB implementation of MeshSet
//-------------------------------------------------------------------------


#ifdef WIN32
#ifdef _DEBUG
// turn off warnings that say they debugging identifier has been truncated
// this warning comes up when using some STL containers
#pragma warning(disable : 4786)
#endif
#endif


#include <algorithm>
#include <utility>
#include "assert.h"

#include "MDB_MeshSet.hpp"
//#include "MDB_Mesh_QueryInterface.hpp"


std::set<MDB_MeshSet*> MDB_MeshSet::globalMeshSets;
MeshSet_NameC MDB_MeshSet::meshSetNames;
MeshSet_IdC MDB_MeshSet::meshSetIds;
int MDB_MeshSet::maxMeshSetId = 0;

MDB_MeshSet::MDB_MeshSet() 
{
  meshSetId = maxMeshSetId++;
  meshSetIds.insert(std::pair<int,MDB_MeshSet*>(meshSetId,this));
  globalMeshSets.insert(this);
}

MDB_MeshSet::MDB_MeshSet( MDBRange<MDBEntityHandle> entities) 
{
  meshSetId = maxMeshSetId++;
  meshSetIds.insert(std::pair<int,MDB_MeshSet*>(meshSetId,this));
  meshSetEntities.merge(entities);
  globalMeshSets.insert(this);
}

MDB_MeshSet::~MDB_MeshSet() 
{
  meshSetIds.erase(meshSetId);
  globalMeshSets.erase(this);
}

  //! recursive implementation; if first_call is true, this is
  //! the first call, otherwise this is the recursive call
void MDB_MeshSet::GetAllMeshSets(std::vector<MDB_MeshSet*> &handle_list,
                                 const bool first_call,
                                 int *info)
{
  if (true != first_call) handle_list.push_back(this);
  
  std::vector<MDB_MeshSet*>::iterator i;
  for (i = meshSetMeshSets.begin();
       i != meshSetMeshSets.end(); i++) {
    (*i)->GetAllMeshSets(handle_list, false, info);
  }
}

  //! recursive implementation; if first_call is true, this is
  //! the first call, otherwise this is the recursive call
void MDB_MeshSet::GetAllMeshSets(std::vector<const MDB_MeshSet*> &handle_list,
                                 const bool first_call,
                                 int *info) const
{
  if (true != first_call) handle_list.push_back(this);
  
  std::vector<MDB_MeshSet*>::const_iterator i;
  for (i = meshSetMeshSets.begin();
       i != meshSetMeshSets.end(); i++) {
    (*i)->GetAllMeshSets(handle_list, false, info);
  }
}

void MDB_MeshSet::GetAllEntities(std::vector<int> &handle_list,
                                 int *info) const
{
#ifdef KARL_COMPILE
  handle_list.reserve(handle_list.size()+meshSetEntities.size());
  copy(meshSetEntities.begin(), meshSetEntities.end(), back_inserter(handle_list));
  
  std::vector<MDB_MeshSet*>::iterator i;
  for (i = meshSetMeshSets.begin();
       i != meshSetMeshSets.end(); i++) {
    (*i)->GetAllEntities(handle_list, info); 
  }
#endif
}

void MDB_MeshSet::GetEntitiesByType(const int entity_type,
                                    std::vector<int> &entity_list,
                                    int *info) const
{ 
  // eventually, we should sort entities by type & topology
  MDBRange<MDBEntityHandle>::const_iterator i = meshSetEntities.begin();
  int this_type;
  
  for ( ; i != meshSetEntities.end(); i++) {
    this_type = TYPE_FROM_HANDLE((*i));
    if (this_type == entity_type) 
      entity_list.push_back(*i);
  }
}
                            
void MDB_MeshSet::GetEntitiesByTopology(const int entity_topology,
                                        std::vector<int> &entity_list,
                                        int *info) const
{ 
    // eventually, we should sort entities by type & topology
  MDBRange<MDBEntityHandle>::const_iterator i = meshSetEntities.begin();
  int this_topology;
  
  for ( ; i != meshSetEntities.end(); i++) {
    this_topology = TYPE_FROM_HANDLE((*i));
    if (this_topology == entity_topology) 
      entity_list.push_back(*i);
  }
}
                            
  //! subtract/intersect/unite meshset_2 from/with/into meshset_1; modifies meshset_1
void MDB_MeshSet::Subtract(MDB_MeshSet* meshset_1, 
                           MDB_MeshSet* meshset_2, int *info) 
{
    // difference the entities
  std::vector<int> temp;
#ifdef KARL_COMPILE
  set_difference(meshset_1->meshSetEntities.begin(), meshset_1->meshSetEntities.end(), 
                 meshset_2->meshSetEntities.begin(), meshset_2->meshSetEntities.end(), 
                 inserter(temp, temp.begin()));
  meshset_1->meshSetEntities.swap(temp);
  
    // difference the meshsets
  std::vector<MDB_MeshSet*> temps;
  set_difference(meshset_1->meshSetMeshSets.begin(), meshset_1->meshSetMeshSets.end(), 
                 meshset_2->meshSetMeshSets.begin(), meshset_2->meshSetMeshSets.end(), 
                 back_inserter(temps));
  meshset_1->meshSetMeshSets.swap(temps);
#endif
}
void MDB_MeshSet::Intersect(MDB_MeshSet* meshset_1, 
                            MDB_MeshSet* meshset_2, 
                            int &proper1, int &proper2,
                            int *info) 
{ 
#ifdef KARL_COMPILE 
  proper1 = proper2 = 0;
  int temp_info;

    // intersect the entities
  std::vector<int> temp;
  
  set_intersection(meshset_1->meshSetEntities.begin(), meshset_1->meshSetEntities.end(), 
                   meshset_2->meshSetEntities.begin(), meshset_2->meshSetEntities.end(), 
                   back_inserter(temp));
  if (temp.size() == meshset_1->NumEntities(&temp_info) && 
      temp.size() < meshset_2->NumEntities(&temp_info)) proper1 = 1;
  if (temp.size() == meshset_2->NumEntities(&temp_info) && 
      temp.size() < meshset_1->NumEntities(&temp_info)) proper2 = 1;
  
  meshset_1->meshSetEntities.swap(temp);
  
    // intersect the meshsets
  std::vector<MDB_MeshSet*> temps;
  set_intersection(meshset_1->meshSetMeshSets.begin(), meshset_1->meshSetMeshSets.end(), 
                   meshset_2->meshSetMeshSets.begin(), meshset_2->meshSetMeshSets.end(), 
                   back_inserter(temps));
 
  if (proper1 && 
      (temps.size() == meshset_1->NumMeshSets(&temp_info) ||
       temps.size() >= meshset_2->NumMeshSets(&temp_info))) proper1 = 0;
  if (proper2 && 
      (temps.size() == meshset_2->NumMeshSets(&temp_info) ||
       temps.size() >= meshset_1->NumMeshSets(&temp_info))) proper2 = 0;

  meshset_1->meshSetMeshSets.swap(temps);

  *info = temp_info;
#endif
}

void MDB_MeshSet::Unite(MDB_MeshSet *meshset_1, 
                        MDB_MeshSet *meshset_2, int *info) 
{ 
#ifdef KARL_COMPILE
    // unite the entities
  std::vector<int> temp;
  set_union(meshset_1->meshSetEntities.begin(), meshset_1->meshSetEntities.end(), 
            meshset_2->meshSetEntities.begin(), meshset_2->meshSetEntities.end(), 
            inserter(temp, temp.begin()));
  meshset_1->meshSetEntities.swap(temp);
  
    // unite the meshsets
  std::vector<MDB_MeshSet*> temps;
  set_union(meshset_1->meshSetMeshSets.begin(), meshset_1->meshSetMeshSets.end(), 
            meshset_2->meshSetMeshSets.begin(), meshset_2->meshSetMeshSets.end(), 
            back_inserter(temps));
  meshset_1->meshSetMeshSets.swap(temps);
#endif
}

void MDB_MeshSet::GetChildren(const int num_hops, std::vector<MDB_MeshSet*> &children,
                              int *info) const
{
    // compute new value of num_hops, either decremented or equal to -1
  int this_hops = (-1 == num_hops ? -1 : num_hops-1);

  std::vector<MDB_MeshSet*>::const_iterator i = childMeshSets.begin();
    // go through child sets; if we find a unique child, and we haven't exceeded
    // the hops, recurse
  while (i != childMeshSets.end()) {
    std::vector<MDB_MeshSet*>::iterator j = std::find(children.begin(), children.end(), *i);
    if (j == children.end()) {
      children.push_back(*i);
      if (0 != num_hops)
        (*i)->GetChildren(this_hops, children, info);
    }
    i++;
  }
}
void MDB_MeshSet::GetParents(const int num_hops, std::vector<MDB_MeshSet*> &parents, 
                             int *info) const
{ 
    // compute new value of num_hops, either decremented or equal to -1
  int this_hops = (-1 == num_hops ? -1 : num_hops-1);

  std::vector<MDB_MeshSet*>::const_iterator i = parentMeshSets.begin();
    // go through parent sets; if we find a unique parent, and we haven't exceeded
    // the hops, recurse
  while (i != parentMeshSets.end()) {
    std::vector<MDB_MeshSet*>::iterator j = std::find(parents.begin(), parents.end(), *i);
    if (j == parents.end()) {
      parents.push_back(*i);
      if (0 != num_hops)
        (*i)->GetParents(this_hops, parents, info);
    }
    i++;
  }
}

  //! returns non-zero if meshsets *are* related topologically
const int MDB_MeshSet::IsRelated(MDB_MeshSet* meshset_1, 
                                 MDB_MeshSet* meshset_2, 
                                 int *info)
{ 
    // test for trivial case...
  if (meshset_1 == meshset_2 || 
      std::find(meshset_1->ParentMeshSets().begin(), 
                meshset_1->ParentMeshSets().end(), meshset_2) ||
      std::find(meshset_1->ChildMeshSets().begin(), 
                meshset_1->ChildMeshSets().end(), meshset_2))
    return 1;
  
    // ok, non-trivial case; check parents first, then children
  std::vector<MDB_MeshSet*> temp;
  int error;
  meshset_1->GetParents(-1, temp, &error);
  if (std::find(temp.begin(), temp.end(), meshset_2) != temp.end()) return 1;
  temp.erase(temp.begin(), temp.end());

    // ok, meshset_2 isn't a parent of meshset_1; check the opposite
  meshset_2->GetParents(-1, temp, &error);
  if (std::find(temp.begin(), temp.end(), meshset_1) != temp.end()) return 1;
  
    // didn't find a parent
  return 0;
}

void MDB_MeshSet::AddEntities(const int *entity_handles,
                              const int num_entities, int *info) 
{ 
  //meshSetEntities.insert(meshSetEntities.end(),
  //                       entity_handles, entity_handles+num_entities);
  for(int i=0; i< num_entities; i++)
    meshSetEntities.insert(entity_handles[i]);
}

void MDB_MeshSet::AddEntities(const MDBRange<MDBEntityHandle> &entities,
                              int *info) 
{
  meshSetEntities.merge(entities);	
}

void MDB_MeshSet::AddMeshSets(MDB_MeshSet **meshset_handles,
                              const int num_meshsets, int *info) 
{ 
  meshSetMeshSets.insert(meshSetMeshSets.end(),
                         meshset_handles, meshset_handles+num_meshsets);
}

void MDB_MeshSet::RemoveEntities(const int *entity_handles,
                                 const int num_entities,
                                 int *info) 
{
#ifdef KARL_COMPILE
  std::vector<int> temp;
  set_difference(meshSetEntities.begin(), meshSetEntities.end(),
                 entity_handles, entity_handles+num_entities, 
                 inserter(temp, temp.begin()));
  meshSetEntities.swap(temp);
#endif
}

void MDB_MeshSet::RemoveEntities(const std::vector<MDBEntityHandle> &entity_handles,
                                 int *info) 
{
  for(int i=entity_handles.size(); i--;)
    meshSetEntities.remove(entity_handles[i]);
  //std::vector<int> temp;
  //std::set_difference(meshSetEntities.begin(), meshSetEntities.end(),
  //               entity_handles.begin(), entity_handles.end(), 
  //               std::inserter(temp, temp.begin()));
  //meshSetEntities.swap(temp);
}

    
int MDB_MeshSet::RemoveMeshSets(MDB_MeshSet **meshset_handles,
                                const int num_meshsets, int *) 
{ 
  std::vector<MDB_MeshSet*> temp;
  std::set_difference(meshset_handles, meshset_handles+num_meshsets, 
                 meshSetMeshSets.begin(), meshSetMeshSets.end(),
                 std::inserter(temp, temp.begin()));
  int num_removed = meshSetMeshSets.size() - temp.size();
  meshSetMeshSets.swap(temp);
  return num_removed;
}

  //! return the number of entities in the meshset, NOT including meshsets
const int MDB_MeshSet::NumEntities(int *) const
{ 
  return meshSetEntities.size();
}

  //! return the number of meshsets in the meshset
const int MDB_MeshSet::NumMeshSets(int *) const
{ 
  return meshSetMeshSets.size();
}

  //! return the number of entities in the meshset with the specified type and/or topology
  //! (NOT including meshsets)
const int MDB_MeshSet::NumEntitiesByType(const int entity_type,
                                         int *info) const
{ 
  std::vector<int> entity_list;
  GetEntitiesByType(entity_type, entity_list, info);
  return entity_list.size();
}

const int MDB_MeshSet::NumEntitiesByTopology(const int entity_topology,
                                             int *info) const
{ 
  std::vector<int> entity_list;
  GetEntitiesByTopology(entity_topology, entity_list, info);
  return entity_list.size();
}

  //! add a parent/child link between the meshsets; returns error if entities are already
  //! related or if child is already a parent of parent
void MDB_MeshSet::AddParentChild(MDB_MeshSet* parent_meshset, 
                                 MDB_MeshSet* child_meshset,
                                 int *info) 
{ 
  parent_meshset->AddChild(child_meshset);
  child_meshset->AddParent(parent_meshset);
}

  //! add multiple parents to multiple children (all parents get related to all children)
void MDB_MeshSet::AddParentsChildren(MDB_MeshSet* *parent_meshsets, 
                                     const int num_parents,
                                     MDB_MeshSet* *child_meshsets,
                                     const int num_children,
                                     int *info) 
{ 
  int i, j;
  for (i = 0; i < num_parents; i++) {
    for (j = 0; j < num_parents; j++) {
      AddParentChild(parent_meshsets[i], child_meshsets[j], info);
    }
  }
}

  //! add multiple parents to multiple children (all parents get related to all children)
void MDB_MeshSet::AddParentsChildren(std::vector<MDB_MeshSet*> parent_meshsets, 
                                     std::vector<MDB_MeshSet*> child_meshsets, 
                                     int *info) 
{ 
  std::vector<MDB_MeshSet*>::iterator 
    parent_it = parent_meshsets.begin(),
    child_it = child_meshsets.begin();
  
  for (; parent_it != parent_meshsets.end(); parent_it++)
    for (; child_it != child_meshsets.end(); child_it++)
      AddParentChild(*parent_it, *child_it, info);
}

  //! remove multiple parents from multiple children (all parents get removed from all children)
void MDB_MeshSet::RemoveParentsChildren(std::vector<MDB_MeshSet*> parent_meshsets, 
                                        std::vector<MDB_MeshSet*> child_meshsets, 
                                        int *info) 
{ 
  std::vector<MDB_MeshSet*>::iterator 
    parent_it = parent_meshsets.begin(),
    child_it = child_meshsets.begin();
  
  for (; parent_it != parent_meshsets.end(); parent_it++)
    for (; child_it != child_meshsets.end(); child_it++)
      RemoveParentChild(*parent_it, *child_it, info);
}

  //! remove a parent/child link between the meshsets; returns error if entities
  //! are not related
void MDB_MeshSet::RemoveParentChild(MDB_MeshSet* parent_meshset, 
                                    MDB_MeshSet* child_meshset,
                                    int *info) 
{ 
  parent_meshset->RemoveChild(child_meshset);
  child_meshset->RemoveParent(parent_meshset);
}

int MDB_MeshSet::AddParent(MDB_MeshSet *parent) 
{
  if (std::find(parentMeshSets.begin(),
                parentMeshSets.end(), parent) == parentMeshSets.end()) {
    parentMeshSets.push_back(parent);
    return 1;
  }
  else return 0;
}

int MDB_MeshSet::AddChild(MDB_MeshSet *child) 
{
  if (std::find(childMeshSets.begin(),
                childMeshSets.end(), child) == childMeshSets.end()) {
    childMeshSets.push_back(child);
    return 1;
  }
  else return 0;
}

int MDB_MeshSet::RemoveParent(MDB_MeshSet *parent) 
{
  std::vector<MDB_MeshSet*>::iterator position = 
    std::find(parentMeshSets.begin(), parentMeshSets.end(), parent);
  if ( position != parentMeshSets.end()) {
    parentMeshSets.erase(position);
    return 1;
  }
  else return 0;
}

int MDB_MeshSet::RemoveChild(MDB_MeshSet *child) 
{
  std::vector<MDB_MeshSet*>::iterator position = 
    std::find(childMeshSets.begin(), childMeshSets.end(), child);
  
  if ( position != childMeshSets.end()) {
    childMeshSets.erase(position);
    return 1;
  }
  else return 0;
}

  //! return the number of child/parent relations for this meshset
const int MDB_MeshSet::NumChildren(int *) const
{ 
  return childMeshSets.size();
}

const int MDB_MeshSet::NumParents(int *) const
{ 
  return parentMeshSets.size();
}

void MDB_MeshSet::SetName(const std::string &set_name, int *) 
{
  if (set_name == meshSetName) return;
  
  else if (meshSetName != "") {
      // name has been set - remove current name from map
    size_t num_removed = meshSetNames.erase(meshSetName);
    assert (1 == num_removed);
  }

    // put the new name on the list
  if (set_name != "") meshSetNames.insert(std::pair<const std::string,MDB_MeshSet*>(set_name, this));
  
    // now set the actual name
  meshSetName = set_name;
}

const int MDB_MeshSet::GetId(int *) const
{
  return meshSetId;
}

void MDB_MeshSet::SetId(const int set_id, int *) 
{
  if (set_id == meshSetId) return;
  
  if (meshSetId != -1) {
      // id has been set already - take out of the master list
    size_t num_removed = meshSetIds.erase(meshSetId);
    assert(1 == num_removed);
  }

    // put the new id on the list
  if (set_id != -1) meshSetIds.insert(std::pair<const int, MDB_MeshSet*>(set_id, this));
  
    // now set the actual id
  meshSetId = set_id;
}

