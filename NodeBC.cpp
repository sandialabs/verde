//-------------------------------------------------------------------------
// Filename      : NodeBC.cpp 
//
// Purpose       : Represents general boundary condition on a set of nodes
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/08/2001
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------
/*!
NodeBC represents a general boundary condition applied to one or more nodes
*/

#include "NodeBC.hpp"
#include "VerdeDefines.hpp"
#include "NodeRef.hpp"

int NodeBC::numActiveNodesets = 0;


NodeBC::NodeBC(int id)
{
  bcId = id;
  numberNodes = 0;
  nodeIds = NULL;
  numberDistributionFactors = 0;
  distributionFactors = NULL;
  bcActive = VERDE_FALSE;
}

NodeBC::~NodeBC()
{
  // delete any memory

  if (nodeIds)
    delete [] nodeIds;

  if (distributionFactors)
    delete [] distributionFactors;
}

/*!
NodeBC::load stores the information for the NodeBC.  This is an exodusII centric
function. More general access will be provided when VMI is available.
NOTE:  it is assumed that the arrays ids and factors are heap memory, and pointers
to these are stored in NodeBC and deleted in the destructor
*/
void NodeBC::load(int number_nodes, int *ids, 
                              int number_distribution_factors,
                              double *factors)
{
  // we will assume that the arrays ids and factors have heap memory
  // so that we can just store the pointers

  numberNodes = number_nodes;
  nodeIds = ids;
  numberDistributionFactors = number_distribution_factors;
  distributionFactors = factors;

  // temp debugging

 //  PRINT_INFO("NodeBC saved: \n"
//              "   id = %d\n"
//              "   node_ids = ",bcId);
//   for(int i = 0; i < numberNodes; i++)
//   {
//     PRINT_INFO("%d, ",nodeIds[i]);
//   }
//   PRINT_INFO("\n");
}

void NodeBC::node_id_list(std::deque<int> &id_list)
{
  for(int i = 0; i < numberNodes; i++)
  {
    id_list.push_back(nodeIds[i]);
  }
}

int NodeBC::number_dist_factors()
{
   return numberDistributionFactors;
}

int* NodeBC::get_nodeIds()
{
   return nodeIds;
}

int NodeBC::number_active_nodes()
{
  // go through the list of node ids, and find out which are active (marked)

  int count = 0;

  for(int i = 0; i < numberNodes; i++)
  {
    NodeRef node_ref(nodeIds[i]);
    if (node_ref.is_marked())
    {
      count++;
    }
  }
  return count;
}


//! fills factor_array with distribution factors of active array
void NodeBC::active_dist_factor_data(double *factor_array)
{
  // assume the array is sized correctly (number_active_nodes)

  double *ptr = factor_array;

  // if the node is active, add its distribution factors to array
  for(int i = 0; i < numberNodes; i++)
  {
    NodeRef node_ref(nodeIds[i]);
    if (node_ref.is_marked())
    {
      *ptr++ = distributionFactors[i];
    }
  }
}



