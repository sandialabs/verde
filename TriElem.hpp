//-------------------------------------------------------------------------
// Filename      : TriElem.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the TriRef, the TriElem exists independent of any
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

#ifndef TRI_ELEM_HPP
#define TRI_ELEM_HPP

#include "VerdeDefines.hpp"
#include "MemoryManager.hpp"
#include "ElemRef.hpp"
#include "BaseElem.hpp"

class EdgeElem;
class VerdeVector;

//! class representing a three noded polygon in VERDE.
class TriElem   : public BaseElem
{
   static MemoryManager memoryManager;
   static int edgeArray[NUM_TRI_NODES][NUM_EDGE_NODES];
   
   //! gives the id's of the four nodes of the polygon
   int connectivity[NUM_TRI_NODES];

   
public:

   //! constructor
   TriElem(int owner_index, int node_0, int node_1, int node_2);

   //! destructor
   ~TriElem () {};
   
   //! class specific new and delete operators
   SetDynamicMemoryAllocation(memoryManager)

   //! prints the element information
   void print();

   //! finds whether other_tri is the mirror of this one, signifying that
   //! both are internal tris
   VerdeBoolean has_opposite_connectivity(TriElem *other_tri);

   //! returns the node_id of the node in position index
   int node_id(int index);
  
   //! creates a EdgeElem for the given edge index
   EdgeElem *create_edge(int index);

   //! set marked flag for all nodes of the face
   void mark_nodes();

   //! returns normal of the tri
   VerdeVector normal();

   //! states whether the nodes of the edge are in the triangle.
   VerdeBoolean contains(EdgeElem *edge);

   //! states whether the node_id is in the Tri
   VerdeBoolean contains(int node_id);

   //! returns min and max bounds for the tri
   void bounding_box(VerdeVector &min, VerdeVector &max);

   //! returns coords of node at index
   VerdeVector coordinates(int node_index);

   //! reverses the tri connectivity so that the normal is reversed
   void reverse_connectivity();


};

// inline functions

inline int TriElem::node_id(int index)
{
   return connectivity[index];
}

inline VerdeVector TriElem::coordinates(int node_index)
{
   assert( node_index < 3 );
   return ElemRef::node_container()->coordinates(connectivity[node_index]);
}

inline VerdeBoolean TriElem::contains(int node_id)
{
   if (node_id == connectivity[0] || node_id == connectivity[1] ||
       node_id == connectivity[2] )
      return VERDE_TRUE;
   else
      return VERDE_FALSE;
}

inline void TriElem::reverse_connectivity()
{
  int temp = connectivity[1];
  connectivity[1] = connectivity[2];
  connectivity[2] = temp;
}

#endif

