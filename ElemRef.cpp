//-------------------------------------------------------------------------
// Filename      : ElemRef.cc 
//
// Purpose       : Base class for a family of lightweight "handles" to
//                 elements.  The actual data for the element is accessed
//                 through the MeshContainer which holds the element.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "ElemRef.hpp"
#include "ElementBlock.hpp"
#include "MDBInterface.hpp"

ElemRef::ElemRef(MeshContainer &container, int index)
{
   elementContainer = &container;
   elementIndex = index;


   // check if index is within bounds
/*
   if (index < 1 || index > elementContainer->number_elements())
   {
      PRINT_INFO("Verde: Error, element index outside of range...\n");
   }
*/
}

ElemRef::~ElemRef() 
{   
}

void ElemRef::set(MeshContainer *container, int index)
{
  elementContainer = container;
  elementIndex = index;
}

void ElemRef::print()
{
   // error report
   PRINT_INFO("Error, virtual function ElemRef::print() reached...\n");
}

VerdeVector ElemRef::centroid(int number_nodes)
{
  // find the average of the nodes

  VerdeVector centroid(0.0,0.0,0.0);
  
  for(int i = 0; i < number_nodes; i++)
  {
    centroid += node_coordinates(i);
  }
  
  centroid /= (double) number_nodes;
  
  return centroid;
}

int ElemRef::node_id(int index)
{
  //ElementBlock *block = elementContainer->owner();
  //int element_index = elementIndex - block->element_offset();
  
  //return block->node_id(element_index, index);

  // get the index from MDB  NOTE:  This is inefficient for getting a single 
  // id at a time

  // get number of adjacencies (number of nodes in this case)

  int number_nodes;


  // for now, we make one static list big enough for the largest need

  static int node_list[27];

  /*int error =*/ verde_app->MDB()->GetAdjacencies(elementIndex,
                                               HIGHEST_TYPE,
                                               DIM_0_ENTITY,
                                               &number_nodes,
                                               node_list);
  return node_list[index];

  
}

int ElemRef::num_nodes()
{
  int number_nodes;
  /*MDBErrorCode error =*/ verde_app->MDB()->
                       GetNumAdjacencies(elementIndex,
                                         HIGHEST_TYPE,
                                         DIM_0_ENTITY,
                                         &number_nodes);
  return number_nodes;
}

