//-------------------------------------------------------------------------
// Filename      : PyramidRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a pyramid element.
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

#ifndef PYRAMID_REF_HPP
#define PYRAMID_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"

class TriElem;
class QuadElem;
class EdgeElem;

const int NUM_PYRAMID_EDGES = 8;
const int NUM_PYRAMID_QUADS = 1;
const int NUM_PYRAMID_TRIS  = 4;
const int NUM_PYRAMID_NODES  = 5;

//! represents a pyramid element in VERDE
class PyramidRef: public ElemRef
{
   static MemoryManager memoryManager;

   static int TriArray[NUM_PYRAMID_TRIS][NUM_TRI_NODES];
   static int EdgeArray[NUM_PYRAMID_EDGES][NUM_EDGE_NODES];
   
   //! creates and returs a tri for the given index
   TriElem *create_tri(int index);

   //! creates and returns a quad for the base of the pyramid
   QuadElem *create_quad();
   
public:

   //! Constructor
   PyramidRef(MeshContainer &container, int index);

   //! Destructor
   ~PyramidRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! prints the element information
   void print();

   //! creates and returns a edge for the given edge index
   EdgeElem *create_edge(int index);

   //! marks each node of the Pyramid
   void mark_nodes();

   //! returns a list of bounding quads (if any) for the element
   void quad_list(std::deque<QuadElem*> &quad_list);

   //! returns a list of bounding tris (if any) for the element
   void tri_list(std::deque<TriElem*> &tri_list);

   //! returns a list of edges for the element
   void edge_list(std::deque<EdgeElem*> &edge_list);
   
};

// inline functions


#endif

