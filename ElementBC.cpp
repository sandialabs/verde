//-------------------------------------------------------------------------
// Filename      : ElementBC.cpp 
//
// Purpose       : Represents general boundary condition on a set of elements
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/08/2001
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------
/*!
ElementBC represents a general boundary condition applied to one or more elements
*/


#include "ElementBC.hpp"
#include "VerdeDefines.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"
#include "EdgeElem.hpp"
#include "ElementBlock.hpp"
#include "HexRef.hpp"
#include "TetRef.hpp"
#include "QuadRef.hpp"
#include "TriRef.hpp"

int ElementBC::numActiveSidesets = 0;

ElementBC::ElementBC(int id)
{
  bcId = id;
  numberElements = 0;
  elementIds = NULL;
  elementSides = NULL;
  numberDistributionFactors = 0;
  distributionFactors = NULL;
  bcActive = VERDE_FALSE;
}

ElementBC::~ElementBC()
{
  // delete any memory

  if (elementIds)
    delete [] elementIds;

  if (elementSides)
    delete [] elementSides;

  if (distributionFactors)
    delete [] distributionFactors;

  // delete quads and tris from lists

  unsigned int i;

  for(i=0; i < quadList.size(); i++)
  {
    delete quadList[i];
  }

  for(i=0; i<triList.size(); i++)
  {
    delete triList[i];
  }
  

  for(i=0; i<edgeList.size(); i++)
  {
    delete edgeList[i];
  }
  
}

/*!
ElementBC::load stores the information for the ElementBC. 
This is an exodusII centric function. More general access will be 
provided when VMI is available.
NOTE:  it is assumed that the arrays ids and factors are heap memory, 
and pointers to these are stored in ElementBC and deleted by the ElementBC
*/
void ElementBC::load(int number_elements, int *ids, int *sides,
                              int number_distribution_factors,
                              double *factors)
{
  // we will assume that the arrays ids and factors have heap memory
  // so that we can just store the pointers

  numberElements = number_elements;
  elementIds = ids;
  elementSides = sides;
  numberDistributionFactors = number_distribution_factors;
  distributionFactors = factors;

  // temp debugging

//    PRINT_INFO("ElementBC saved: \n"
//              "   id = %d\n"
//              "   Contents = \n",bcId);
//   for(int i = 0; i < numberElements; i++)
//   {
//     PRINT_INFO("   id: %d   side: %d\n ",elementIds[i], elementSides[i]);
//   }
//   PRINT_INFO("\n");
  
}

void ElementBC::element_lists(std::deque<QuadElem*> &quad_list,
    std::deque<TriElem*> &tri_list,
    std::deque<EdgeElem*> &edge_list)
{
  // if the sides have not been calculated, then do so now

  if (!quadList.size() && !triList.size() && !edgeList.size())
  {
    find_sides();
  }

  unsigned int i;
  for( i=0; i< quadList.size(); i++)
    quad_list.push_back(quadList[i]);
  for( i=0; i< triList.size(); i++)
    tri_list.push_back(triList[i]);
  for( i=0; i< edgeList.size(); i++)
    edge_list.push_back(edgeList[i]);

}


int ElementBC::number_dist_factors()
{
   return numberDistributionFactors;
}

/*!
find_sides() converts information from a list of element ids and side ids
(such as is found in the ExodusII format) to a list of quads,tris, 
and/or edges which can be drawn graphically.  After conversion,
find_sides automatically deletes memory for the old representation
*/

