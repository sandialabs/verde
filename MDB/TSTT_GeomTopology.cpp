//
//-------------------------------------------------------------------------
// Filename      : TSTT_GeomTopology.cpp
// Creator       : Tim Tautges
//
// Date          : 03/01/02
//
// Owner         : Tim Tautges
//
// Description   : this class has various geometry-related algorithms that
//                 operate on meshsets
//-------------------------------------------------------------------------


#include "MDB_MeshSet.hpp"
#include "MDB_Mesh_QueryInterface.hpp"
#include "assert.h"
#include "utility"
#include "TSTT_GeomTopology.hpp"

/*

// snippet of code to insert geometric topology information in a mesh,
// and to restore that information from a mesh
//
// Assumes the following tags in the TSTT_MeshSet objects:
// Tag:Value: GeomDimension:[0, 1, 2, 3]
//
// Also assumes that the mesh has been created in the interface already
//

// Build MeshSets and store in interface from geometry information I already
// know about; assumes existence of some geometry classes/interfaces
void TSTT_GeomTopologyTool::Build_GeomTopology_MeshSets(const int max_dimension,
                                                        int *info) 
{
    // Use the following objects/functions for geometric topology
    // (change these to follow the OMG or TSTT geometry interface eventually)
    // GeomEntity--GeomVertex, GeomEdge, GeomFace, GeomRegion
    // void GeomEntity::Entity_GetAdjacencies(const GeomEntity_Handle entity_handle,
    //                                        const int adjacent_entity_dimension,
    //                                        GeomEntity_Handle *adj_entities,
    //                                        int *num_adj_entities,
    //                                        int *info);
    // const int GeomEntity::Entity_Dimension(int *info);
    // void GeomEntity::Entity_GetMeshEntities(const GeomEntity_Handle entity_handle,
    //                                         const int mesh_entity_dimension,
    //                                         TSTT_MeshEntity_Handle *mesh_entity_handles,
    //                                         int *num_mesh_entities,
    //                                         int *info);
    // void  GeomEntity::Geom_GetTag(const GeomEntity_Handle entity_handle, const char* name, 
    //                               int *tag_size, void **value, int *info)=0;
    //                               
    // void  GeomEntity::Geom_SetTag(const GeomEntity_Handle entity_handle, const char* name, 
    //                               const int tag_size, const void * const value, int *info)=0;
    //                               
    // void  GeomEntity::Geom_AddTag(const GeomEntity_Handle entity_handle, const char* name, 
    //                               int *info)=0;
    //                               
    // void  GeomEntity::Geom_DeleteTag(const GeomEntity_Handle entity_handle, const char* name, 
    //                                  int *info)=0;
    //                               


    // void GeomEntity::Entity_GetTag
    // const int Geom::Geom_NumEntities(const int dimension, int *info);
    // void Geom::Geom_GetEntities(const int dimension, 
    //                             GeomEntity_Handle *entities, int *num_entities,
    //                             int *info);
    // 

  int num_entities, num_parents, num_mesh_entities;
  GeomEntity_Handle *entity_handles, *parent_handles;
  TSTT_Entity_Handle *mesh_handles;
  TSTT_MeshSet_Handle this_meshset;
  
  for (int entity_type = 0; entity_type <= 3; entity_type++) {
    
    // build MeshSets for vertices
    Geom::Geom_GetEntities(entity_type, entity_handles, &num_entities, info);
    if (*info != 0) return;
  
    for (int this_id = 0; this_id < num_entities; this_id++) {

      TSTT_MeshSet_Handle *new_meshsets;
      int num_meshsets;
      GeomEntity::Geom_GetTag(entity_handles[this_id], "TSTT_MeshSet_Handle",
                              &num_meshsets, &new_meshsets);
      if (*info != 0) return;
      assert(0 == num_meshsets || 1 == num_meshsets);

        // if this entity already has a meshset, assume that meshset has been
        // set up properly (should we verify this, in case the mesh changed?)
      if (1 == num_meshsets) continue;

        // ok, no meshset yet; wait until after we get the mesh to create one, in case there's
        // no mesh on this one

      // get the mesh entities of the proper dimension on this entity
      GeomEntity::Entity_GetMeshEntities(entity_handles[this_id],
                                         entity_type,
                                         mesh_entity_handles, &num_mesh_entities,
                                         info);
        // check for error; eventually, need to clean up before exiting
      if (*info != 0) return;

        // if this geom entity isn't meshed, no need to create a meshset
      if (0 == num_mesh_entities) continue;

      TSTT_MeshSet_Create(&this_meshset, info);
      if (0 != *info) return;
      
        // add the mesh to this meshset
      TSTT_MeshSet_AddEntities(this_meshset, mesh_entity_handles, num_mesh_entities,
                               info);
      if (*info != 0) return;
      
        // put tags on this meshset to identify it as relating to geom. topology
      TSTT_MeshSet_AddTag(this_meshset, "GeomDimension", &entity_type,
                          info);
      if (*info != 0) return;
      
        // lastly, add relations to child MeshSets
      if (entity_type > 0) {
        TSTT_MeshSet_Handle child_sets[MAX_CHILDREN];
        GeomEntity::GeomEntity_Handle children[MAX_CHILDREN];
        int num_children;
          // get the child entities
        GeomEntity::Entity_GetAdjacencies(entity_handles[this_id],
                                          entity_type-1,
                                          children, &num_children
                                          info);
        if (0 != *info) return;
        
          // ok, have the children; go through them and get the meshsets
        for (int child_num = 0; child_num < num_children; child_num++) {
          TSTT_MeshSet_Handle *new_meshsets;
          int num_meshsets;
          GeomEntity::Geom_GetTag(children[child_num], "TSTT_MeshSet_Handle",
                                  &num_meshsets, &new_meshsets, info);
          if (0 != *info) return;
          assert(1 == num_meshsets);
          child_sets[child_num] = new_meshsets[0];
        }
        
          // add this parent to all the children and vica versa
        TSTT_MeshSet_AddParentsChildren(&this_meshset, 1,
                                        child_sets, num_children,
                                        info);
        if (0 != info) return;
      } // loop to assign parent-child relations
      
    } // loop over entities of dimension entity_type

  } // loop over entity_type
  
}

void GeomTopologyTool::Compute_Mesh_Skin(const TSTT::Mesh this_mesh, 
                                         TSTT_MeshSet_Handle *skin_faces_vec,
                                         int *num_skin_faces,
                                         int *info) 
{
    // function to compute the outer skin of the mesh; assumes MeshSets based on geometric
    // topology have been constructed
  
    // get all the meshsets
  TSTT_MeshSet_Handle *meshset_handles = NULL;
  int num_meshsets;
  TSTT_MeshSet_GetMeshSets(meshset_handles, num_meshsets, &info);
  if (0 != *info) return;

    // build a list of gfaces
  vector<TSTT_MeshSet_Handle> gfaces;
  int **mset_tags, tag_size;
  for (int mset = 0; mset < num_meshsets; mset++) {
    TSTT_MeshSet_GetTag(meshset_handles[mset], "GeomDimension",
                        &tag_size, mset_tags, info);
    assert(1 == tag_size || 0 == tag_size);
      // check for existence of the tag and the value/dimension
    if (1 == tag_size && 2 == **mset_tags)
        // this is a gface; add it to our list
      gfaces.push_back(meshset_handles[mset]);
  }
  
    // ok, we have all the faces; check their containing regions to find the skin
  vector<TSTT_MeshSet_Handle>::iterator i;
  vector<TSTT_MeshSet_Handle> skin_faces;
  
  for (i = gfaces.begin(); i != gfaces.end(); i++) {
      // get the number of parents associated with this gface; if it's 1, we have
      // a skin face
    int num_parents = TSTT_MeshSet_NumParents(*i, info);

      // check for error;
    if (0 != *info ||
          // also, check for unusual numbers of parents; that's not an outright
          // error, but it's something we don't handle
        0 > num_parents || 2 < num_parents) continue;
    if (1 == num_parents) skin_faces.push_back(*i);
  }

    // ok, we have the skin faces; put them on the vector passed in
  if (NULL == skin_faces_vec ||
      (NULL != skin_faces_vec && 0 != *num_skin_faces)) {
    delete [] skin_faces_vec;
    skin_faces_vec = new TSTT_MeshSet_Handle[skin_faces.size()];
    *num_skin_faces = skin_faces.size();
  }
  
  vector<TSTT_MeshSet_Handle>::iterator i;
  for (i = skin_faces.begin(), int face_num = 0; i != skin_faces.end(); i++, face_num++) 
    skin_faces_vec[face_num] = *i;

    // if we got here, we're successful
  *info = 0;
}

*/

