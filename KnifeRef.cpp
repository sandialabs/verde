//-------------------------------------------------------------------------
// Filename      : KnifeRef.cc
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

#include "KnifeRef.hpp"
#include "QuadElem.hpp"
#include "EdgeElem.hpp"
#include "VerdeMessage.hpp"

int KnifeRef::QuadArray[NUM_KNIFE_QUADS][NUM_QUAD_NODES] =
{ {0,3,2,1},
  {0,1,5,4},
  {1,2,6,5},
  {3,0,4,5},
  {2,3,5,6} 
};

int KnifeRef::EdgeArray[NUM_KNIFE_EDGES][NUM_EDGE_NODES] =
{ {0,1},
  {1,2},
  {2,3},
  {0,3},
  {0,4},
  {1,5},
  {2,6},
  {3,5},
  {4,5},
  {5,6}
};

KnifeRef::KnifeRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

KnifeRef::~KnifeRef()
{
}

void KnifeRef::print()
{
   // print information for KnifeRef

   PRINT_INFO("KnifeRef %d",elementIndex);
  
   for (int i = 0; i < NUM_KNIFE_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}



QuadElem *KnifeRef::create_quad(int index)
{
   // create a quad with the correct nodes

   QuadElem *quad = new QuadElem(this->index(), node_id(QuadArray[index][0]),
                                   node_id(QuadArray[index][1]),
                                   node_id(QuadArray[index][2]),
                                   node_id(QuadArray[index][3]));
   
   return quad;
}

void KnifeRef::quad_list(std::deque<QuadElem*> &quad_list)
{
   // create the quads and add to the quad_list

   for (int i = 0; i < NUM_KNIFE_QUADS; i++)
   {
      QuadElem *quad = create_quad(i);
      quad_list.push_back(quad);
   }
}

void KnifeRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // create the six edges and add to the edge list
   
   for (int i = 0; i < NUM_KNIFE_EDGES; i++)
   {
      EdgeElem *edge = create_edge(i);
      edge_list.push_back(edge);
   }
}


EdgeElem *KnifeRef::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->index(), node_id(EdgeArray[index][0]),
                                   node_id(EdgeArray[index][1]));
   
   return edge;
}

void KnifeRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_KNIFE_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
      
   }
}
