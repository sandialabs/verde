/** @defgroup Functions for adding, deleting, setting and getting tags

/** @defgroup Mesh_AddTag_ functions
 */

// Allocate a tag of the specified size for these_entities (could
// just be a single entity too);
// Input:
// TSTT_Entity_Handle *these_entities: entities to which this tag will be
//     associated
// const Int num_entities: number of these_entities
// const char *tag_name: the application-assigned name for this tag
// const Int tag_size: the size of data to be allocated for this tag
// const void *default_value: the default value used to initialize
//     this tag
//
// Output:
// TSTT_Tag_Handle *tag_handle: handle by which tag can be referred to;
//   if void, tag_handle is not returned
// MeshError *errorReturn: the return value for the function
//
void Mesh_AddTag_Entities(Mesh *mesh,
                          TSTT_Entity_Handle *these_entities, 
                          const Int num_entities,
                          const char* tag_name, const Int tag_size,
                          const void *default_value,
                          Tag_Handle *tag_handle,
                          MeshError *errorReturn);

// Allocate a tag of the specified size to all entities of the
// specified type.  
// Input:
// (most args similar to Mesh_AddTag_Entities, only new ones listed here)
// const Int individual_values: if = 1, each entity of the specified type
//   is allowed to have a unique value of this tag; if = 0, one value of
//   this tag is allowed for the specified type
// const Int future_too: if = 1 and individual_values = 1, all entities 
//   of the specified type created subsequently are given this tag, 
//   otherwise only extant entities of the specified type are given this tag.
//   This argument is ignored if individual_values = 0
void Mesh_AddTag_EntityType(Mesh *mesh,
                            const EntityType entity_type, 
                            const char* name, const Int tag_size,
                            const void *default_value,
                            const Int individual_values,
                            const Int future_too,
                            Tag_Handle *tag_handle,
                            MeshError *errorReturn);
    
// Allocate a tag of the specified size to all entities of the
// specified topology (a single tag value per entity topology is allowed)
// (args similar to Mesh_AddTag_EntityType)
void Mesh_AddTag_EntityTopology(Mesh *mesh,
                                const Int entity_topology, 
                                const char* name, const Int tag_size,
                                const void *default_value,
                                const Int individual_values,
                                const Int future_too,
                                Tag_Handle *tag_handle,
                                MeshError *errorReturn);

// Allocate a tag to the specified MeshSet (args similar to
// Mesh_AddTag_Entities) 
void Mesh_AddTag_MeshSet(Mesh *mesh,
                         TSTT_MeshSet_Handle this_meshset, 
                         const char* name, const Int tag_size,
                         const void *default_value,
                         Tag_Handle *tag_handle,
                         MeshError *errorReturn);
    
// Allocate a tag to the specified mesh interface instance (args
// similar to Mesh_AddTag_Entities) 
void Mesh_AddTag_Mesh(Mesh *mesh, 
                      const char* name, const Int tag_size,
                      const void *default_value,
                      Tag_Handle *tag_handle,
                      MeshError *errorReturn);
    
/** @defgroup Mesh_DeleteTag_ functions
 */

// De-allocate the tag named tag_name from these_entities
//
// Input:
// TSTT_Entity_Handle *these_entities: the entities from which this tag
//   will be de-allocated 
// const Int num_entities: number of entities in that list
// const char *tag_name: the tag name which will be de-allocated
//
// Output:
// MeshError *errorReturn: the return flag for this function
void Mesh_DeleteTag_Entities(Mesh *mesh,
                             TSTT_Entity_Handle *these_entities, 
                             const Int num_entities,
                             cTag_Handle tag_handle, 
                             MeshError *errorReturn);

