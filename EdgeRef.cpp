//-------------------------------------------------------------------------
// Filename      : EdgeRef.cpp
//
// Purpose       : Lightweight class which is a reference to a edge element.
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

#include "EdgeRef.hpp"
#include "EdgeElem.hpp"
#include "VerdeMessage.hpp"


EdgeRef::EdgeRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

EdgeRef::~EdgeRef()
{
}

void EdgeRef::print()
{
   // print information for EdgeRef

   PRINT_INFO("EdgeRef %d",elementIndex);

   for (int i = 0; i < NUM_EDGE_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}


void EdgeRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_EDGE_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
   }
}

void EdgeRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // add edge to the edge_list

   EdgeElem *edge = new EdgeElem(this->index(), node_id(0), node_id(1));

   edge_list.push_back(edge);
   
}