int TSTT_GeomTopology::imprint_sets(TSTT_MeshSet_Handle *imprint_sets,
                                    const int num_imprint_sets,
                                    int *info) 
{
    // general function to (intersect and) imprint sets together.
    // ASSUMPTION: if the sets imprint and/or intersect, when elements are
    // overlapping in space, they also match topologically; so, this function
    // is meant to *recover* imprints that were there before, and which were
    // used to generate the mesh

    // put the input sets into a list, so that we can add to it later
  std::vector<TSTT_MeshSet_Handle> set_list;
  std::copy(imprint_sets, imprint_sets+num_imprint_sets,
            std::back_inserter(set_list));
  
    // now imprint each set pair together; just do pairwise imprinting for
    // sets in the initial list (might need to do more later...)
  int i, j, temp_info;
  for (i = 0; i < num_imprint_sets; i++) {
    for (j = 0; j < num_imprint_sets; j++) {
      TSTT_GeomTopology::imprint_sets(set_list[i], set_list[j], &temp_info);
    }
  }

  *info = temp_info;
  return temp_info;
}

int TSTT_GeomTopology::imprint_sets(std::vector<TSTT_MeshSet_Handle> &imprint_sets1,
                                    std::vector<TSTT_MeshSet_Handle> &imprint_sets2,
                                    int *info) 
{
    // general function to (intersect and) imprint sets together.
    // ASSUMPTION: if the sets imprint and/or intersect, when elements are
    // overlapping in space, they also match topologically; so, this function
    // is meant to *recover* imprints that were there before, and which were
    // used to generate the mesh

  std::vector<TSTT_MeshSet_Handle>::iterator 
    set1_it = imprint_sets1.begin(), 
    set2_it = imprint_sets2.begin();
  
    // now imprint each set pair together; just do pairwise imprinting for
    // sets in the initial list (might need to do more later...)
  int temp_info;
  for (; set1_it != imprint_sets1.end(); set1_it++) {
    for (; set2_it != imprint_sets2.end(); set2_it++) {
      TSTT_GeomTopology::imprint_sets(*set1_it, *set2_it, &temp_info);
    }
  }

  *info = temp_info;
  return temp_info;
}