// De-allocate the tag named tag_name from entities of the specified
// type 
// Input:
// (most arguments similar to Mesh_DeleteTag_Entities, those aren't repeated
//  here)
// const Int just_future: if = 1, the tag is not de-allocated from 
//   current entities, it just won't be allocated for future entities
//   (i.e. this reverses the effect of the "future_too" argument in
//   Mesh_AddTag_EntityType)
void Mesh_DeleteTag_EntityType(Mesh *mesh,
                               const EntityType entity_type,
                               cTag_Handle tag_handle, 
                               const Int just_future,
                               MeshError *errorReturn);

// De-allocate the tag named tag_name from entities of the specified
// topology (arguments similar to Mesh_DeleteTag_Entities)
void Mesh_DeleteTag_EntityTopology(Mesh *mesh,
                                   const EntityType entity_type,
                                   cTag_Handle tag_handle, 
                                   const Int just_future,
                                   MeshError *errorReturn);

// De-allocate the tag named tag_name from entities of this_meshset 
// (arguments similar to Mesh_DeleteTag_Entities)
void Mesh_DeleteTag_MeshSet(Mesh *mesh,
                            TSTT_MeshSet_Handle this_meshset,
                            cTag_Handle tag_handle, MeshError *errorReturn);

// De-allocate the named tag from the specified mesh interface
// instance (args similar to Mesh_DeleteTag_Entities) 
void Mesh_DeleteTag_Mesh(Mesh *mesh, 
                         cTag_Handle tag_handle, 
                         MeshError *errorReturn);
    
/** @defgroup Mesh_GetTag_ functions
 */

// Get the value for a tag of the specified size for this_entity;
// Input:
// TSTT_Entity_Handle this_entity: entity for which you want to 
//     get the tag
// const char *tag_name: the name of the tag to get
// const void **tag_value: the tag value
// Int *tag_size: the size of the tag returned
//
// Output:
// MeshError *errorReturn: the return value for the function
//
void Mesh_GetTag_Entity(const Mesh *mesh,
                        TSTT_Entity_CHandle this_entity, 
                        const char* tag_name, 
                        void **tag_value, Int *tag_size,
                        MeshError *errorReturn);

// Get the value for a tag of the specified name from all entities in
// these_entities (args similar to Mesh_GetTag_Entities)
void Mesh_GetTag_Entities(const Mesh *mesh,
                          TSTT_Entity_CHandle *these_entities, 
                          const Int num_entities,
                          const char* tag_name, 
                          void **tag_value, Int *tag_size,
                          MeshError *errorReturn);

// Get a tag of the specified type from all entities of the
// specified type (args similar to Mesh_GetTag_Entities)
void Mesh_GetTag_EntityType(const Mesh *mesh,
                            const EntityType entity_type, 
                            cTag_Handle tag_handle, 
                            void **tag_value, Int *tag_size,
                            MeshError *errorReturn);
    
// Get a tag of the specified type from all entities of the
// specified topology (args similar to Mesh_GetTag_Entities)
void Mesh_GetTag_EntityTopology(const Mesh *mesh,
                                const Int entity_topology, 
                                cTag_Handle tag_handle, 
                                void **tag_value, Int *tag_size,
                                MeshError *errorReturn);

// Get a tag from the specified MeshSet (args similar to
// Mesh_GetTag_Entities) 
void Mesh_GetTag_MeshSet(const Mesh *mesh,
                         TSTT_MeshSet_CHandle this_meshset, 
                         cTag_Handle tag_handle, 
                         void **tag_value, Int *tag_size,
                         MeshError *errorReturn);
    
// Get a tag from the specified mesh interface instance (args
// similar to Mesh_GetTag_Entities) 
void Mesh_GetTag_Mesh(const Mesh *mesh, 
                      cTag_Handle tag_handle, 
                      void **tag_value, Int *tag_size,
                      MeshError *errorReturn);
    
/** @defgroup Mesh_SetTag_ functions
 */

