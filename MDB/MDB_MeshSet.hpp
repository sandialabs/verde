//
//-------------------------------------------------------------------------
// Filename      : MDB_MeshSet.hpp 
// Creator       : Tim Tautges
//
// Date          : 02/01/02
//
// Owner         : Tim Tautges
//
// Description   : MDB_MeshSet is the MDB implementation of MeshSet
//-------------------------------------------------------------------------

#ifndef MDB_MESHSET_HPP
#define MDB_MESHSET_HPP



#include <map>
#include <set>
#include <vector>
#include <string>

#include "MDBDefines.h"
#include "../util/MDBRange.hpp"

//#include "MDB_Mesh_QueryInterface.hpp"

//namespace TSTT 
//{

class MDB_MeshSet;

typedef std::map<std::string, MDB_MeshSet*> MeshSet_NameC;
typedef MeshSet_NameC::iterator MeshSet_NameCI;

typedef std::map<int,MDB_MeshSet*> MeshSet_IdC;
typedef MeshSet_IdC::iterator MeshSet_IdCI;

class MDB_MeshSet
{
public:

    //! class-specific OO functions

    //! create an empty meshset
  MDB_MeshSet();

    //! create a non-empty meshset
  MDB_MeshSet( MDBRange<MDBEntityHandle> entities);

    //! virtual destructor
  virtual ~MDB_MeshSet();

    //! get/set id
  virtual const int GetId(int *info) const;
  virtual void SetId(const int set_id, int *info);
    
    //! get/set name
  virtual const std::string &GetName(int *info) const;
  virtual void SetName(const std::string &set_name, int *);
    
    //! get/set mark
  const int GetMark(int *) const;
  void SetMark(const int flag, int *);
    
    //! return a begin iterator to the name list
  static std::map<std::string,MDB_MeshSet*>::const_iterator GetNamesBegin();
    
    //! return an end iterator to the name list
  static std::map<std::string,MDB_MeshSet*>::const_iterator GetNamesEnd();
    
    //! class-wide list of meshset ids and corresponding handles
    //! (should eventually be some smart hashed list or something)
  static std::map<int,MDB_MeshSet*>::const_iterator GetIdsBegin();
    
    //! class-wide list of meshset ids and corresponding handles
    //! (should eventually be some smart hashed list or something)
  static std::map<int,MDB_MeshSet*>::const_iterator GetIdsEnd();
    
    //! return a const reference to the global meshset list
  static const std::set<MDB_MeshSet*> &GlobalMeshSets();
    
    //! return a const reference to the entity list
  const MDBRange<MDBEntityHandle> &MeshSetEntities() const;

    //! return a const reference to the meshset list
  const std::vector<MDB_MeshSet*> &MeshSetMeshSets() const;
    
    //! return a const reference to the child list
  const std::vector<MDB_MeshSet*> &ChildMeshSets() const;
    
    //! return a const reference to the parent list
  const std::vector<MDB_MeshSet*> &ParentMeshSets() const;

    //! recursive implementation; if first_call is true, this is
    //! the first call, otherwise this is the recursive call
  void GetAllMeshSets(std::vector<MDB_MeshSet*> &handle_list,
                          const bool first_call,
                          int *info);
    
    //! recursive implementation; if first_call is true, this is
    //! the first call, otherwise this is the recursive call
  void GetAllMeshSets(std::vector<const MDB_MeshSet*> &handle_list,
                          const bool first_call,
                          int *info) const;
    
    //! get all entities, including those of included MeshSets
  void GetAllEntities(std::vector<int> &handle_list,
                          int *info) const;

    //! get all entities in this MeshSet with the specified type
  void GetEntitiesByType(const int entity_type,
                             std::vector<int> &entity_list,
                             int *info) const;
    
    //! get all entities in this MeshSet with the specified topology
  void GetEntitiesByTopology(const int entity_topology,
                                 std::vector<int> &entity_list,
                                 int *info) const;
    
    //! subtract/intersect/unite meshset_2 from/with/into meshset_1; modifies meshset_1
  static void Subtract(MDB_MeshSet *meshset_1, 
                       MDB_MeshSet *meshset_2, int *info);

  static void Intersect(MDB_MeshSet *meshset_1, 
                        MDB_MeshSet *meshset_2, 
                        int &proper1, int &proper2,
                        int *info);

  static void Unite(MDB_MeshSet *meshset_1, 
                    MDB_MeshSet *meshset_2, int *info);

    //! get all children pointed to by this meshset
  void GetChildren(const int num_hops, std::vector<MDB_MeshSet*> &children,
                       int *info) const;

    //! get all parents pointed to by this meshset
  void GetParents(const int num_hops, std::vector<MDB_MeshSet*> &parents,
                      int *info) const;

  //! returns non-zero if meshsets *are* related topologically
  static const int IsRelated(MDB_MeshSet *meshset_1, 
                             MDB_MeshSet *meshset_2, int *info);

    //! add these entities to this meshset
  void AddEntities(const int *entity_handles,
                       const int num_entities, int *info);
    
    //! add these entities to this meshset
  void AddEntities(const MDBRange<MDBEntityHandle> &entities,
                   int *info);
    
    //! add these meshsets to this meshset
  void AddMeshSets(MDB_MeshSet **meshset_handles,
                   const int num_meshsets, int *info);
    
    //! remove these entities from this meshset
  void RemoveEntities(const int *entity_handles,
                          const int num_entities,
                          int *info);
    
    //! remove these entities from this meshset
  void RemoveEntities(const std::vector<MDBEntityHandle> &entity_handles,
                          int *info);
    
