 //-------------------------------------------------------------------------
// Filename      : ElemRef.hpp 
//
// Purpose       : Base class for a family of lightweight "handles" to
//                 elements.  The actual data for the element is accessed
//                 through the MeshContainer which holds the element.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef ELEM_REF_HPP
#define ELEM_REF_HPP

// definitions needed for derived elements to size arrays

const int NUM_QUAD_NODES = 4;
const int NUM_TRI_NODES  = 3;
const int NUM_EDGE_NODES = 2;

#include <deque>

#include "VerdeMessage.hpp"
#include "MeshContainer.hpp"
#include "VerdeApp.hpp"
#include "Mesh.hpp"


class MeshContainer;
class EdgeElem;
class QuadElem;
class TriElem;


//! Base class for a family of lightweight "handles" to elements.
class ElemRef
{
   
protected:
   
  //! contains the information this ElemRef points to
  MeshContainer *elementContainer;

  //- the singleton node container for all nodes in Verde
  //static MeshContainer *nodeContainer;
   
  //! index of the information in the elementContainer
  int elementIndex;
   
public:

  //! constructor
  ElemRef(MeshContainer &container, int index);

  //! destructor
  virtual ~ElemRef ();

  //! sets the ElemRef to point to the indicated element
  void set(MeshContainer *container, int index);
   
  //! prints the element information
  virtual void print();

  //! returns index number of this element
  int index();

  //! returns block id of this element
  int block_id();

  // sets the ref to point to the new element
  //void set(int index);

  //! returns a list of bounding quads (if any) for the element
  virtual void quad_list(std::deque<QuadElem*>& /*quad_list*/){};

  //! returns a list of bounding tris (if any)
  virtual void tri_list(std::deque<TriElem*>& /*tri_list*/){};
   
  //! returns a list of edges for the element
  virtual void edge_list(std::deque<EdgeElem*>& /*edge_list*/){};

  //! access to the global nodeContainer object
  static MeshContainer *node_container(); 

  //! return the node_id at position index
  int node_id(int index);

  //! returns the number of nodes this element has
  int num_nodes();

  //! returns the original id of the element (from exodus element num map)
  int original_id();

  //! return the node coords for position index
  VerdeVector node_coordinates(int index);

  //! returns the centroid (average) of the nodes
  VerdeVector centroid(int number_nodes);

  //! returns the exodus id assigned (if any) for this nodeRef
  int exodus_id();

  //! sets the exodus id for this nodeRef
  void set_exodus_id(int value);

  //! general marking functions to see if a node has been traversed.
  void mark(VerdeBoolean value = VERDE_TRUE);

  //! general marking functions to see if a node has been traversed.
  VerdeBoolean is_marked();


};

// inline functions

inline int ElemRef::exodus_id()
{
   return elementContainer->exodus_id(elementIndex);
}

inline void ElemRef::set_exodus_id(int value)
{
   elementContainer->set_exodus_id(elementIndex,value);
}

inline void ElemRef::mark(VerdeBoolean value)
{
   elementContainer->mark(elementIndex,value);
}

inline VerdeBoolean ElemRef::is_marked()
{
   return elementContainer->is_marked(elementIndex);
}

inline int ElemRef::index()
{
   return elementIndex;
}

inline int ElemRef::block_id()
{
   return elementContainer->block_id();
}

inline VerdeVector ElemRef::node_coordinates(int index)
{
   return verde_app->mesh()->node_container()->coordinates(node_id(index));
}

inline int ElemRef::original_id()
{
  return verde_app->mesh()->original_id(elementIndex);
}

inline MeshContainer * ElemRef::node_container()
{ 
  return verde_app->mesh()->node_container();
}

#endif

