/*  File       :     BaseElem.hpp
 *
 *  Purpose    :     Base class for the QuadElem, TriElem, EdgeElem classes.
 *
 *  Creator    :     Clinton Stimpson
 *
 *  Date       :     15 Jan 2001
 *
 *  Owner      :     Clinton Stimpson
 */



#ifndef BASEELEM_HPP
#define BASEELEM_HPP


//! base class for EdgeElem, TriElem, and QuadElem
class BaseElem
{
protected:
  //! id of the owning element
  int id_of_owner;
   
public:
   //! constructor and destructor
  BaseElem(int owner_id) { id_of_owner = owner_id; }
  ~BaseElem(){};
   
  //! returns the owner id
  int owner_id() { return id_of_owner; }
};


#endif