int TSTT_GeomTopology::imprint_sets(TSTT_MeshSet_Handle set1,
                                    TSTT_MeshSet_Handle set2,
                                    int *info) 
{
    // given two mesh sets, intersect & imprint them together, including their
    // boundaries, so that they're conformal.  This may reqiure construction of
    // new sets, or at least new bdy sets

  int temp_info;
  
    // step A: intersect set interiors
  std::vector<TSTT_Entity_Handle> intersection_elems;
  int proper1, proper2;
  MDB_MeshSet *intersection_set = new MDB_MeshSet();
  intersection_set->AddEntities(set2->MeshSetEntities(), &temp_info);
  MDB_MeshSet::Intersect(intersection_set, set1,
                         proper1, proper2, &temp_info);

    // only make a new set if its interior is non-null
  if (0 != intersection_set->NumEntities(&temp_info)) {
    if (1 == proper1) intersection_set = set1;
    else if (1 == proper2) intersection_set = set2;
    else {
      intersection_set = new MDB_MeshSet(intersection_elems);
        // make sure and skin 
        //intersection_set->ConstructSkin(&temp_info);
    }
  }
  else delete intersection_set;
  
    // step B: modify interiors of other sets
  if (NULL != intersection_set) {
      // non-zero intersection of interior; modify set1 and set2 by:
      // a) removing int(C) from A and B
    if (intersection_set != set1)
      set1->RemoveEntities(intersection_set->MeshSetEntities(), &temp_info);
    if (intersection_set != set2)
      set2->RemoveEntities(intersection_set->MeshSetEntities(), &temp_info);
    
      // b) adding parents(A,B) to C
    std::vector<TSTT_MeshSet_Handle> parent_sets, dummy_list;
    dummy_list.push_back(intersection_set);
    if (intersection_set != set1)
      set1->GetParents(1, parent_sets, &temp_info);
    if (intersection_set != set2)
      set2->GetParents(1, parent_sets, &temp_info);
    MDB_MeshSet::AddParentsChildren(parent_sets, dummy_list, &temp_info);
  }
  
    // step C: imprint & combine lower-order entities (i.e. call this function
    // recursively)
  std::vector<TSTT_MeshSet_Handle> children1, children2;
  if (NULL == intersection_set) {
      // if the intersection set is null, just check for intersections on
      // the original set boundaries (but only if there are really children)
    if (0 != set1->NumChildren(&temp_info) && 0 != set2->NumChildren(&temp_info)) {
      children1 = set1->ChildMeshSets();
      children2 = set2->ChildMeshSets();
      TSTT_GeomTopology::imprint_sets(children1, children2, &temp_info);
    }
  }
  else {
      // else imprint the intersection set with set1 and set2; but, only
      // if the intersection isn't equal to either
    if (set1 != intersection_set && 
        0 != set1->NumChildren(&temp_info) && 0 != intersection_set->NumChildren(&temp_info)) {
      children1 = set1->ChildMeshSets();
      children2 = intersection_set->ChildMeshSets();
      TSTT_GeomTopology::imprint_sets(children1, children2, &temp_info);
    }
    if (set2 != intersection_set && 
        0 != set2->NumChildren(&temp_info) && 0 != intersection_set->NumChildren(&temp_info)) {
      children1 = set2->ChildMeshSets();
        // need to re-assign children2 because intersection set might have gotten
        // new children (from split)
      children2 = intersection_set->ChildMeshSets();
      TSTT_GeomTopology::imprint_sets(children1, children2, &temp_info);
    }
  }
    
    // step D: XOR boundary with bdy of intersection set
  XorBoundary(set1, intersection_set);
  XorBoundary(set2, intersection_set);

  return temp_info;
}

