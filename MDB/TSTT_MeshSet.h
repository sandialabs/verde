//
//-------------------------------------------------------------------------
// Filename      : TSTT_MeshSet.h
// Creator       : Tim Tautges
//
// Date          : 02/01/02
//
// Owner         : Tim Tautges
//
// Description   : TSTT_MeshSet is the interface to an arbitrary or non-arbitrary
//                 set of mesh entities
//-------------------------------------------------------------------------

  //! get the value of a tag with the specified name on this meshset;
  //! return *info != 0 if the tag doesn't exist on this meshset
virtual void TSTT_MeshSet_GetTag(MeshSet_CHandle meshset_handle, const char* name, 
                                 int *tag_size, 
                                 unsigned char **value, int *info) const =0;

  //! add a tag with the specified name on this meshset;
  //! return *info != 0 if the addition isn't successful
virtual void TSTT_MeshSet_AddTag(MeshSet_Handle meshset_handle, const char* name, 
                                 const int tag_size, 
                                 const void * const value, int *info)=0;

  //! delete a tag with the specified name from this meshset;
  //! return *info != 0 if the addition isn't successful
virtual void TSTT_MeshSet_DeleteTag(MeshSet_Handle meshset_handle, const char* name, 
                                    int *info)=0;

  //! return handles to all the meshsets which have been defined
  //! (should this really be here, or should it be a function/data on
  //! the TSTT_Mesh instance?)
virtual void TSTT_MeshSet_GetMeshSets(MeshSet_Handle **meshset_handles, int *num_entities,
                                      int *info) const =0;

  //! return the meshsets contained in this meshset
virtual void TSTT_MeshSet_GetMeshSets(MeshSet_Handle meshset_handle, 
                                      MeshSet_Handle **set_handles, 
                                      int *num_sets, int *info) const =0;

  //! like GetMeshSets, but recurses down meshsets
virtual void TSTT_MeshSet_GetAllMeshSets(MeshSet_Handle meshset_handle, 
                                         MeshSet_Handle **set_handles, 
                                         int *num_sets, int *info) const =0;

  //! return handles to all the entities contained in this meshset
virtual void TSTT_MeshSet_GetEntities(MeshSet_Handle meshset_handle, 
                                      TSTT_Entity_Handle **entity_handles, 
                                      int *num_entities, int *info) const =0;

  //! like GetEntities, but recurses down contained MeshSets
virtual void TSTT_MeshSet_GetAllEntities(MeshSet_Handle meshset_handle, int *num_entities,
                                         TSTT_Entity_Handle **entity_handles, int *info) const =0;

  //! return the entities with the specified type in this meshset
virtual void TSTT_MeshSet_GetEntitiesByType(MeshSet_Handle meshset_handle, 
                                            const int entity_type,
                                            TSTT_Entity_Handle **entity_handles, 
                                            int *num_entities, int *info) const =0;
                            
  //! return the entities with the specified topology in this meshset
virtual void TSTT_MeshSet_GetEntitiesByTopology(MeshSet_Handle meshset_handle, 
                                                const int entity_topology,
                                                TSTT_Entity_Handle **entity_handles, 
                                                int *num_entities, int *info) const =0;
                            
  //! subtract/intersect/unite meshset_2 from/with/into meshset_1; modifies meshset_1
virtual void TSTT_MeshSet_Subtract(MeshSet_Handle meshset_1, 
                                   MeshSet_Handle meshset_2, int *info)=0;
virtual void TSTT_MeshSet_Intersect(MeshSet_Handle meshset_1, 
                                    MeshSet_Handle meshset_2, int *info)=0;
virtual void TSTT_MeshSet_Unite(MeshSet_Handle meshset_1, 
                                MeshSet_Handle meshset_2, int *info)=0;

  //! get relations between sets; assumes hierarchical (parent/child) relationships; if
  //! num_hops = -1, gets *all* parents or children over all hops
virtual void TSTT_MeshSet_GetChildren(MeshSet_Handle from_meshset, const int num_hops, 
                                      int *num_sets, MeshSet_Handle **set_handles,
                                      int *info) const =0;
virtual void TSTT_MeshSet_GetParents(MeshSet_Handle from_meshset, const int num_hops, 
                                     int *num_sets, MeshSet_Handle **set_handles, 
                                     int *info) const =0;

  //! returns non-zero if meshsets *are* related topologically
virtual int IsRelated(MeshSet_Handle meshset_1, 
                      MeshSet_Handle meshset_2, int *info) const =0;

  //! create an empty meshset
virtual void TSTT_MeshSet_Create(MeshSet_Handle *meshset_created, int *info)=0;

  //! add num_entities entities to a meshset
virtual void TSTT_MeshSet_AddEntities(MeshSet_Handle meshset, 
                                      const TSTT_Entity_Handle * const entity_handles,
                                      const int num_entities, int *info)=0;

  //! add num_meshsets meshsets to a meshset
virtual void TSTT_MeshSet_AddMeshSets(MeshSet_Handle meshset, 
                                      MeshSet_CHandle * const meshset_handles,
                                      const int num_meshsets, int *info)=0;

  //! remove num_entities entities from the MeshSet; returns non-zero if
  //! some entities were not in the meshset to begin with
virtual void TSTT_MeshSet_RemoveEntities(MeshSet_Handle meshset, 
                                         const TSTT_Entity_Handle *const entity_handles,
                                         const int num_entities,
                                         int *info)=0;

  //! remove num_meshsets meshsets from the meshset
virtual void TSTT_MeshSet_RemoveMeshSets(MeshSet_Handle meshset, 
                                         MeshSet_CHandle * const meshset_handles,
                                         const int num_meshsets, int *info)=0;

  //! return the number of entities in the meshset, NOT including meshsets
virtual const int TSTT_MeshSet_NumEntities(MeshSet_CHandle meshset, int *info) const =0;

  //! return the number of meshsets in the meshset
virtual const int TSTT_MeshSet_NumMeshSets(MeshSet_CHandle meshset, int *info) const =0;

  //! return the number of entities in the meshset with the specified type and/or topology
  //! (NOT including meshsets)
virtual const int TSTT_MeshSet_NumEntitiesByType(MeshSet_CHandle meshset, 
                                                 const int entity_type,
                                                 int *info) const =0;
virtual const int TSTT_MeshSet_NumEntitiesByTopology(MeshSet_CHandle meshset, 
                                                     const int entity_topology,
                                                     int *info) const =0;

  //! add a parent/child link between the meshsets; returns error if entities are already
  //! related or if child is already a parent of parent
virtual void TSTT_MeshSet_AddParentChild(MeshSet_Handle parent_meshset, 
                                         MeshSet_Handle child_meshset,
                                         int *info)=0;

  //! add multiple parents to multiple children (all parents get related to all children)
virtual void TSTT_MeshSet_AddParentsChildren(MeshSet_Handle *parent_meshsets, 
                                             const int num_parents,
                                             MeshSet_Handle *child_meshsets,
                                             const int num_children,
                                             int *info)=0;

  //! remove a parent/child link between the meshsets; returns error if entities
  //! are not related
virtual void TSTT_MeshSet_RemoveParentChild(MeshSet_Handle parent_meshset, 
                                            MeshSet_Handle child_meshset,
                                            int *info)=0;

  //! return the number of child/parent relations for this meshset
virtual const int NumChildren(MeshSet_CHandle meshset, 
                              int *info) const =0;
virtual const int NumParents(MeshSet_CHandle meshset, 
                             int *info) const =0;
