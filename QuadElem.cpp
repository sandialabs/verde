//-------------------------------------------------------------------------
// Filename      : QuadElem.cc 
//
// Purpose       : Class representing a four noded polygon in VERDE
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/05/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "QuadElem.hpp"
#include "EdgeElem.hpp"
#include "VerdeMessage.hpp"
#include "ElemRef.hpp"

int QuadElem::edgeArray[4][2] =
{ {0,1},
  {1,2},
  {2,3},
  {3,0}
};

QuadElem::QuadElem(QuadElem &elem)
   : BaseElem(elem.id_of_owner)
{
  connectivity[0] = elem.node_id(0);
  connectivity[1] = elem.node_id(1);
  connectivity[2] = elem.node_id(2);
  connectivity[3] = elem.node_id(3);
  assert(node_id(3) == elem.node_id(3));
}

QuadElem::QuadElem(int owner_id, int node_0, int node_1, int node_2, int node_3)
   : BaseElem(owner_id)
{
   // load info so that lowest id is in position 1

   //MeshContainer *node_container = Mesh::instance()->node_container();
   
   int min = VERDE_MIN_4(node_0, node_1, node_2, node_3);

   if (node_0 == min)
   {
      connectivity[0] = node_0;
      connectivity[1] = node_1;
      connectivity[2] = node_2;
      connectivity[3] = node_3;
   }
   else if (node_1 == min)
   {
      connectivity[0] = node_1;
      connectivity[1] = node_2;
      connectivity[2] = node_3;
      connectivity[3] = node_0;
   }
   else if (node_2 == min)
   {
      connectivity[0] = node_2;
      connectivity[1] = node_3;
      connectivity[2] = node_0;
      connectivity[3] = node_1;
   }
   else if (node_3 == min)
   {
      connectivity[0] = node_3;
      connectivity[1] = node_0;
      connectivity[2] = node_1;
      connectivity[3] = node_2;
   }
   
}


VerdeBoolean QuadElem::shares_three_nodes(QuadElem* other_quad, int shared_nodes[3])
{
  // check all 16 possible ways that we could get a match
  // doing it long hand here for speed, so we don't have to do additional operations


#ifdef dummy

  std::cout << 
    connectivity[0] << " " << 
    connectivity[1] << " " << 
    connectivity[2] << " " << 
    connectivity[3] << "    " <<
    other_quad->connectivity[0] << " " << 
    other_quad->connectivity[1] << " " <<
    other_quad->connectivity[2] << " " << 
    other_quad->connectivity[3] << std::endl;

#endif  

  if(connectivity[0] == other_quad->connectivity[0] &&
      connectivity[1] == other_quad->connectivity[3] &&
      connectivity[2] == other_quad->connectivity[2] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[2];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[3] &&
      connectivity[1] == other_quad->connectivity[2] &&
      connectivity[2] == other_quad->connectivity[1] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[2];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[2] &&
      connectivity[1] == other_quad->connectivity[1] &&
      connectivity[2] == other_quad->connectivity[0] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[2];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[1] &&
      connectivity[1] == other_quad->connectivity[0] &&
      connectivity[2] == other_quad->connectivity[3] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[2];
    return VERDE_TRUE;
  }
  if(connectivity[1] == other_quad->connectivity[0] &&
      connectivity[2] == other_quad->connectivity[3] &&
      connectivity[3] == other_quad->connectivity[2] )
  {
    shared_nodes[0] = connectivity[1];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[1] == other_quad->connectivity[3] &&
      connectivity[2] == other_quad->connectivity[2] &&
      connectivity[3] == other_quad->connectivity[1] )
  {
    shared_nodes[0] = connectivity[1];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[1] == other_quad->connectivity[2] &&
      connectivity[2] == other_quad->connectivity[1] &&
      connectivity[3] == other_quad->connectivity[0] )
  {
    shared_nodes[0] = connectivity[1];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[1] == other_quad->connectivity[1] &&
      connectivity[2] == other_quad->connectivity[0] &&
      connectivity[3] == other_quad->connectivity[3] )
  {
    shared_nodes[0] = connectivity[1];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[1] &&
      connectivity[2] == other_quad->connectivity[3] &&
      connectivity[3] == other_quad->connectivity[2] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[0] &&
      connectivity[2] == other_quad->connectivity[2] &&
      connectivity[3] == other_quad->connectivity[1] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[3] &&
      connectivity[2] == other_quad->connectivity[1] &&
      connectivity[3] == other_quad->connectivity[0] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[2] &&
      connectivity[2] == other_quad->connectivity[0] &&
      connectivity[3] == other_quad->connectivity[3] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[2];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[1] &&
      connectivity[1] == other_quad->connectivity[0] &&
      connectivity[3] == other_quad->connectivity[2] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[0] &&
      connectivity[1] == other_quad->connectivity[3] &&
      connectivity[3] == other_quad->connectivity[1] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[3] &&
      connectivity[1] == other_quad->connectivity[2] &&
      connectivity[3] == other_quad->connectivity[0] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }
  if(connectivity[0] == other_quad->connectivity[2] &&
      connectivity[1] == other_quad->connectivity[1] &&
      connectivity[3] == other_quad->connectivity[3] )
  {
    shared_nodes[0] = connectivity[0];
    shared_nodes[1] = connectivity[1];
    shared_nodes[2] = connectivity[3];
    return VERDE_TRUE;
  }

  return VERDE_FALSE;
}