    //! remove these meshsets from this meshset
  int RemoveMeshSets(MDB_MeshSet**meshset_handles,
                         const int num_meshsets, int *info);
    
    //! return the number of entities contained in this meshset
  const int NumEntities(int *) const;
    
    //! return the number of meshsets contained in this meshset
  const int NumMeshSets(int *) const;
    
    //! return the number of entities with the given type contained in this meshset
  const int NumEntitiesByType(const int entity_type,
                                  int *info) const;
    
    //! return the number of entities with the given topology contained in this meshset
  const int NumEntitiesByTopology(const int entity_topology,
                                      int *info) const;

    //! add a parent/child link between the meshsets; returns error if entities are already
    //! related or if child is already a parent of parent
  static void AddParentChild(MDB_MeshSet *parent_meshset, 
                             MDB_MeshSet *child_meshset,
                             int *info);

  //! add multiple parents to multiple children (all parents get related to all children)
  static void AddParentsChildren(MDB_MeshSet **parent_meshsets, 
                                 const int num_parents,
                                 MDB_MeshSet **child_meshsets,
                                 const int num_children,
                                 int *info);

  //! add multiple parents to multiple children (all parents get related to all children)
  static void AddParentsChildren(std::vector<MDB_MeshSet*> parent_meshsets, 
                                 std::vector<MDB_MeshSet*> child_meshsets, 
                                 int *info);

  //! remove multiple parents from multiple children (all parents get removed from all children)
  static void RemoveParentsChildren(std::vector<MDB_MeshSet*> parent_meshsets, 
                                    std::vector<MDB_MeshSet*> child_meshsets, 
                                    int *info);

  //! remove a parent/child link between the meshsets; returns error if entities
  //! are not related
  static void RemoveParentChild(MDB_MeshSet *parent_meshset, 
                                MDB_MeshSet *child_meshset,
                                int *info);

    //! return the number of children pointed to by this meshset
  const int NumChildren(int *) const;
    
    //! return the number of parents pointed to by this meshset
  const int NumParents(int *) const;
    
private:

    //! add a parent to this meshset; returns true if parent was added, 0 if it was
    //! already a parent of this meshset
  int AddParent(MDB_MeshSet *parent);
    
    //! add a child to this meshset; returns true if child was added, 0 if it was
    //! already a child of this meshset
  int AddChild(MDB_MeshSet *child);
    
    //! remove a parent from this meshset; returns true if parent was removed, 0 if it was
    //! not a parent of this meshset
  int RemoveParent(MDB_MeshSet *parent);
    
    //! remove a child from this meshset; returns true if child was removed, 0 if it was
    //! not a child of this meshset
  int RemoveChild(MDB_MeshSet *child);
    
    //! class-wide list of meshset names and corresponding handles
    //! (should eventually be some smart hashed list or something)
  static MeshSet_NameC meshSetNames;

    //! class-wide list of meshset ids and corresponding handles
    //! (should eventually be some smart hashed list or something)
  static MeshSet_IdC meshSetIds;

    //! class-wide list of meshsets
  static std::set<MDB_MeshSet*> globalMeshSets;

    //! the name of this meshset
  std::string meshSetName;
    
    //! the id of this meshset (unique among all meshsets 
    //! in this interface instance)
  int meshSetId;
    
    //! keep track of the max id
  static int maxMeshSetId;
  
    //! a mark flag, for quick sorting & searching
  int meshSetMark;

    //! set of entities contained in this meshset
  //std::vector<int> meshSetEntities;
  MDBRange<MDBEntityHandle> meshSetEntities;

    //! set of meshsets contained in this meshset
  std::vector<MDB_MeshSet*> meshSetMeshSets;

    //! links to parents/children
  std::vector<MDB_MeshSet*> parentMeshSets, childMeshSets;
    
};
 
inline const std::string &MDB_MeshSet::GetName(int *info) const
{return meshSetName;}

inline const int MDB_MeshSet::GetMark(int *) const
{return meshSetMark;}

inline void MDB_MeshSet::SetMark(const int flag, int *) 
{meshSetMark = flag;}

inline std::map<std::string,MDB_MeshSet*>::const_iterator MDB_MeshSet::GetNamesBegin() 
{return meshSetNames.begin();}

  //! return an end iterator to the name list
inline std::map<std::string,MDB_MeshSet*>::const_iterator MDB_MeshSet::GetNamesEnd() 
{return meshSetNames.end();}

  //! class-wide list of meshset ids and corresponding handles
  //! (should eventually be some smart hashed list or something)
inline std::map<int,MDB_MeshSet*>::const_iterator MDB_MeshSet::GetIdsBegin()
{return meshSetIds.begin();}

  //! class-wide list of meshset ids and corresponding handles
  //! (should eventually be some smart hashed list or something)
inline std::map<int,MDB_MeshSet*>::const_iterator MDB_MeshSet::GetIdsEnd()
{return meshSetIds.end();}

inline const std::set<MDB_MeshSet*> &MDB_MeshSet::GlobalMeshSets() 
{return globalMeshSets;}

  //! return a const reference to the entity list
inline const MDBRange<MDBEntityHandle> &MDB_MeshSet::MeshSetEntities() const
{return meshSetEntities;}

  //! return a const reference to the meshset list
inline const std::vector<MDB_MeshSet*> &MDB_MeshSet::MeshSetMeshSets() const
{return meshSetMeshSets;}

  //! return a const reference to the child list
inline const std::vector<MDB_MeshSet*> &MDB_MeshSet::ChildMeshSets() const
{return childMeshSets;}

  //! return a const reference to the parent list
inline const std::vector<MDB_MeshSet*> &MDB_MeshSet::ParentMeshSets() const
{return parentMeshSets;}
//}
#endif
