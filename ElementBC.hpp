//-------------------------------------------------------------------------
// Filename      : ElementBC.hpp 
//
// Purpose       : Represents general boundary condition on a set of elements
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/08/2001
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef ELEMENT_BC_HPP
#define ELEMENT_BC_HPP

#include <deque>

#include "VerdeDefines.hpp"

class QuadElem;
class TriElem;
class EdgeElem;
class Mesh;

//!represents a general boundary condition applied to one or more elements
class ElementBC
{
public:

  //! constructor
  ElementBC(int id);

  //! destructor
  ~ElementBC();

  //! retrieve list of element ids for this BC
  void element_lists(std::deque<QuadElem*> &quad_list,
      std::deque<TriElem*> &tri_list,
      std::deque<EdgeElem*> &edge_list);

  //! return number of elements in this BC
  int number_elements();

  //! return number of active elements (those that will be output)
  int number_active_elements(Mesh *mesh);

  //! store information in ElementBC (ExodusII centric method)
  void load(int number_elements, int *id_list, int *side_list,
                int number_distribution_factors, double *factors);

  //! return id of this ElementBC
  int id();

  //! sets active flag ElementBC::bcActive
  void active(VerdeBoolean value);

  //! gets elementIds array pointer 
  int * get_elementIds();

  //! gets elementSide array pointer
  int * get_elementSides();

  //! gets number dist. factors
  int number_dist_factors();

  //! number active sidesets
  static int number_active_sidesets() { return numActiveSidesets; }

  //! resets static variable 'numActiveSidesets'
  static void static_reset() { numActiveSidesets = 0; }

  //! reads active flag ElementBC::bcActive
  VerdeBoolean is_active();

  //! return pointer to distribution factors
  double *dist_factor_data();

  //! fills factor_array with distribution factors of active array
  void active_dist_factor_data(Mesh *mesh, double *factor_array);

private:

  //! unique id for the elementBC
  int bcId;

  //! number of elements in the ElementBC
  int numberElements;

  //! pointer to a list of element ids for the ElementBC
  int *elementIds;

  //! pointer to a list of element side ids for the ElementBC
  int *elementSides;

  //! number of active sidesets
  static int numActiveSidesets;

  //! number of distribution factors for this ElementBC
  int numberDistributionFactors;

  //! pointer to a list of distribution factors for the elementBC 
  double *distributionFactors;

  //! flag stating whether bc is active
  VerdeBoolean bcActive;

  //! List of Quads calculated for the ElementBC
  std::deque<QuadElem*> quadList;
  
  //! List of Tris calculated for the ElementBC
  std::deque<TriElem*> triList;
  
  //! List of Edges calculated for the ElementBC
  std::deque<EdgeElem*> edgeList;

  //! converts the ids and sides information to polygons
  void find_sides();  

};

//inline void ElementBC::add_element(int id)
//{
//  elementIdList.append(id);
//}

inline int ElementBC::number_elements()
{
  return numberElements;
}

inline int ElementBC::id()
{
  return bcId;
}

inline void ElementBC::active(VerdeBoolean value)
{
        if(value && !bcActive)
          numActiveSidesets++;
        else if( !value && bcActive)
          numActiveSidesets--;

	bcActive = value;
}

inline VerdeBoolean ElementBC::is_active()
{
	return bcActive;
}

inline int * ElementBC::get_elementIds()
{
      return elementIds; 
}

inline int * ElementBC::get_elementSides()
{
      return elementSides; 

}

inline double * ElementBC::dist_factor_data()
{
  return distributionFactors;
}

#endif

