//-------------------------------------------------------------------------
// Filename      : WedgeRef.cc
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

#include "WedgeRef.hpp"
#include "TriElem.hpp"
#include "QuadElem.hpp"
#include "EdgeElem.hpp"
#include "VerdeMessage.hpp"

int WedgeRef::TriArray[NUM_WEDGE_TRIS][NUM_TRI_NODES] =
{ {0,2,1},
  {3,4,5} 
};

int WedgeRef::QuadArray[NUM_WEDGE_QUADS][NUM_QUAD_NODES] =
{ {0,1,4,3},
  {1,2,5,4}, 
  {2,0,3,5} 
};

int WedgeRef::EdgeArray[NUM_WEDGE_EDGES][NUM_EDGE_NODES] =
{ {0,1},
  {1,2},
  {0,2},
  {0,3}, 
  {1,4},
  {2,5}, 
  {3,4},
  {4,5},
  {3,5} 
};

WedgeRef::WedgeRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

WedgeRef::~WedgeRef()
{
}

void WedgeRef::print()
{
   // print information for WedgeRef

   PRINT_INFO("WedgeRef %d\n",elementIndex);

   for (int i = 0; i < NUM_WEDGE_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}


TriElem *WedgeRef::create_tri(int index)
{
   // create a tri with the correct nodes

   TriElem *tri = new TriElem(this->index(), node_id(TriArray[index][0]),
                                node_id(TriArray[index][1]),
                                node_id(TriArray[index][2]));
   return tri;
}

EdgeElem *WedgeRef::create_edge(int index)
{
   // create an edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->index(), node_id(EdgeArray[index][0]),
                                   node_id(EdgeArray[index][1]));
   return edge;
}

QuadElem *WedgeRef::create_quad(int index)
{
   // create a quad with the correct nodes

   QuadElem *quad = new QuadElem(this->index(), node_id(QuadArray[index][0]),
                                   node_id(QuadArray[index][1]),
                                   node_id(QuadArray[index][2]),
                                   node_id(QuadArray[index][3]));
   
   return quad;
}

void WedgeRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_WEDGE_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
   }
}

void WedgeRef::tri_list(std::deque<TriElem*> &tri_list)
{
   // create the tris and add to the tri_list

   for (int i = 0; i < NUM_WEDGE_TRIS; i++)
   {
      TriElem *tri = create_tri(i);
      tri_list.push_back(tri);
   }
}

void WedgeRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // create the edges and add to the edge_list

   for (int i = 0; i < NUM_WEDGE_EDGES; i++)
   {
      EdgeElem *edge = create_edge(i);
      edge_list.push_back(edge);
   }
}

void WedgeRef::quad_list(std::deque<QuadElem*> &quad_list)
{
   // create the quads and add to the quad_list

   for (int i = 0; i < NUM_WEDGE_QUADS; i++)
   {
      QuadElem *quad = create_quad(i);
      quad_list.push_back(quad);
   }
}
