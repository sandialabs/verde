//-------------------------------------------------------------------------
// Filename      : MDB_MeshContainer.cpp
//
// Purpose       : Holds the actual mesh data for a representation-type
//                 MeshSet
//
// Special Notes : (see header)
//
// Creator       : Tim Tautges (from code by Ray Meyers)
//
// Date          : 3/02
//
// Owner         : Tim Tautges
//-------------------------------------------------------------------------

#include "MDB_MeshContainer.hpp"
#include "MDB_MeshSet.hpp"

MDB_MeshContainer::MDB_MeshContainer() 
    : nodeX(NULL), nodeY(NULL), nodeZ(NULL),
      elemConnectivity(NULL), numNodes(0), numElements(0),
      numNodesPerElement(0), meshsetOwner(NULL), elemType()
{
}

MDB_MeshContainer::~MDB_MeshContainer()
{
   // delete node arrays
   if (nodeX)
      delete [] nodeX;
   if (nodeY)
      delete [] nodeY;
   if (nodeZ)
      delete [] nodeZ;
}

MDBErrorCode MDB_MeshContainer::load_nodes(const int number_nodes, 
                                           double *node_x, double *node_y, double *node_z,
                                           const bool copy_data)
{
  assert(numNodes == 0);
  
  numNodes = number_nodes;
  
  if (copy_data) {
    nodeX = new double[numNodes];
    nodeY = new double[numNodes];
    nodeZ = new double[numNodes];
    memcpy(nodeX, node_x, sizeof(double)*numNodes);
    memcpy(nodeY, node_y, sizeof(double)*numNodes);
    memcpy(nodeZ, node_z, sizeof(double)*numNodes);
  }
  else {
    nodeX = node_x;
    nodeY = node_y;
    nodeZ = node_z;
  }
  
  return MDB_SUCCESS;
}

MDBErrorCode MDB_MeshContainer::load_elements(const int num_elements,
                                             const int nodes_per_elem,
                                             int *connect_vector,
                                             const int prev_numnodes,
                                             const bool copy_data)
{
    // copy the connect array if necessary
  if (copy_data) {
    elemConnectivity = new int[sizeof(int)*nodes_per_elem*num_elements];

    if (NULL == elemConnectivity) return MDB_FAILURE;
    
    memcpy(elemConnectivity, connect_vector, sizeof(int)*nodes_per_elem*num_elements);
  }
  else {
    elemConnectivity = connect_vector;
  }
    
    // if prev_numnodes is non-zero, we need to offset the connectivity numbers
    // by that
  if (0 != prev_numnodes) {
    for(int i = 0; i<nodes_per_elem*num_elements; i++)
      elemConnectivity[i] += prev_numnodes;
  }

  return MDB_SUCCESS;
}