void TSTT_GeomTopology::XorBoundary(TSTT_MeshSet_Handle set1,
                                    TSTT_MeshSet_Handle intersection_set) 
{
#ifdef KARL_COMPILE
  if (NULL == intersection_set ||
      set1 == intersection_set) return;
  
  std::vector<TSTT_MeshSet_Handle> dummy_list,
    children1 = set1->ChildMeshSets(),
    children2 = intersection_set->ChildMeshSets();

    // find intersection, complement
  set_intersection(children1.begin(), children1.end(), 
                   children2.begin(), children2.end(), 
                   back_inserter(dummy_list));
  children1.swap(dummy_list);
  dummy_list.erase(dummy_list.begin(), dummy_list.end());
  set_difference(children2.begin(), children2.end(), 
                 children1.begin(), children1.end(), 
                 back_inserter(dummy_list));
  children2.swap(dummy_list);

    // children1 holds intersection, children2 the complement of that
  dummy_list.erase(dummy_list.begin(), dummy_list.end());
  dummy_list.push_back(set1);
  int temp_info;
  if (!children1.empty()) 
    MDB_MeshSet::RemoveParentsChildren(dummy_list, children1, &temp_info);
  if (!children2.empty()) 
    MDB_MeshSet::AddParentsChildren(dummy_list, children2, &temp_info);
#endif
}

