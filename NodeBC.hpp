//-------------------------------------------------------------------------
// Filename      : NodeBC.hpp 
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

#ifndef NODE_BC_HPP
#define NODE_BC_HPP

#include <deque>

#include "VerdeDefines.hpp"

//! represents a boundary condition on a set of nodes
class NodeBC
{
public:

  //! constructor
  NodeBC(int id);

  //! destructor
  ~NodeBC();

  //! adds a node to the NodeBC
  //void add_node_id(int id);

  //! retrieve list of node ids for this BC
  void node_id_list(std::deque<int> &id_list);

  //! return number of active Nodesets 
  static int number_active_nodesets() { return numActiveNodesets; }

  //! reset static variable 'numActiveNodesets'
  static void static_reset() { numActiveNodesets = 0; }

  //! return number of nodes in this BC
  int number_nodes();

  //! return number of active nodes (those that will be output)
  int number_active_nodes();

  //! return number of dist. factors int his BC 
  int number_dist_factors();

  //! store information in NodeBC (ExodusII centric method)
  void load(int number_nodes, int *ids, 
                int number_distribution_factors, double *factors);

  //! return id of this NodeBC
  int id();

  //! return ptr to nodeIds 
  int* get_nodeIds();

  //! sets active flag NodeBC::bcActive
  void active(VerdeBoolean value);

  //! reads active flag NodeBC::bcActive
  VerdeBoolean is_active();

  //! returns the pointer to the distribution factor data
  double *dist_factor_data();

  //! fills factor_array with distribution factors of active array
  void active_dist_factor_data(double *factor_array);

private:

  //! unique id for the nodeBC
  int bcId;

  //! number of nodes in the NodeBC
  int numberNodes;

  //! number active node BC's
  static int numActiveNodesets;

  //! pointer to a list of node ids for the NodeBC
  int *nodeIds;

  //! number of distribution factors for this NodeBC
  int numberDistributionFactors;

  //! pointer to a list of distribution factors for the nodeBC 
  double *distributionFactors;

  //! flag stating whether bc is active
  VerdeBoolean bcActive;


};

//inline void NodeBC::add_node(int id)
//{
//  nodeIdList.append(id);
//}

inline int NodeBC::number_nodes()
{
  return numberNodes;
}

inline int NodeBC::id()
{
  return bcId;
}

inline void NodeBC::active(VerdeBoolean value)
{
        if(value && !bcActive)
          numActiveNodesets++;
        else if( !value && bcActive)
          numActiveNodesets--;

	bcActive = value;
}

inline VerdeBoolean NodeBC::is_active()
{
	return bcActive;
}

inline double * NodeBC::dist_factor_data()
{
  return distributionFactors;
}

#endif

