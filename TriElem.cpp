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

#include "TriElem.hpp"
#include "EdgeElem.hpp"
#include "VerdeMessage.hpp"
#include "ElemRef.hpp"

int TriElem::edgeArray[3][2] =
{ {0,1},
  {1,2},
  {2,0}
};


TriElem::TriElem(int owner_id, int node_0, int node_1, int node_2)
   : BaseElem(owner_id)
{
   // load info so that lowest id is in position 1

   int min = VERDE_MIN(node_0, node_1);
   min = VERDE_MIN(min, node_2);
   
   if (node_0 == min)
   {
      connectivity[0] = node_0;
      connectivity[1] = node_1;
      connectivity[2] = node_2;
   }
   else if (node_1 == min)
   {
      connectivity[0] = node_1;
      connectivity[1] = node_2;
      connectivity[2] = node_0;
      
   }
   else if (node_2 == min)
   {
      connectivity[0] = node_2;
      connectivity[1] = node_0;
      connectivity[2] = node_1;
   }
   
}

VerdeBoolean TriElem::has_opposite_connectivity(TriElem *other_tri)
{
   // test each node pair to see if they are okay
  //
  // note: SkinTool doesn't check for matches unless the first
  // node ids match, so lets check the 2nd and 3rd nodes, then
  // the 1st one.

  if(connectivity[2] == other_tri->connectivity[1]
      && connectivity[1] == other_tri->connectivity[2]
      && connectivity[0] == other_tri->connectivity[0])
    return VERDE_TRUE;
  return VERDE_FALSE;
     
}

void TriElem::print()
{
   // print out the tri information

   PRINT_INFO("Tri Connectivity:  %d %d %d\n",
              connectivity[0], connectivity[1], connectivity[2]);
}

EdgeElem *TriElem::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->owner_id(), connectivity[edgeArray[index][0]],
                                   connectivity[edgeArray[index][1]]);
   
   return edge;
}

void TriElem::mark_nodes()
{
   MeshContainer *container = ElemRef::node_container();
   container->mark(connectivity[0],VERDE_TRUE);
   container->mark(connectivity[1],VERDE_TRUE);
   container->mark(connectivity[2],VERDE_TRUE);
}

VerdeVector TriElem::normal()
{
   
   MeshContainer *container = ElemRef::node_container();

   // get coordinates

   VerdeVector base = container->coordinates(connectivity[0]);
   VerdeVector one = container->coordinates(connectivity[1]) - base;
   VerdeVector two = container->coordinates(connectivity[2]) - base;

   VerdeVector normal = one * two;
   normal.normalize();

   return normal;
}


VerdeBoolean TriElem::contains(EdgeElem *edge)
{
   // check each edge to see if it matches

   int start_node = edge->node_id(0);
   int end_node = edge->node_id(1);

   if (connectivity[0] != start_node &&
       connectivity[1] != start_node &&
       connectivity[2] != start_node)
   {
      return VERDE_FALSE;
   }

   if (connectivity[0] != end_node &&
       connectivity[1] != end_node &&
       connectivity[2] != end_node)
   {
      return VERDE_FALSE;
   }

   return VERDE_TRUE;
 
}

void TriElem::bounding_box(VerdeVector &min, VerdeVector&max)
{
   // set the initial ranges

   min = coordinates(0);
   max = min;

   for (int i = 1; i < 3; i++)
   {
      VerdeVector coords = coordinates(i);
      
      // do x coords
      
      double val = coords.x();
      if (val < min.x())
         min.x(val);
      else if (val > max.x())
         max.x(val);
      
      // do y coords
      
      val = coords.y();
      if (val < min.y())
         min.y(val);
      else if (val > max.y())
         max.y(val);

      // do z coords
      
      val = coords.z();
      if (val < min.z())
         min.z(val);
      else if (val > max.z())
         max.z(val);
   }
}

