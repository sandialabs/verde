//
//-------------------------------------------------------------------------
// Filename      : MDB_RMeshSet.hpp 
// Creator       : Tim Tautges
//
// Date          : 03/02
//
// Owner         : Tim Tautges
//
// Description   : MDB_RMeshSet is the class used to store node and element
//               : *representation*; node and element grouping are stored in
//               : regular meshsets (i.e. MDB_MeshSet)
//-------------------------------------------------------------------------

#ifndef MDB_RMESHSET_HPP
#define MDB_RMESHSET_HPP

#include <map>
#include <set>
#include <vector>
#include "TSTT_Mesh_QueryInterface.h"
#include "MDB_MeshSet.hpp"
#include "SparseTagServer.hpp"

//namespace TSTT 
//{

class MDB_RMeshSet : public MDB_MeshSet
{
public:

    //! create a node-type meshset
  MDB_RMeshSet(int num_nodes,
               double *node_x, double *node_y, double *node_z,
               const int node_id_offset,
               const int prev_node_offset);
  
  //! element form of constructor
  MDB_RMeshSet(int num_elems,
               int *connect_array,
               const int elem_id_offset,
               EntityType entity_type,
               EntityTopology entity_topology);

    //! virtual destructor
  virtual ~MDB_RMeshSet();

    //! get the id offset for this mesh
  int entity_id_offset() const;
  
    //! set the id offset for this mesh
  void entity_id_offset(const int id);
  
    //! get the number of entities in this set
  int num_entities() const;
  
    //! get the connectivity array
  bool elem_connectivity(const int begin_handle,
                         const int end_handle,
                         int **vec, int *vec_length) const;
  
    //! set the connectivity array
  bool set_elem_connectivity(const int begin_handle,
                             const int end_handle,
                             const int *vec, 
                             const int vec_length) const;
  
    //! get the coordinate arrays
  bool node_x(const int begin_handle, const int end_handle,
              double **vec, int *num_entities) const;
  bool node_y(const int begin_handle, const int end_handle,
              double **vec, int *num_entities) const;
  bool node_z(const int begin_handle, const int end_handle,
              double **vec, int *num_entities) const;

    //! set the coordinate arrays
  bool set_node_x(const int begin_handle, const int end_handle,
                  const double *vec, const int num_entities);
  bool set_node_y(const int begin_handle, const int end_handle,
                  const double *vec, const int num_entities);
  bool set_node_z(const int begin_handle, const int end_handle,
                  const double *vec, const int num_entities);

    //! get the coordinates for a node (index in global space)
//  VerdeVector get_coordinates(const int index) const;
  
    //! return a reference to the global RMeshSet list for entity_type
  static std::map<int, MDB_RMeshSet*> &global_rmesh_sets(const int entity_type);

    //! functions to return coordinate tag handles; this version checks for tag
    //! and creates one if missing
  int xcoord_tag() const, ycoord_tag() const, zcoord_tag() const;

    //! function to return connectivity tag handle
  int connectivity_tag() const;

    //! return the entityType
  int entity_type() const;
  
    //! return the entityTopology
  int entity_topology() const;
  
    //! return the verticesPerElement
  int vertices_per_element() const;
  
  //! finds if the entity_handle is in the RMeshSet
  static bool is_in_rmeshset (TSTT_cEntity_Handle entity_handle, 
                              const MDB_RMeshSet *rmeshset);

  //! finds if the entity_handle is in the RMeshSet, non-static version;
    //! NOTE: doesn't check type of entity handle, for now
  bool is_in_rmeshset (TSTT_cEntity_Handle entity_handle);

  //! finds the rmeshset corresponding to this entity_handle.  Returns 0 if the 
  //! entity_handle is not valid or not found in the database
  static MDB_RMeshSet *find_rmeshset(const EntityType my_type,
                                     const EntityTopology my_topology,
                                     TSTT_cEntity_Handle entity_handle);

    //! function to add an rmeshset to the global map; return true if addition
    //! was successful
  static bool add_rmeshset(MDB_RMeshSet *rmeshset);
  
private:

    //! the entity id offset for my mesh
  int entityIdOffset;

    //! the type of entity I store
  EntityType entityType;

    //! the topology of the entity I store
  EntityTopology entityTopology;

