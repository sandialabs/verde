//-------------------------------------------------------------------------
// Filename      : QuadRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a quad element.
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

#ifndef QUAD_REF_HPP
#define QUAD_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"

class QuadElem;
class EdgeElem;

//! NUM_QUAD_NODES defined in ElemRef.hpp
const int NUM_QUAD_EDGES = 4;

//! lightweight class which is a reference to a quad element.
class QuadRef: public ElemRef
{
   static MemoryManager memoryManager;

   static int EdgeArray[NUM_QUAD_EDGES][NUM_EDGE_NODES];


public:

   //! constructor
   QuadRef(MeshContainer &container, int index);

   //! destructor
   ~QuadRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! creates and returns a edge for the given edge index
   EdgeElem *create_edge(int index);

   //! prints the element information
   void print();
   
   //! marks each node of the Quad
   void mark_nodes();

   //! returns a list of bounding quads (if any) for the element
   void quad_list(std::deque<QuadElem*> &quad_list);

   //! returns a list of edges for the element
   void edge_list(std::deque<EdgeElem*> &edge_list);
   
  
};

// inline functions


#endif

