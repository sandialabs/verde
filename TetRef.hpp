//-------------------------------------------------------------------------
// Filename      : TetRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a tet element.
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

#ifndef TET_REF_HPP
#define TET_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"
#include "NodeRef.hpp"

class TriElem;
class EdgeElem;

const int NUM_TET_EDGES = 6;
const int NUM_TET_QUADS = 0;
const int NUM_TET_TRIS  = 4;
const int NUM_TET_NODES  = 4;

//! lightweight class which is a reference to a tet element
class TetRef: public ElemRef
{
   static MemoryManager memoryManager;

   static int TriArray[NUM_TET_TRIS][NUM_TRI_NODES];
   static int EdgeArray[NUM_TET_EDGES][NUM_EDGE_NODES];
   
   //! creates and returns a edge for the given edge index
   EdgeElem *create_edge(int index);
   
public:

   //! constructor
   TetRef(MeshContainer &container, int index);

   //! destructor
   ~TetRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! creates and returns a tri for the given tri index
   TriElem *create_tri(int index);

   //! prints the element information
   void print();
   
   //! marks each node of the Tet
   void mark_nodes();

   //! returns a list of bounding tris (if any) for the element
   void tri_list(std::deque<TriElem*> &tri_list);

   //! returns a list of edges for the element
   void edge_list(std::deque<EdgeElem*> &edge_list);

  
};

// inline functions



#endif

