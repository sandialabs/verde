//!
//! language-independent declarations (and definitions) for MDB_MeshSet
//!

  //! return handles to all the meshsets which have been defined
  //! (should this really be here, or should it be a function/data on
  //! the TSTT_Mesh instance?)
  //! also, this should return a language-indep. iterator, not an array
virtual void TSTT_MeshSet_GetMeshSets(TSTT_MeshSet_Handle **meshset_handles, int *num_entities,
                                      int *info) const;

  //! return the meshsets contained in this meshset
virtual void TSTT_MeshSet_GetMeshSets(TSTT_MeshSet_Handle meshset_handle, 
                                      TSTT_MeshSet_Handle **set_handles, 
                                      int *num_sets, int *info) const;

  //! like GetMeshSets, but recurses down meshsets
virtual void TSTT_MeshSet_GetAllMeshSets(TSTT_MeshSet_Handle meshset_handle, 
                                         TSTT_MeshSet_Handle **set_handles, 
                                         int *num_sets, int *info) const;

  //! return handles to all the entities contained in this meshset
virtual void TSTT_MeshSet_GetEntities(TSTT_MeshSet_Handle meshset_handle, 
                                      TSTT_Entity_Handle **entity_handles, 
                                      int *num_entities, int *info) const;

  //! like GetEntities, but recurses down contained MeshSets
virtual void TSTT_MeshSet_GetAllEntities(TSTT_MeshSet_Handle meshset_handle, int *num_entities,
                                         TSTT_Entity_Handle **entity_handles, int *info) const;

  //! return the entities with the specified type in this meshset
virtual void TSTT_MeshSet_GetEntitiesByType(TSTT_MeshSet_Handle meshset_handle, 
                                            const int entity_type,
                                            TSTT_Entity_Handle **entity_handles, 
                                            int *num_entities, int *info) const;

  //! return the entities with the specified topology in this meshset
virtual void TSTT_MeshSet_GetEntitiesByTopology(TSTT_MeshSet_Handle meshset_handle, 
                                                const int entity_topology,
                                                TSTT_Entity_Handle **entity_handles, 
                                                int *num_entities, int *info) const;

  //! subtract/intersect/unite meshset_2 from/with/into meshset_1; modifies meshset_1
virtual void TSTT_MeshSet_Subtract(TSTT_MeshSet_Handle meshset_1, 
                                   TSTT_MeshSet_Handle meshset_2, int *info);

virtual void TSTT_MeshSet_Intersect(TSTT_MeshSet_Handle meshset_1, 
                                    TSTT_MeshSet_Handle meshset_2, int *info);

virtual void TSTT_MeshSet_Unite(TSTT_MeshSet_Handle meshset_1, 
                                TSTT_MeshSet_Handle meshset_2, int *info);

  //! get relations between sets; assumes hierarchical (parent/child) relationships; if
  //! num_hops = -1, gets *all* parents or children over all hops
virtual void TSTT_MeshSet_GetChildren(TSTT_MeshSet_Handle from_meshset, const int num_hops,
                                      int *num_sets, TSTT_MeshSet_Handle **set_handles, int *info) const;

virtual void TSTT_MeshSet_GetParents(TSTT_MeshSet_Handle from_meshset, const int num_hops,
                                     int *num_sets,
                                     TSTT_MeshSet_Handle **set_handles, int *info) const;

  //! returns non-zero if meshsets *are* related topologically
virtual int TSTT_MeshSet_IsRelated(TSTT_MeshSet_Handle meshset_1, 
                                   TSTT_MeshSet_Handle meshset_2, int *info) const;

  //! create an empty meshset
virtual void TSTT_MeshSet_Create(TSTT_MeshSet_Handle *meshset_created, int *info);

  //! add num_entities entities to a meshset
