//-------------------------------------------------------------------------
// Filename      : QuadRef.cc
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

#include "QuadRef.hpp"
#include "EdgeElem.hpp"
#include "QuadElem.hpp"
#include "VerdeMessage.hpp"

int QuadRef::EdgeArray[NUM_QUAD_EDGES][NUM_EDGE_NODES] =
{ {0,1},
  {1,2},
  {2,3},
  {0,3}
};

QuadRef::QuadRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

QuadRef::~QuadRef()
{
}

void QuadRef::print()
{
   // print information for QuadRef

   PRINT_INFO("QuadRef %d",elementIndex);

   for (int i = 0; i < NUM_QUAD_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}


EdgeElem *QuadRef::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->index(), node_id(EdgeArray[index][0]),
                                   node_id(EdgeArray[index][1]));
   return edge;
}

void QuadRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_QUAD_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
   }
}

void QuadRef::quad_list(std::deque<QuadElem*> &quad_list)
{
   // add quad to the quad_list

  QuadElem *quad = new QuadElem(this->index(), node_id(0),
                                   node_id(1),
                                   node_id(2),
                                   node_id(3));

   quad_list.push_back(quad);
   
}

void QuadRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // create the edges and add to the edge_list

   for (int i = 0; i < NUM_QUAD_EDGES; i++)
   {
      EdgeElem *edge = create_edge(i);
      edge_list.push_back(edge);
   }
}
