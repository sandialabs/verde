//-------------------------------------------------------------------------
// Filename      : HexRef.cc
//
// Purpose       : Lightweight class which is a reference to a hex element.
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

#include "HexRef.hpp"
#include "QuadElem.hpp"
#include "EdgeElem.hpp"

int HexRef::QuadArray[NUM_HEX_QUADS][NUM_QUAD_NODES] =
{ {0,1,5,4},
  {1,2,6,5},
  {2,3,7,6},
  {3,0,4,7},
  {0,3,2,1},
  {4,5,6,7} 
};

int HexRef::EdgeArray[NUM_HEX_EDGES][NUM_EDGE_NODES] =
{ {0,1},
  {1,2},
  {2,3},
  {0,3},
  {0,4},
  {1,5},
  {2,6},
  {3,7},
  {4,5},
  {5,6},
  {6,7},
  {4,7},
};

HexRef::HexRef(MeshContainer &container, int index)
        :ElemRef(container,index)
{
}

HexRef::~HexRef()
{
}

void HexRef::print()
{
   // print information for HexRef

   PRINT_INFO("HexRef %d",elementIndex);
  
   for (int i = 0; i < NUM_HEX_NODES; i++)
   {
      VerdeVector coords = node_coordinates(i);
      PRINT_INFO("      NodeRef %d: %3.2f %3.2f %3.2f\n",
                 node_id(i),coords.x(),coords.y(),coords.z());
   }   
}



QuadElem *HexRef::create_quad(int index)
{
  // get the array of nodes
  static int node_list[27];
  int number_nodes;
  int error = verde_app->MDB()->GetAdjacencies(elementIndex,
                                               HIGHEST_TYPE,
                                               DIM_0_ENTITY,
                                               &number_nodes,
                                               node_list);
  
   // create a quad with the correct nodes

   QuadElem *quad = new QuadElem(  this->index(),
                                   node_list[QuadArray[index][0]],
                                   node_list[QuadArray[index][1]],
                                   node_list[QuadArray[index][2]],
                                   node_list[QuadArray[index][3]]);
   
   return quad;
}

void HexRef::quad_list(std::deque<QuadElem*> &quad_list)
{
   // create the quads and add to the quad_list

   for (int i = 0; i < NUM_HEX_QUADS; i++)
   {
      QuadElem *quad = create_quad(i);
      quad_list.push_back(quad);
   }
}

void HexRef::edge_list(std::deque<EdgeElem*> &edge_list)
{
   // create the six edges and add to the edge list
   
   for (int i = 0; i < NUM_HEX_EDGES; i++)
   {
      EdgeElem *edge = create_edge(i);
      edge_list.push_back(edge);
   }
}


EdgeElem *HexRef::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->index(), node_id(EdgeArray[index][0]),
                                   node_id(EdgeArray[index][1]));
   
   return edge;
}

void HexRef::mark_nodes()
{
   // mark each node

   for(int i = 0; i < NUM_HEX_NODES; i++)
   {
	   verde_app->mesh()->node_container()->mark(node_id(i),VERDE_TRUE);
   }
}