VerdeBoolean QuadElem::has_opposite_connectivity(QuadElem *other_quad)
{

   if(connectivity[3] == other_quad->connectivity[1] &&
      connectivity[1] == other_quad->connectivity[3] &&
      connectivity[2] == other_quad->connectivity[2] &&
      connectivity[0] == other_quad->connectivity[0])
     return VERDE_TRUE;
   return VERDE_FALSE;

}

void QuadElem::print()
{
   // print out the quad information

   PRINT_INFO("Quad Connectivity:  %d %d %d %d\n",
              connectivity[0], connectivity[1], connectivity[2],
              connectivity[3]);
}

EdgeElem *QuadElem::create_edge(int index)
{
   // create a edge with the correct nodes

   EdgeElem *edge = new EdgeElem(this->owner_id(), connectivity[edgeArray[index][0]],
                                   connectivity[edgeArray[index][1]]);
   
   return edge;
}

void QuadElem::mark_nodes()
{
   MeshContainer *container = ElemRef::node_container();
   container->mark(connectivity[0],VERDE_TRUE);
   container->mark(connectivity[1],VERDE_TRUE);
   container->mark(connectivity[2],VERDE_TRUE);
   container->mark(connectivity[3],VERDE_TRUE);
}


VerdeVector QuadElem::normal()
{
   
   MeshContainer *container = ElemRef::node_container();

   // get coordinates

   VerdeVector base = container->coordinates(connectivity[0]);
   VerdeVector one = container->coordinates(connectivity[1]) - base;
   VerdeVector two = container->coordinates(connectivity[3]) - base;

   VerdeVector normal = one * two;
   normal.normalize();

   return normal;
}

VerdeBoolean QuadElem::contains(EdgeElem *edge)
{
   // check each edge to see if it matches

   int start_node = edge->node_id(0);
   int end_node = edge->node_id(1);

   // Do this the explicit way for speed

   if (connectivity[0] == start_node ||
       connectivity[2] == start_node)
   {
      if (connectivity[3] == end_node ||
          connectivity[1] == end_node)
      {
         return VERDE_TRUE;
      }
   }
   else if (connectivity[1] == start_node ||
            connectivity[3] == start_node)
   {
      if (connectivity[0] == end_node ||
          connectivity[2] == end_node)
      {
         return VERDE_TRUE;
      }
   }

   return VERDE_FALSE;
}

void QuadElem::bounding_box(VerdeVector &min, VerdeVector&max)
{
   // set the initial ranges

   min = coordinates(0);
   max = min;

   for (int i = 1; i < 4; i++)
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

   
