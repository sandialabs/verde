//-------------------------------------------------------------------------
// Filename      : EdgeRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a edge element.
//                 The actual data for the node is accessed through the
//                 MeshContainer which holds the element.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/10/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef EDGE_REF_HPP
#define EDGE_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"

class EdgeElem;

// NUM_EDGE_NODES defined in ElemRef.hpp

//! reference to an edge element.
class EdgeRef: public ElemRef
{
   static MemoryManager memoryManager;

public:

   //! constructor
   EdgeRef(MeshContainer &container, int index);

   //! destructor
   ~EdgeRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! prints the element information
   void print();
   
   //! marks each node of the Element
   void mark_nodes();

   //! returns a list of edges for the element
   void edge_list(std::deque<EdgeElem*> &edge_list);
  
};

// inline functions



#endif

