//-------------------------------------------------------------------------
// Filename      : EdgeElem.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the EdgeRef, the EdgeElem exists independent of any
//                 ElementBlock (or its MeshContainer). 
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 04/02/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef EDGE_ELEM_HPP
#define EDGE_ELEM_HPP

#include "VerdeDefines.hpp"
#include "MemoryManager.hpp"
#include "VerdeVector.hpp"
#include "ElemRef.hpp"
#include "BaseElem.hpp"

//! represents a four noded polygon in VERDE.
class EdgeElem   : public BaseElem
{
   static MemoryManager memoryManager;
   
   //! gives the id's of the two nodes of the edge
   int connectivity[2];

   //! int counting the times an edge is marked
   int timesMarked;
   
public:

   //! constructor
   EdgeElem(int owner_index, int node_0, int node_1);

   //! destructor
   ~EdgeElem () {};
   
   //! class specific new and delete operators
   SetDynamicMemoryAllocation(memoryManager)

   //! prints the element information
   void print();
   
   //! returns the node_id of the node in position index
   int node_id(int index);

   //! if true increments the mark, if false sets back to zero
   void mark(VerdeBoolean value = VERDE_TRUE);

   //! returns whether the edge has been marked
   VerdeBoolean is_marked();

   //! returns the number of time an edge has been marked
   int times_marked();

   //! returns the coordinates of node at index
   VerdeVector coordinates(int node_index);
};

// inline functions

inline int EdgeElem::node_id(int index)
{
   return connectivity[index];
}

inline void EdgeElem::mark(VerdeBoolean value)
{
   if (value) 
   {
     timesMarked++;
   }
   else
   {
     timesMarked = 0;
   }
}

inline VerdeBoolean EdgeElem::is_marked()
{
  if(timesMarked)
    return VERDE_TRUE;
 
   return VERDE_FALSE;
}

inline int EdgeElem::times_marked()
{
   return timesMarked;
}

inline VerdeVector EdgeElem::coordinates(int node_index)
{
  assert(node_index < 2);
  return ElemRef::node_container()->coordinates(connectivity[node_index]);
}


class nodes_less
{
public:
  nodes_less(){}
  ~nodes_less(){}
  bool operator()(EdgeElem * const &edge_one, EdgeElem * const &edge_two)
  {
    if(edge_one->node_id(0) < edge_two->node_id(0))
      return true;
    else if(edge_one->node_id(0) == edge_two->node_id(0))
      return edge_one->node_id(1) < edge_two->node_id(1);
    else
      return false;
  }
};
        

#endif

