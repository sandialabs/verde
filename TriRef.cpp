//-------------------------------------------------------------------------
// Filename      : TriRef.cpp
//
// Purpose       : Lightweight class which is a reference to a tri element.
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

#include "TriRef.hpp"
#include "EdgeElem.hpp"
#include "TriElem.hpp"


int TriRef::EdgeArray[NUM_TRI_EDGES][NUM_EDGE_NODES] =
{ {0,1},
  {1,2},
  {0,2}
};

TriRef::TriRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

TriRef::~TriRef()
{
}

void TriRef::print()
{
   // print information for TriRef

   PRINT_INFO("TriRef %d",elementIndex);

   for (int i = 0; i < NUM_TRI_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}


EdgeElem *TriRef::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->index(), node_id(EdgeArray[index][0]),
                                   node_id(EdgeArray[index][1]));
   return edge;
}

void TriRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_TRI_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
   }
}

void TriRef::tri_list(std::deque<TriElem*> &tri_list)
{
   // add tri to the tri_list

   TriElem *tri = new TriElem(this->index(), node_id(0),
                                   node_id(1),
                                   node_id(2));

   tri_list.push_back(tri);
   
}

void TriRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // create the edges and add to the edge_list

   for (int i = 0; i < NUM_TRI_EDGES; i++)
   {
      EdgeElem *edge = create_edge(i);
      edge_list.push_back(edge);
   }
}

