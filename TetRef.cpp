//-------------------------------------------------------------------------
// Filename      : TetRef.cc
//
// Purpose       : Lightweight class which is a reference to a tet element.
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

#include "TetRef.hpp"
#include "TriElem.hpp"
#include "EdgeElem.hpp"

int TetRef::TriArray[NUM_TET_TRIS][NUM_TRI_NODES] =
{ {0,1,3},
  {1,2,3},
  {2,0,3},
  {0,2,1} 
};

int TetRef::EdgeArray[NUM_TET_EDGES][NUM_EDGE_NODES] =
{ {0,1},
  {1,2},
  {0,2},
  {0,3}, 
  {1,3}, 
  {2,3} 
};

TetRef::TetRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

TetRef::~TetRef()
{
}

void TetRef::print()
{
   // print information for TetRef

   PRINT_INFO("TetRef %d",elementIndex);

   for (int i = 0; i < NUM_TET_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}

TriElem *TetRef::create_tri(int index)
{
  // get the array of nodes
  static int node_list[27];
  int number_nodes;
  /*int error =*/ verde_app->MDB()->GetAdjacencies(elementIndex,
                                               HIGHEST_TYPE,
                                               DIM_0_ENTITY,
                                               &number_nodes,
                                               node_list);
   // create a tri with the correct nodes

   TriElem *tri = new TriElem(this->index(), 
                              node_list[TriArray[index][0]],
                              node_list[TriArray[index][1]],
                              node_list[TriArray[index][2]]);
   return tri;
}

EdgeElem *TetRef::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->index(), node_id(EdgeArray[index][0]),
                                   node_id(EdgeArray[index][1]));
   return edge;
}

void TetRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_TET_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
   }
}

void TetRef::tri_list(std::deque<TriElem*> &tri_list)
{
   // create the tris and add to the tri_list

   for (int i = 0; i < NUM_TET_TRIS; i++)
   {
      TriElem *tri = create_tri(i);
      tri_list.push_back(tri);
   }
}

void TetRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // create the edges and add to the edge_list

   for (int i = 0; i < NUM_TET_EDGES; i++)
   {
      EdgeElem *edge = create_edge(i);
      edge_list.push_back(edge);
   }
}