// Set the value for a tag of the specified size for these_entities;
// Input:
// TSTT_Entity_Handle *these_entities: entities for which you want to 
//     set the tags
// const Int num_entities: number of entities in these_entities
// const char *tag_name: the name of the tag to get
// const void *tag_values: the tag values
// Int tag_size: the size of the tags being set
//
// Output:
// MeshError *errorReturn: the return value for the function
//
void Mesh_SetTag_Entities(Mesh *mesh,
                          TSTT_Entity_Handle *these_entities, 
                          const Int num_entities,
                          const char* tag_name, 
                          const void *tag_values, const Int tag_size,
                          MeshError *errorReturn);

// Set a tag of the specified type on all entities of the
// specified type (args similar to Mesh_SetTag_Entities); if this
// tag was created to have individual values per entity, this tag
// value is put on all entities of the specified type.
void Mesh_SetTag_EntityType(Mesh *mesh,
                            const EntityType entity_type, 
                            cTag_Handle tag_handle, 
                            const void *tag_value, const Int tag_size,
                            MeshError *errorReturn);
    
// Set a tag of the specified type from all entities of the
// specified topology (args similar to Mesh_SetTag_Entities); if this
// tag was created to have individual values per entity, this tag
// value is put on all entities of the specified topology.
void Mesh_SetTag_EntityTopology(Mesh *mesh,
                                const Int entity_topology, 
                                cTag_Handle tag_handle, 
                                const void *tag_value, const Int tag_size,
                                MeshError *errorReturn);

// Set a tag from the specified MeshSet (args similar to
// Mesh_SetTag_Entities) 
void Mesh_SetTag_MeshSet(Mesh *mesh,
                         TSTT_MeshSet_Handle this_meshset, 
                         cTag_Handle tag_handle, 
                         const void *tag_value, const Int tag_size,
                         MeshError *errorReturn);
    
// Set a tag from the specified mesh interface instance (args
// similar to Mesh_SetTag_Entities) 
void Mesh_SetTag_Mesh(Mesh *mesh, 
                      cTag_Handle tag_handle, 
                      const void *tag_value, const Int tag_size,
                      MeshError *errorReturn);
    
/** @defgroup Miscellaneous functions for getting general tag information
 */

// Get the size of a tag of the specified name
// Input:
// const char *tag_name: the name of the tag to get
//
// Output:
// Int *tag_size: the size of the tag
// MeshError *errorReturn: the return value for the function
//
void Mesh_Tag_GetSize(Mesh *mesh,
                      cTag_Handle tag_handle, 
                      Int *tag_size,
                      MeshError *errorReturn);

// Get the type of a tag of the specified name (mesh, meshset,
// etc.)
//
// Input:
// const char *tag_name: the name of the tag to get
//
// Output:
// Int *tag_type: the type of the tag (0=Entity, 1=EntityType,
//    2=EntityTopology, 3=MeshSet)
// MeshError *errorReturn: the return value for the function
//
void Mesh_Tag_GetInfo(Mesh *mesh,
                      cTag_Handle tag_handle,
                      char* tag_name, 
                      Int *tag_type,
                      Int *individual,
                      Int *future,
                      MeshError *errorReturn);

// Get the tag with the specified name
//
// Input:
// const char *tag_name: the name of the tag for which you want the handle
//
// Output:
// Tag_Handle *tag_handle: handle for the specified tag
// MeshError *errorReturn: the return value for the function
//
void Mesh_Tag_GetHandle(Mesh *mesh,
                        const char *tag_name,
                        Tag_Handle *tag_handle,
                        MeshError *errorReturn);

// Get the tags which exist in the mesh instance for the specified type
//
// Input:
// const Int tag_type: the tag type (0=Entity, etc.)
//
// Output:
// Tag_Handle *tag_handles: the handles of existing tags
// Int *num_tags: the number of tags in tag_handles
// MeshError *errorReturn: the return value for the function
//
void Mesh_Tag_GetTags(Mesh *mesh,
                      const Int tag_type,
                      Tag_Handle *tag_handles,
                      Int *num_tags,
                      MeshError *errorReturn);