    //! store the vertices per element for element-type sets
  int verticesPerElement;

    //! the number of entities
  int numEntities;

    //! tag handles for this entity type
    // std::vector<int> myTags;
  
    //! keep a global list of RMeshSets
  static std::map<int,MDB_RMeshSet*> globalRMeshSets[TSTT_LAST_TOPOLOGY];

    //! coordinate tags
  int xcoordTag, ycoordTag, zcoordTag;
  
    //! various types of element connectivity tags
  int connectivityTag;

    //! tag server for rmeshsets
  static SparseTagServer sparseTagServer;

    //! function for making new coordinate tag
  int make_coord_tag(const char *tag_name) const;
  
    //! function for making new connectivity tag
  int make_connectivity_tag() const;

    //! private function for getting coordinate values from a tag
  bool get_coordinate(const int begin_handle, const int end_handle,
                      double **vec, int *size_vec, 
                      const int tag_handle) const;
  
    //! private function for setting coordinate values on a tag
  bool set_coordinate(const int begin_handle, const int end_handle,
                      const double *vec, const int size_vec, 
                      const int tag_handle);
  
};

    //! return the entityType
inline int MDB_RMeshSet::entity_type() const
{
  return entityType;
}

inline int MDB_RMeshSet::entity_topology() const
{
  return entityTopology;
}

inline int MDB_RMeshSet::vertices_per_element() const
{
  return verticesPerElement;
}
  
inline int MDB_RMeshSet::entity_id_offset() const
{
  return entityIdOffset;
}
  
inline void MDB_RMeshSet::entity_id_offset(const int id)
{
  entityIdOffset = id;
}
  
inline int MDB_RMeshSet::num_entities() const
{
  return numEntities;
}

inline std::map<int, MDB_RMeshSet*> &MDB_RMeshSet::global_rmesh_sets(const int entity_type) 
{
  return globalRMeshSets[entity_type];
}

inline int MDB_RMeshSet::xcoord_tag() const
{
  if (-1 == xcoordTag)
    const_cast<MDB_RMeshSet*>(this)->xcoordTag = make_coord_tag("xcoord");
  return xcoordTag;
}

inline int MDB_RMeshSet::ycoord_tag() const
{
  if (-1 == ycoordTag) 
    const_cast<MDB_RMeshSet*>(this)->ycoordTag = make_coord_tag("ycoord");
  return ycoordTag;
}

inline int MDB_RMeshSet::zcoord_tag() const
{
  if (-1 == zcoordTag) 
    const_cast<MDB_RMeshSet*>(this)->zcoordTag = make_coord_tag("zcoord");
  return zcoordTag;
}

inline int MDB_RMeshSet::connectivity_tag() const
{
  if (-1 == connectivityTag) 
    const_cast<MDB_RMeshSet*>(this)->connectivityTag = make_connectivity_tag();

  return connectivityTag;
}

inline bool MDB_RMeshSet::node_x(const int begin_handle, const int end_handle,
                                 double **x, int *num_entities) const
{
  return get_coordinate(begin_handle, end_handle,
                        x, num_entities, xcoordTag);
}

inline bool MDB_RMeshSet::node_y(const int begin_handle, const int end_handle,
                                 double **y, int *num_entities) const
{
  return get_coordinate(begin_handle, end_handle,
                        y, num_entities, ycoordTag);
}

inline bool MDB_RMeshSet::node_z(const int begin_handle, const int end_handle,
                                 double **z, int *num_entities) const
{
  return get_coordinate(begin_handle, end_handle,
                        z, num_entities, zcoordTag);
}

inline bool MDB_RMeshSet::set_node_x(const int begin_handle, const int end_handle,
                                     const double *vec, const int num_entities)
{
  return set_coordinate(begin_handle, end_handle,
                        vec, num_entities, xcoord_tag());
}

inline bool MDB_RMeshSet::set_node_y(const int begin_handle, const int end_handle,
                                     const double *vec, const int num_entities)
{
  return set_coordinate(begin_handle, end_handle,
                        vec, num_entities, ycoord_tag());
}

inline bool MDB_RMeshSet::set_node_z(const int begin_handle, const int end_handle,
                                     const double *vec, const int num_entities)
{
  return set_coordinate(begin_handle, end_handle,
                        vec, num_entities, zcoord_tag());
}

#endif
