//-------------------------------------------------------------------------
// Filename      : MDB_MeshContainer.hpp 
//
// Purpose       : Holds the actual mesh data for a representation-type
//                 MeshSet
//
// Special Notes : This container holds a chunk of node position and/or
//                 element connectivity data, along with functions for accessing
//                 those data.  Nodes and elements in this container follow an
//                 internal numbering system starting at 0.  A container is owned
//                 by and associated to an RMeshSet, which manages global ids, tags,
//                 and other things for these nodes/elements.
//
// Creator       : Tim Tautges (from code by Ray Meyers)
//
// Date          : 3/02
//
// Owner         : Tim Tautges
//-------------------------------------------------------------------------

#ifndef MDB_MESHCONTAINER_HPP
#define MDB_MESHCONTAINER_HPP

#include "MDB.hpp"
#include "MDBInterface.hpp"
#include "../VerdeVector.hpp"
#include "../VerdeString.hpp"
#include "../VerdeDefines.hpp"

class MDB_RMeshSet;


//! container for storing nodes and elements
class MDB_MeshContainer
{
public:
   
    //! Constructor
  MDB_MeshContainer();

    //! Constructor
  MDB_MeshContainer(MDB_RMeshSet *owner);

    //! Destructor
  ~MDB_MeshContainer();

    //! puts the proper nodal data in the MeshData; if copy_data is true,
    //! re-allocate storage for the positions, otherwise use position memory
    //! passed in
  MDBErrorCode load_nodes(const int number_nodes, 
                          double *node_x, double *node_y, double *node_z,
                          const bool copy_data = false);
  
    //! puts the proper element data in the MeshData; increments node #s by prev_numnodes
    //! if that parameter is non-zero; if copy_data is true, re-allocate storage for
    //! connectivity, otherwise use memory passed in
  MDBErrorCode load_elements(const int num_elements,
                            const int nodes_per_elem,
                            int *connect_vector,
                            const int prev_numnodes = 0,
                            const bool copy_data = false);
   
    //! returns vector of coordinates of the node at node_index
  VerdeVector coordinates(int node_index) const;

    //! return number of nodes defined in this container
  int num_nodes() const;

    //! return number of elements defined in this container
  int num_elements() const;

    //! return the element type string for this container
  VerdeString element_type() const;

    //! return the element type string for this container
  void set_element_type(VerdeString new_type);

    //! return the number of nodes per element
  int num_nodes_per_element() const;
  
    //! set the number of nodes per element
  void num_nodes_per_element(const int num_nodes);

    //! sets the owning MeshSet of this mesh container
  void meshset_owner(MDB_RMeshSet *owner_set);

    //! returns the owning block of this mesh container
  MDB_RMeshSet *meshet_owner() const;

    //! return pointers to the coordinate arrays
  const double *node_x() const;
  const double *node_y() const;
  const double *node_z() const;

    //! return pointer to the connectivity array
  const int *elem_connectivity() const;
  
private:

    //! Arrays to hold nodal information for this meshData
  double *nodeX;
  double *nodeY;
  double *nodeZ;

    //! array to hold the element information for this mesheshData
  int *elemConnectivity;

    //! exodus element type (not sure if we need this here?)
  VerdeString elemType;

    //! number of elements in this meshData
  int numElements;

    //! number of nodes in this meshData
  int numNodes;

    //! number of nodes per element in this meshData
  int numNodesPerElement;

  MDB_RMeshSet *meshsetOwner;
    //- the owner of the MeshContainer

};

inline VerdeVector MDB_MeshContainer::coordinates(int node_index) const
{
  double coords[3];
  assert(0 <= node_index && numNodes > node_index);
  return VerdeVector(nodeX[node_index], nodeY[node_index], nodeZ[node_index]);
}

inline int MDB_MeshContainer::num_nodes() const
{
  return numNodes;
}

inline int MDB_MeshContainer::num_elements() const
{
  return numElements;
}

inline VerdeString MDB_MeshContainer::element_type() const
{
  return elemType;
}

inline void MDB_MeshContainer::set_element_type(VerdeString new_type)
{
  elemType = new_type;
}

inline int MDB_MeshContainer::num_nodes_per_element() const
{
  return numNodesPerElement;
}

inline void MDB_MeshContainer::num_nodes_per_element(const int num_nodes) 
{
  numNodesPerElement = num_nodes;
}

inline void MDB_MeshContainer::meshset_owner(MDB_RMeshSet *owner_set) 
{
  meshsetOwner = owner_set;
}

inline MDB_RMeshSet *MDB_MeshContainer::meshet_owner() const
{
  return meshsetOwner;
}

  //! return pointers to the coordinate arrays
inline const double *MDB_MeshContainer::node_x() const
{
  return nodeX;
}

inline const double *MDB_MeshContainer::node_y() const
{
  return nodeY;
}

inline const double *MDB_MeshContainer::node_z() const
{
  return nodeZ;
}

inline const int *MDB_MeshContainer::elem_connectivity() const
{
  return elemConnectivity;
}
  
#endif