void ElementBC::find_sides()
{
 
  // check to see if this has already been done

  if (quadList.size() || triList.size() || edgeList.size())
  {
    // assume the conversion has taken place

    return;
  }
  

  // We have the id of each element, but at this point we do not know
  // what type of element it is.  We must locate the block it is in, 
  // then we can determine the element type and create a proper element

  ElementBlock *block;
  Mesh *mesh = verde_app->mesh();
 
  for(int i = 0; i < numberElements; i++)
  {
    int element_id = elementIds[i];
    int side_id = elementSides[i];
    
    // get the block for this id
    block = mesh->find_block_for_element(element_id);
    //int index = element_id - block->element_offset();
   
    // check to see if his block has been loaded
    // if not, load it!
    if(!block->elements_loaded())
      mesh->load_element_block(block); 
 
    // find the element type for this block

    VerdeString element_type = block->element_type();
    const char *type = element_type.c_str();
    
    // based on the type of element, create a ref to use

    if (strstr(type,"HEX"))
    {
      // create a hex, and get the correct side

      HexRef hex(*block->mesh_container(),element_id);
      quadList.push_back(hex.create_quad(side_id - 1));
    }
    
    else if (strstr(type,"TET"))
    {
      // create a tet, and get the correct side

      TetRef tet(*block->mesh_container(),element_id);
      triList.push_back(tet.create_tri(side_id - 1));
    }
    
    else if (strstr(type,"QUAD"))
    {
      // create a quad and get the correct edge

      QuadRef quad(*block->mesh_container(),element_id);
      edgeList.push_back(quad.create_edge(side_id - 1));
    }
    
    else if (strstr(type,"TRI"))
    {
      // create a tri and get the correct edge

      TriRef tri(*block->mesh_container(),element_id);
      if(mesh->number_dimensions() == 2)
      {
        triList.push_back(new TriElem(element_id, 
                tri.node_id(0),
                tri.node_id(1),
                tri.node_id(2)));
      }
      else
      {
        if(side_id == 1)
        {
          TriElem* tri_elem = new TriElem(tri.index(),
                                         tri.node_id(0),
                                         tri.node_id(1),
                                         tri.node_id(2));
          triList.push_back(tri_elem);
        }
        else if(side_id == 2)
        {
          TriElem* tri_elem = new TriElem(tri.index(),
                                         tri.node_id(0),
                                         tri.node_id(2),
                                         tri.node_id(1));
          triList.push_back(tri_elem);
        }
        else
        {
          edgeList.push_back(tri.create_edge(side_id - 3));
        }
      }
    }
    
    else if (strstr(type,"SHEL"))
    {
      // create a quad and get the correct edge

      QuadRef quad(*block->mesh_container(),element_id);
      //edgeList.push_back(quad.create_edge(side_id - 1));

      // for a shell, sides 1 and 2 denote the front and back quads

      if (side_id == 1)
      {
        // create a quad with the same connectivity as the QuadRef
        QuadElem *quad_elem = new QuadElem(quad.index(),
                                           quad.node_id(0), 
                                           quad.node_id(1),
                                           quad.node_id(2), 
                                           quad.node_id(3));
        quadList.push_back(quad_elem);
      }
      else if (side_id == 2)
      {
        // create a quad with opposite connectivity to the QuadRef
        QuadElem *quad_elem = new QuadElem(quad.index(),
                                           quad.node_id(0), 
                                           quad.node_id(3),
                                           quad.node_id(2), 
                                           quad.node_id(1));
        quadList.push_back(quad_elem);
      }
      else 
      {
        // create a quad and get the correct edge

        QuadRef quad(*block->mesh_container(),element_id);
        edgeList.push_back(quad.create_edge(side_id - 3));
      }
    }
  }

  // make sure we have enough sides

  assert( (unsigned int)numberElements == (quadList.size() + triList.size()
                             + edgeList.size()));

  // debug contents of sidesets

  // PRINT_INFO("SideSet %d contents: \n",bcId);
//   int i;
//   for(i= 0; i < quadList.size(); i++)
//   {
//     QuadElem *quad = quadList.get_and_step();
//     PRINT_INFO("   Quad: %d %d %d %d\n",quad->node_id(0), quad->node_id(1),
//                quad->node_id(2), quad->node_id(3));
//   }
  
//   for(i= 0; i < triList.size(); i++)
//   {
//     TriElem *tri = triList.get_and_step();
//     PRINT_INFO("   Tri: %d %d %d %d\n",tri->node_id(0), tri->node_id(1),
//                tri->node_id(2), tri->node_id(3));
//   }
  
//   for(i= 0; i < edgeList.size(); i++)
//   {
//     EdgeElem *edge = edgeList.get_and_step();
//     PRINT_INFO("   Edge: %d %d %d %d\n",edge->node_id(0), edge->node_id(1),
//                edge->node_id(2), edge->node_id(3));
//   }
  
}
    
int ElementBC::number_active_elements(Mesh *mesh)
{
  // go through the list of element ids, and find out which are active (marked)

  int count = 0;

  //iterate through sidesets 
  for( int i = 0; i < numberElements; i++ )
  {
     //get original id of element
     int id = elementIds[i];

     //get block element 
     ElementBlock* block = mesh->find_block_for_element(id); 

     //get out if block of element is not active
     if( block->is_active() )
       count++;
  }

  return count;

}


//! fills factor_array with distribution factors of active array
void ElementBC::active_dist_factor_data(Mesh *mesh, double *factor_array)
{
  // assume the array is sized correctly (number_active_elements)

  double *ptr = factor_array;

  //iterate through sidesets 
  for(int i = 0; i < numberElements; i++ )
  {
     //get original id of element
     int id = elementIds[i];

     //get block element 
     ElementBlock* block = mesh->find_block_for_element(id); 

     //get out if block of element is not active
     if( block->is_active() )
    {
      *ptr++ = distributionFactors[i];
    }
  }

}


