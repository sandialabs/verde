//-------------------------------------------------------------------------
// Filename      : QuadElem.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the QuadRef, the QuadElem exists independent of any
//                 ElementBlock (or its MeshContainer). 
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/05/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef QUAD_ELEM_HPP
#define QUAD_ELEM_HPP

#include "VerdeDefines.hpp"
#include "MemoryManager.hpp"
#include "ElemRef.hpp"
#include "QuadRef.hpp"
#include "BaseElem.hpp"

class EdgeElem;
class VerdeVector;

//! represents a four noded polygon in VERDE
class QuadElem   : public BaseElem
{
   static MemoryManager memoryManager;
   static int edgeArray[NUM_QUAD_EDGES][NUM_EDGE_NODES];
   
   //! gives the id's of the four nodes of the polygon   
   int connectivity[NUM_QUAD_NODES];
   
public:

   //! returns non-boolean match condition for quads
   enum MatchCondition {NO_MATCH, MATCH, MATCH_3_NODES};
  
   //! constructor
   QuadElem(int owner_index, int node_0, int node_1, int node_2, int node_3);

   //! copy constructor
   QuadElem(QuadElem &elem);

   //! destructor
   ~QuadElem () {};
   
   //! class specific new and delete operators
   SetDynamicMemoryAllocation(memoryManager)

   //! prints the element information
   void print();

   //! finds whether other_quad is the mirror of this one, signifying that
   //! both are internal quads
   VerdeBoolean has_opposite_connectivity(QuadElem* other_quad);

   //! find out whether this quad shares three nodes with the other_quad
   //! if they do share three nodes, the shared nodes are returned
   VerdeBoolean shares_three_nodes(QuadElem* other_quad, int shared_nodes[3]);

   //! returns the node_id of the node in position index
   int node_id(int index);

   //! creates a EdgeElem for the given edge index
   EdgeElem *create_edge(int index);

   //! set marked flag for all nodes of the elem
   void mark_nodes();   

   //! returns approximate normal of the elem (assumes a planar elem)
   VerdeVector normal();

   //! states whether the nodes in the edge are also consecutive nodes
   //! in this elem
   VerdeBoolean contains(EdgeElem *edge);

   //! states whether the node_id is in the Elem
   VerdeBoolean contains(int node_id);

   //! returns coords of node at index
   VerdeVector coordinates(int node_index);

   //! returns min and max bounds for the elem
   void bounding_box(VerdeVector &min, VerdeVector &max);

   //! reverses the quad connectivity so that the normal is the other way
   void reverse_connectivity();

};

// inline functions

inline int QuadElem::node_id(int index)
{
   return connectivity[index];
}

inline VerdeBoolean QuadElem::contains(int node_id)
{
   if (node_id == connectivity[0] || node_id == connectivity[1] ||
       node_id == connectivity[2] || node_id == connectivity[3])
      return VERDE_TRUE;
   else
      return VERDE_FALSE;
}

inline VerdeVector QuadElem::coordinates(int node_index)
{
   assert( node_index < 4 );
   return ElemRef::node_container()->coordinates(connectivity[node_index]);
}

inline void QuadElem::reverse_connectivity()
{
  int temp = connectivity[1];
  connectivity[1] = connectivity[3];
  connectivity[3] = temp;
}


#endif

