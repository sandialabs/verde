//-------------------------------------------------------------------------
// Filename      : KnifeRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a knife element.
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

#ifndef KNIFE_REF_HPP
#define KNIFE_REF_HPP

#include "ElemRef.hpp"
#include "MemoryManager.hpp"


class QuadElem;
class EdgeElem;

const int NUM_KNIFE_EDGES = 10;
const int NUM_KNIFE_QUADS = 5;
const int NUM_KNIFE_TRIS  = 0;
const int NUM_KNIFE_NODES  = 7;


class KnifeRef: public ElemRef
{
   static MemoryManager memoryManager;
   static int QuadArray[NUM_KNIFE_QUADS][NUM_QUAD_NODES];
   static int EdgeArray[NUM_KNIFE_EDGES][NUM_EDGE_NODES];
   
   //! creates a QuadElem for the given quad index
   QuadElem *create_quad(int index);

   //! creates a EdgeElem for the given edge index
   EdgeElem *create_edge(int index);


public:

   //! constructor
   KnifeRef(MeshContainer &container, int index);

   //! destructor
   ~KnifeRef ();

   //! class specific new and delete operators   
   SetDynamicMemoryAllocation(memoryManager)
   
   //! prints the element information
   void print();

   //! marks each node of the Knife
   void mark_nodes();

   //! returns a list of bounding quads (if any) for the element
   void quad_list(std::deque<QuadElem*> &quad_list);

   //! returns a list of edges for the element
   void edge_list(std::deque<EdgeElem*> &edge_list);
   
  
};

// inline functions


#endif