virtual void TSTT_MeshSet_AddEntities(TSTT_MeshSet_Handle meshset, 
                                      const TSTT_Entity_Handle * const entity_handles,
                                      const int num_entities, int *info);

  //! add num_meshsets meshsets to a meshset
virtual void TSTT_MeshSet_AddMeshSets(TSTT_MeshSet_Handle meshset, 
                                      TSTT_MeshSet_Handle *meshset_handles,
                                      const int num_meshsets, int *info);

  //! remove num_entities entities from the MeshSet; returns non-zero if
  //! some entities were not in the meshset to begin with
virtual void TSTT_MeshSet_RemoveEntities(TSTT_MeshSet_Handle meshset, 
                                         const TSTT_Entity_Handle *const entity_handles,
                                         const int num_entities,
                                         int *info);

  //! remove num_meshsets meshsets from the meshset
virtual void TSTT_MeshSet_RemoveMeshSets(TSTT_MeshSet_Handle meshset, 
                                         TSTT_MeshSet_Handle *meshset_handles,
                                         const int num_meshsets, int *info);

  //! return the number of entities in the meshset, NOT including meshsets
virtual const int TSTT_MeshSet_NumEntities(TSTT_cMeshSet_Handle meshset, int *info) const;

  //! return the number of meshsets in the meshset
virtual const int TSTT_MeshSet_NumMeshSets(TSTT_cMeshSet_Handle meshset, int *info) const;

  //! return the number of entities in the meshset with the specified type and/or topology
  //! (NOT including meshsets)
virtual const int TSTT_MeshSet_NumEntitiesByType(TSTT_cMeshSet_Handle meshset, 
                                                 const int entity_type,
                                                 int *info) const;

virtual const int TSTT_MeshSet_NumEntitiesByTopology(TSTT_cMeshSet_Handle meshset, 
                                                     const int entity_topology,
                                                     int *info) const;

  //! add a parent/child link between the meshsets; returns error if entities are already
  //! related or if child is already a parent of parent
virtual void TSTT_MeshSet_AddParentChild(TSTT_MeshSet_Handle parent_meshset, 
                                         TSTT_MeshSet_Handle child_meshset,
                                         int *info);

  //! add multiple parents to multiple children (all parents get related to all children)
virtual void TSTT_MeshSet_AddParentsChildren(TSTT_MeshSet_Handle *parent_meshsets, 
                                             const int num_parents,
                                             TSTT_MeshSet_Handle *child_meshsets,
                                             const int num_children,
                                             int *info);

  //! remove a parent/child link between the meshsets; returns error if entities
  //! are not related
virtual void TSTT_MeshSet_RemoveParentChild(TSTT_MeshSet_Handle parent_meshset, 
                                            TSTT_MeshSet_Handle child_meshset,
                                            int *info);

  //! return the number of child/parent relations for this meshset
virtual const int TSTT_MeshSet_NumChildren(TSTT_cMeshSet_Handle meshset, 
                                           int *info) const;

virtual const int TSTT_MeshSet_NumParents(TSTT_cMeshSet_Handle meshset, 
                                          int *info) const;

  //! get/set the name of this meshset
virtual void TSTT_MeshSet_GetName(TSTT_cMeshSet_Handle meshset,
                                  char **name_string, int *name_length,
                                  int *info) const;

virtual void TSTT_MeshSet_SetName(TSTT_MeshSet_Handle meshset,
                                  const char *name_string, const int name_length,
                                  int *info);

  //! get/set the id of this meshset
virtual void TSTT_MeshSet_GetId(TSTT_cMeshSet_Handle meshset,
                                int *id, int *info) const;

virtual void TSTT_MeshSet_SetId(TSTT_MeshSet_Handle meshset,
                                const int id, int *info);

virtual const int TSTT_MeshSet_GetMark(TSTT_cMeshSet_Handle meshset, int *info) const;

virtual void TSTT_MeshSet_SetMark(TSTT_MeshSet_Handle meshset, const int flag, int *info);


  
