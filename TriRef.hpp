//-------------------------------------------------------------------------
// Filename      : TriRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a tri element.
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

#ifndef TRI_REF_HPP
#define TRI_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"

class TriElem;
class EdgeElem;

//! NUM_TRI_NODES defined in ELemRef.hpp
const int NUM_TRI_EDGES = 3;

//! lightweight class which is a reference to a tri element 
class TriRef: public ElemRef
{
   static MemoryManager memoryManager;

   static int EdgeArray[NUM_TRI_EDGES][NUM_EDGE_NODES];
   
public:

   //! constructor
   TriRef(MeshContainer &container, int index);

   //! destructor
   ~TriRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! creates and returns a edge for the given edge index
   EdgeElem *create_edge(int index);

   //! prints the element information
   void print();
   
   //! marks each node of the Element
   void mark_nodes();

   //! returns a list of bounding tris (if any) for the element
   void tri_list(std::deque<TriElem*> &tri_list);

   //! returns a list of edges for the element
   void edge_list(std::deque<EdgeElem*> &edge_list);
   
  
};

// inline functions



#endif

