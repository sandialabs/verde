//-------------------------------------------------------------------------
// Filename      : PyramidRef.cc
//
// Purpose       : Lightweight class which is a reference to a pyramid element.
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

#include "PyramidRef.hpp"
#include "TriElem.hpp"
#include "QuadElem.hpp"
#include "EdgeElem.hpp"
#include "VerdeMessage.hpp"

int PyramidRef::TriArray[NUM_PYRAMID_TRIS][NUM_TRI_NODES] =
{ {0,1,4},
  {1,2,4},
  {2,3,4},
  {3,0,4} 
};

int PyramidRef::EdgeArray[NUM_PYRAMID_EDGES][NUM_EDGE_NODES] =
{ {0,1},
  {1,2},
  {2,3},
  {0,3}, 
  {0,4},
  {1,4}, 
  {2,4},
  {3,4} 
};



PyramidRef::PyramidRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

PyramidRef::~PyramidRef()
{
}

void PyramidRef::print()
{
   // print information for PyramidRef

   PRINT_INFO("PyramidRef %d",elementIndex);

   for (int i = 0; i < NUM_PYRAMID_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}

TriElem *PyramidRef::create_tri(int index)
{
   // create a tri with the correct nodes

   TriElem *tri = new TriElem(this->index(), node_id(TriArray[index][0]),
                                node_id(TriArray[index][1]),
                                node_id(TriArray[index][2]));
   return tri;
}

EdgeElem *PyramidRef::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->index(), node_id(EdgeArray[index][0]),
                                   node_id(EdgeArray[index][1]));
   return edge;
}

QuadElem *PyramidRef::create_quad()
{
   // create a quad with the correct nodes

   QuadElem *quad = new QuadElem(this->index(), node_id(0),
                                   node_id(3),
                                   node_id(2),
                                   node_id(1));
   return quad;
}

void PyramidRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_PYRAMID_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
   }
}

void PyramidRef::tri_list(std::deque<TriElem*> &tri_list)
{
   // create the tris and add to the tri_list

   for (int i = 0; i < NUM_PYRAMID_TRIS; i++)
   {
      TriElem *tri = create_tri(i);
      tri_list.push_back(tri);
   }
}

void PyramidRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // create the edges and add to the edge_list

   for (int i = 0; i < NUM_PYRAMID_EDGES; i++)
   {
      EdgeElem *edge = create_edge(i);
      edge_list.push_back(edge);
   }
}

void PyramidRef::quad_list(std::deque<QuadElem*> &quad_list)
{
   // add quad to the quad_list

   quad_list.push_back(create_quad());
   
}
