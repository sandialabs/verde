//-------------------------------------------------------------------------
// Filename      : WedgeRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a wedge element.
//                 The actual data for the node is accessed through the
//                 MeshContainer which holds the element.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 03/10/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef WEDGE_REF_HPP
#define WEDGE_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"

class TriElem;
class QuadElem;
class EdgeElem;

const int NUM_WEDGE_EDGES = 9;
const int NUM_WEDGE_QUADS = 3;
const int NUM_WEDGE_TRIS  = 2;
const int NUM_WEDGE_NODES  = 6;

//! lightweight class which is a reference to a wedge element.
class WedgeRef: public ElemRef
{
   static MemoryManager memoryManager;

   static int TriArray[NUM_WEDGE_TRIS][NUM_TRI_NODES];
   static int EdgeArray[NUM_WEDGE_EDGES][NUM_EDGE_NODES];
   static int QuadArray[NUM_WEDGE_QUADS][NUM_QUAD_NODES];

   //! creates and returs a tri for the given index
   TriElem *create_tri(int index);

   //! creates and returns a quad for the given index
   QuadElem *create_quad(int index);
   
   //! creates and returns a edge for the given index
   EdgeElem *create_edge(int index);

public:

   //! constructor
   WedgeRef(MeshContainer &container, int index);

   //! destructor
   ~WedgeRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! prints the element information
   void print();

   //! marks each node of the Wedge
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

