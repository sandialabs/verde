//-------------------------------------------------------------------------
// Filename      : HexRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a hex element.
//                 The actual data for the node is accessed through the
//                 MeshContainer which holds the element.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef HEX_REF_HPP
#define HEX_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"


class QuadElem;
class EdgeElem;

const int NUM_HEX_EDGES = 12;
const int NUM_HEX_QUADS = 6;
const int NUM_HEX_TRIS  = 0;
const int NUM_HEX_NODES  = 8;

//! represents a hex in VERDE
class HexRef: public ElemRef
{
   static MemoryManager memoryManager;
   static int QuadArray[NUM_HEX_QUADS][NUM_QUAD_NODES];
   static int EdgeArray[NUM_HEX_EDGES][NUM_EDGE_NODES];
   

   //! creates a EdgeElem for the given edge index
   EdgeElem *create_edge(int index);


public:

   //! constructor
   HexRef(MeshContainer &container, int index);

   //! destructor
   ~HexRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! creates a QuadElem for the given quad index
   QuadElem *create_quad(int index);

   //! prints the element information
   void print();

   //! marks each node of the Hex
   void mark_nodes();

   //! returns a list of bounding quads (if any) for the element
   void quad_list(std::deque<QuadElem*> &quad_list);

   //! returns a list of edges for the element
   void edge_list(std::deque<EdgeElem*> &edge_list);
   
  
};

// inline functions

#endif

