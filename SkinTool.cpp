
// Filename      : SkinTool.cc 
//
// Purpose       : For a given set of elements, finds the exterior surface(s)
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/5/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------


#ifdef WIN32
#ifdef _DEBUG
// Turn off warnings that say a debugging identifier has been truncated.
// This warning comes up when using some STL containers.
#pragma warning(4 : 4786)
#endif
#endif


#include <vector>
#include <set>
#include <algorithm>
#include <functional>


#include "SkinTool.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"
#include "EdgeElem.hpp"
#include "ElemRef.hpp"
#include "BucketSort.hpp"
#include "ElementBlock.hpp"
#include "HexRef.hpp"
#include "TetRef.hpp"
#include "PyramidRef.hpp"
#include "WedgeRef.hpp"
#include "EdgeRef.hpp"
#include "QuadRef.hpp"
#include "TriRef.hpp"
#include "KnifeRef.hpp"
#include "GridSearch.hpp"


// define a structure to be used for storing the id and x-value of 
// nodes for sorting and binary search

/*! This structure is used only by the find_coincident_nodes algorithm
to hold sorting information.  Nodes are sorted by x coodinate only, 
and looked up by a binary search
*/
struct NodeInfo
{
  double xVal;
  int nodeId;
};
/*! boolean operator < for use in sorting the NodeInfo information
in find_coincident_nodes algorithm
*/
bool operator<(const NodeInfo &x, const NodeInfo &y)
{
  return x.xVal < y.xVal;
}


//Declare static variables
int SkinTool::QUAD_NODES_PER_LIST = 8;
int SkinTool::TRI_NODES_PER_LIST = 5;

std::deque<std::deque<QuadElem*> > SkinTool::quadList(100);
std::deque<std::multimap<int, TriElem*> > SkinTool::triList(100);

int SkinTool::quadNumberLists = 100;
int SkinTool::triNumberLists = 100;

int SkinTool::numberInteriorQuads = 0;
int SkinTool::numberInteriorTris = 0;
  
std::deque<QuadElem*> SkinTool::coincidentQuadList;
std::deque<TriElem*> SkinTool::coincidentTriList;
std::deque<int> SkinTool::coincidentNodeIdList;
std::deque<TriElem*> SkinTool::nonConformalTriList;
std::deque<QuadElem*> SkinTool::shareThreeNodesQuadList;


// do exterior checks by default
int SkinTool::quad_three_node_check_level = 1;


SkinTool::SkinTool()
{
  skinned = VERDE_FALSE;
  localQuadList = NULL;
  localTriList = NULL;
  nodeDistanceTolerance = 1.0E-06;
  numberLocalQuadLists = 0;
  numberLocalTriLists = 0;
  mInGlobalSkin = false;
  numberLocalInteriorQuads = 0;
  numberLocalInteriorTris = 0;
}

/*! 
initializes (or reinitializes) the SkinTool.
Initializes tool using number_nodes to govern
how many lists are needed for efficient calculations
*/
void SkinTool::initialize(int number_elements)
{
  numberLocalQuadLists = number_elements/QUAD_NODES_PER_LIST + 1;
  numberLocalTriLists = number_elements/TRI_NODES_PER_LIST + 1;
}

void SkinTool::initialize_local_quad_list()
{
  // allocate the quadList array

  localQuadList = new std::deque<QuadElem*>* [numberLocalQuadLists];

   // since the chance is small that any of the lists will never have
   // members, we allocate each list now.

  for (int i = 0; i < numberLocalQuadLists; i++)
  {
    localQuadList[i] = new std::deque<QuadElem*>;
  }
}

void SkinTool::initialize_local_tri_list()
{
  // allocate the localTriList array

  localTriList = new std::multimap<int, TriElem* >* [numberLocalTriLists];

   // since the chance is small that any of the lists will never have
   // members, we allocate each list now.

  for (int i = 0; i < numberLocalTriLists; i++)
  {
    localTriList[i] = new std::multimap<int, TriElem*>;
  }
  
}

SkinTool::~SkinTool()
{

  // reset will clean up this tool
  reset();
  delete_list_memory();	

}

void SkinTool::add_local_quad(QuadElem *new_quad)
{
  assert(new_quad != 0);
  // find the right list and append
  int index = new_quad->node_id(0) % numberLocalQuadLists;
  localQuadList[index]->push_back(new_quad);
   
}

void SkinTool::add_quad(QuadElem *new_quad)
{
  assert(new_quad != 0);
  // find the right list and append
  int index = new_quad->node_id(0) % quadNumberLists; 
  quadList[index].push_back(new_quad);
   
}

void SkinTool::add_local_tri(TriElem *new_tri)
{
  assert(new_tri !=0);

  int index = new_tri->node_id(0) % numberLocalTriLists;

  // the key by which the tri is indexed in the container
  int tri_key = 
    new_tri->node_id(1) +
    new_tri->node_id(2) ;
  
  // add the tri to the container
  localTriList[index]->insert( 
      std::multimap< int, TriElem* >::value_type(tri_key, new_tri));
}

void SkinTool::add_tri(TriElem *new_tri)
{

  assert(new_tri !=0);

  int index = new_tri->node_id(0) % triNumberLists;

  // the key by which the tri is indexed in the container
  int tri_key = 
    new_tri->node_id(1) +
    new_tri->node_id(2) ;
  
  // add the tri to the container
  triList[index].insert( 
      std::multimap< int, TriElem* >::value_type(tri_key, new_tri));

}


VerdeStatus SkinTool::find_local_match(QuadElem *quad)
{
  // find the list index
  int index = quad->node_id(0) % numberLocalQuadLists;

  std::deque<QuadElem*>::iterator localQuadList_iterator;
  std::deque<QuadElem*>::iterator end_list;

  end_list = localQuadList[index]->end();
  QuadElem* test_quad;

  for(localQuadList_iterator = localQuadList[index]->begin();
      localQuadList_iterator != end_list;
      ++localQuadList_iterator)
  {
    test_quad = *localQuadList_iterator; 
    if( quad->has_opposite_connectivity(test_quad))
    {
      localQuadList[index]->erase(localQuadList_iterator);
      delete test_quad;
      numberLocalInteriorQuads++;
      return VERDE_SUCCESS;
    }
/*    else if (condition == QuadElem::MATCH_3_NODES)
    {
      // create duplicate quads, it may be that these will get deleted
      QuadElem *dup_quad = new QuadElem(*quad);
      shareThreeNodesQuadList.push_back(dup_quad);
      dup_quad = new QuadElem(*test_quad);
      shareThreeNodesQuadList.push_back(dup_quad);
    }
    */
  }
  return VERDE_FAILURE;

}

VerdeStatus SkinTool::find_match(QuadElem *quad)
{
  // find the list index
  int index = quad->node_id(0) % quadNumberLists;

  std::deque<QuadElem*>::iterator quadList_iterator;
  std::deque<QuadElem*>::iterator end_list;

  end_list = quadList[index].end();
  QuadElem* test_quad;

  for(quadList_iterator = quadList[index].begin();
      quadList_iterator != end_list;
      ++quadList_iterator)
  {
    test_quad = *quadList_iterator; 
    if( quad->has_opposite_connectivity(test_quad))
    {
      quadList[index].erase(quadList_iterator);
      return VERDE_SUCCESS;
    }
  }
  return VERDE_FAILURE;

}

VerdeStatus SkinTool::find_local_match(TriElem* tri)
{
  std::multimap< int, TriElem* >::iterator localTriList_iterator;

  // simple hashing function that says which container this
  // tri could match an existing tri in.
  int index = tri->node_id(0) % numberLocalTriLists;

  // the key the tri is indexed by
  int tri_key =
    tri->node_id(1) +
    tri->node_id(2) ;

  // find possible matches based on the key
  localTriList_iterator = (*localTriList[index]).find(tri_key);

  // if we don't find a match, quit
  if(localTriList_iterator == localTriList[index]->end() )
    return VERDE_FAILURE;

  // if we found possible matches, go through each one to 
  // do a complete matching
  while(localTriList_iterator->first == tri_key &&
      localTriList_iterator != localTriList[index]->end() )
  {
    // if there is  match, delete the match from the container
    if(tri->has_opposite_connectivity( localTriList_iterator->second ) == VERDE_TRUE )
    {
      delete localTriList_iterator->second;
      localTriList[index]->erase(localTriList_iterator);
      numberLocalInteriorTris++;
      return VERDE_SUCCESS;
    }

    // go to the next possible match
    localTriList_iterator++;
  }

  // if we didn't get any matches, just quit
  return VERDE_FAILURE; 
  
}

VerdeStatus SkinTool::find_match(TriElem* tri)
{
  std::multimap< int, TriElem* >::iterator triList_iterator;

  // simple hashing function that says which container this
  // tri could match an existing tri in.
  int index = tri->node_id(0) % triNumberLists;

  // the key the tri is indexed by
  int tri_key =
    tri->node_id(1) +
    tri->node_id(2) ;

  // find possible matches based on the key
  triList_iterator = (triList[index]).find(tri_key);

  // if we don't find a match, quit
  if(triList_iterator == triList[index].end() )
    return VERDE_FAILURE;

  // if we found possible matches, go through each one to 
  // do a complete matching
  while(triList_iterator->first == tri_key &&
      triList_iterator != triList[index].end() )
  {
    // if there is  match, delete the match from the container
    if(tri->has_opposite_connectivity( triList_iterator->second ) == VERDE_TRUE )
    {
      triList[index].erase(triList_iterator);
      return VERDE_SUCCESS;
    }

    // go to the next possible match
    triList_iterator++;
  }

  // if we didn't get any matches, just quit
  return VERDE_FAILURE;
  
}
void SkinTool::process_ref(ElemRef &new_ref)
{
  // get quad list

  static std::deque<QuadElem*> quad_list;
  quad_list.clear();
  new_ref.quad_list(quad_list);
  int number = quad_list.size();
   
  // initialize quadList if necessary

  if (localQuadList == NULL && number)
    initialize_local_quad_list();
   
   // build the quads and add

  int i;
  for ( i = 0; i < number; i++)
  {
    QuadElem *quad = quad_list[i];
    if (find_local_match(quad) == VERDE_FAILURE)
      add_local_quad(quad);
    else 
      delete quad;
  }

  // get tri list

  static std::deque<TriElem*> tri_list;
  tri_list.clear();
  new_ref.tri_list(tri_list);
  number = tri_list.size();
   
   // initialize localTriList if necessary

  if (localTriList == NULL && number)
    initialize_local_tri_list();
   
   // build the tris and add

  for ( i = 0; i < number; i++)
  {
    TriElem *tri = tri_list[i];
    if (find_local_match(tri) == VERDE_FAILURE)
      add_local_tri(tri);
    else 
      delete tri;
  }

}


void SkinTool::exterior_quads (std::deque<QuadElem*> &exterior_list)
{
  // get the exterior quads of the volumetric blocks
  int i, list_size;

  if ( quadList.size() )
  {
    for ( i = 0; i < quadNumberLists; i++)
    {
      if(!quadList[i].empty())
      {
        std::copy(quadList[i].begin(), quadList[i].end(),
          std::back_inserter(exterior_list));
      }
    }
  }
 
  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  
  //loop through block, appending   
  list_size = block_list.size();
  for(i=0; i<list_size; i++)
  {
    ElementBlock *block;
    block = block_list[i];
    if(!block->is_active())
      continue;
    if(strncmp(block->element_type().c_str(), "QUAD", 4) ==0 ||
       strncmp(block->element_type().c_str(), "SHELL", 5 ) == 0 ) 
    { 
      block->skin_tool()->local_exterior_quads( exterior_list );
    } 
  }
}

void SkinTool::local_exterior_quads (std::deque<QuadElem*> &exterior_list)
{
  // get the exterior quads of the volumetric blocks
  if (localQuadList != NULL)
  {
    for (int i = 0; i < numberLocalQuadLists; i++)
    {
      if(!localQuadList[i]->empty())
      {
        std::copy(localQuadList[i]->begin(), localQuadList[i]->end(),
          std::back_inserter(exterior_list));
      }
    }
  }
  
  // append the shell elements to the skin
  std::copy(localQuadShellList.begin(), localQuadShellList.end(),
      std::back_inserter<std::deque<QuadElem*> >(exterior_list));
}
void SkinTool::exterior_tris (std::deque<TriElem*> &exterior_list)
{
  int i, list_size;
  
  // get the exterior tris of the volumetric blocks
  if( !triList.empty() )
  {
    for (int k = 0; k < triNumberLists; k++)
    {
      if(!triList[k].empty())
      {
        for(std::multimap<int,TriElem*>::iterator iter = triList[k].begin();
        iter != triList[k].end(); ++iter)
          exterior_list.push_back(iter->second);
      }
    }
  }

  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  
  //loop through block, appending   
  list_size = block_list.size();
  for(i=0; i<list_size; i++)
  {
    ElementBlock *block;
    block = block_list[i];
    if(!block->is_active())
      continue;
    if(strncmp(block->element_type().c_str(), "TRI", 3) == 0 ) 
    { 
      block->skin_tool()->local_exterior_tris( exterior_list );
    } 
  }
}

void SkinTool::local_exterior_tris (std::deque<TriElem*> &exterior_list)
{

  // get the exterior tris of the volumetric blocks
  if(localTriList != NULL)
  {
    for (int i = 0; i < numberLocalTriLists; i++)
    {
      for(std::multimap<int, TriElem*>::iterator iter = localTriList[i]->begin();
          iter != localTriList[i]->end(); ++iter)
        exterior_list.push_back(iter->second);
    }
  }

  // append the shell elements to the skin
  std::copy(localTriShellList.begin(), localTriShellList.end(),
      std::back_inserter(exterior_list));
   
}

void SkinTool::local_edges(std::deque<EdgeElem*> &exterior_list)
{
  std::copy(localBeamEdgeList.begin(), localBeamEdgeList.end(),
      std::back_inserter(exterior_list));
}


/*!
outputs the edge elements in the skin from any beams in the mesh to the exterior_list
*/
void SkinTool::exterior_edges(std::deque<EdgeElem*> &exterior_list)
{
  unsigned int i, list_size;

  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  
  //loop through block, appending   
  list_size = block_list.size();
  for(i=0; i<list_size; i++)
  {
    ElementBlock *block;
    block = block_list[i];
    if(!block->is_active())
      continue;
   if (strncmp(block->element_type().c_str(),"BEA",3) == 0 ||
           strncmp(block->element_type().c_str(),"BAR",3) == 0 ||
           strncmp(block->element_type().c_str(),"TRU",3) == 0)
   {
     block->skin_tool()->local_edges(exterior_list);
    } 
  }
}

int SkinTool::number_exterior_quads ()
{
  int number = 0;
  if ( quadList.size() )
  {
    for (int i = 0; i< quadNumberLists; i++)
    {
      number += quadList[i].size();
    }
  }

  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  
  //loop through each block, finding ones of QUAD and SHELL   
  unsigned int list_size = block_list.size();
  for(unsigned int i=0; i<list_size; i++)
  {
    ElementBlock *block;
    block = block_list[i];
    if(!block->is_active())
      continue;
    if(strncmp(block->element_type().c_str(), "QUAD", 4) ==0 ||
       strncmp(block->element_type().c_str(), "SHELL", 5 ) == 0 ) 
    { 
      std::deque<QuadElem*> quad_list;
      block->skin_tool()->local_exterior_quads( quad_list );
      number += quad_list.size();
    } 
  }

  return number;

}

int SkinTool::number_exterior_tris ()
{
  int number = 0;
  for (int j = 0; j < triNumberLists; j++)
  {
    number += triList[j].size();
  }
  
  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  
  //loop through block, appending   
  unsigned int list_size = block_list.size();
  for(unsigned int i=0; i<list_size; i++)
  {
    ElementBlock *block;
    block = block_list[i];
    if(!block->is_active())
      continue;
    if(strncmp(block->element_type().c_str(), "TRI", 3) == 0 ) 
    { 
      std::deque<TriElem*> tri_list;
      block->skin_tool()->local_exterior_tris( tri_list );
      number += tri_list.size();
    } 
  }
  return number;
}

/*!
returns the number of exterior edges in the model.  These all come from any
blocks in the model which are of type BEAM, BAR, etc
*/
int SkinTool::number_exterior_edges()
{
  int number = 0;
  unsigned int i, list_size;

  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  
  //loop through block, appending   
  list_size = block_list.size();
  for(i=0; i<list_size; i++)
  {
    ElementBlock *block;
    block = block_list[i];
    if(!block->is_active())
      continue;
   if (strncmp(block->element_type().c_str(),"BEA",3) == 0 ||
           strncmp(block->element_type().c_str(),"BAR",3) == 0 ||
           strncmp(block->element_type().c_str(),"TRU",3) == 0)
   {
     std::deque<EdgeElem*> edge_list;
     block->skin_tool()->local_edges(edge_list);
     number += edge_list.size();
    } 
  }

  return number;

}

void SkinTool::print_summary()
{
  // print out results

  std::deque<QuadElem*> quad_list;
  exterior_quads(quad_list);

  if (quad_list.size())
    PRINT_INFO("\nNumber of Exterior Quads: %d\n\n",quad_list.size());

  std::deque<TriElem*> tri_list;
  exterior_tris(tri_list);

  if (tri_list.size())
    PRINT_INFO("\nNumber of Exterior Tris: %d\n\n",tri_list.size());

  // print out each quad also

  //for (int i = tri_list.size(); i > 0; i--)
  //{
  //   tri_list.get_and_step()->print();
  //}
}

void SkinTool::find_coincident_elements()
{
  coincidentQuadList.clear();
  coincidentTriList.clear();

  // only if we have coincident nodes can we have coincident elements
  if(number_coincident_nodes() == 0)
    return;

  // set up bucket sort for speed

  VerdeVector body_min, body_max, elem_min, elem_max;
  QuadElem *quad;
  TriElem *tri;

   // get the exterior quads and tris to calculate bounds
   
  std::deque<QuadElem*> quad_list;
  exterior_quads(quad_list);
  std::deque<TriElem*> tri_list;
  exterior_tris(tri_list);

   // make sure we have a boundary to work with

  if (quad_list.empty() && tri_list.empty())
    return;
   

   // set initial bounds
  if (!quad_list.empty())
  { 
    quad = quad_list[0];
    quad->bounding_box(body_min,body_max);
  }
  else if (!tri_list.empty())
  { 
    tri = tri_list[0];
    tri->bounding_box(body_min,body_max);
  }

  // now, set bounds for quads
  std::deque<QuadElem*>::iterator qter = quad_list.begin();
  for( ; qter != quad_list.end(); ++qter)
  {
    quad = *qter;
    quad->bounding_box(elem_min,elem_max);
    // set body bounds if necessary
    if (elem_min.x() < body_min.x())
      body_min.x(elem_min.x());
    else if (elem_max.x() > body_max.x())
      body_max.x(elem_max.x());
    if (elem_min.y() < body_min.y())
      body_min.y(elem_min.y());
    else if (elem_max.y() > body_max.y())
      body_max.y(elem_max.y());
    if (elem_min.z() < body_min.z())
      body_min.z(elem_min.z());
    else if (elem_max.z() > body_max.z())
      body_max.z(elem_max.z());
  }   

  // now repeat for tris
   
  std::deque<TriElem*>::iterator tter = tri_list.begin();
  for( ; tter != tri_list.end(); ++tter)
  {
    tri = *tter;
    tri->bounding_box(elem_min,elem_max);
    // set body bounds if necessary
    if (elem_min.x() < body_min.x())
      body_min.x(elem_min.x());
    else if (elem_max.x() > body_max.x())
      body_max.x(elem_max.x());
    if (elem_min.y() < body_min.y())
      body_min.y(elem_min.y());
    else if (elem_max.y() > body_max.y())
      body_max.y(elem_max.y());
    if (elem_min.z() < body_min.z())
      body_min.z(elem_min.z());
    else if (elem_max.z() > body_max.z())
      body_max.z(elem_max.z());
  }   

  BucketSort bucket_sort(body_min,body_max,
                         quad_list.size()+tri_list.size());

  // put quads in sort
  for(qter = quad_list.begin(); qter != quad_list.end(); ++qter)
  {
    bucket_sort.add_quad(*qter);
  }


  // now look for intersections
  std::deque<QuadElem*>::iterator qter2;
  for(qter = quad_list.begin(); qter != quad_list.end(); ++qter)
  {
    quad = *qter;
    bucket_sort.remove_quad(quad);
    quad->bounding_box(elem_min, elem_max);
    bucket_sort.set_bounds(elem_min,elem_max);
    std::deque<QuadElem*> test_list;
    bucket_sort.get_neighborhood_quads(test_list);
    
    for(qter2 = test_list.begin(); qter2 != test_list.end(); ++qter2)
    {
      if (coincident_quad(quad, *qter2))
      {
        coincidentQuadList.push_back(quad);
        // we could remove test_quad from our list also for speed
        break;
      }
    }
  }

  // put tris in sort
  for(tter = tri_list.begin(); tter != tri_list.end(); ++tter)
  {
    bucket_sort.add_tri(*tter);
  }


  // now look for intersections
  std::deque<TriElem*>::iterator tter2;
  for(tter = tri_list.begin(); tter != tri_list.end(); ++tter)
  {
    tri = *tter;
    bucket_sort.remove_tri(tri);
    tri->bounding_box(elem_min, elem_max);
    bucket_sort.set_bounds(elem_min,elem_max);
    std::deque<TriElem*> test_list;
    bucket_sort.get_neighborhood_tris(test_list);
    
    for(tter2 = test_list.begin(); tter2 != test_list.end(); ++tter2)
    {
      if (coincident_tri(tri, *tter2))
      {
        coincidentTriList.push_back(tri);
        // we could remove test_tri from our list also for speed
        break;
      }
    }
  }   
}

         

VerdeBoolean SkinTool::coincident_quad(QuadElem *quad0,
                                       QuadElem *quad1)
{
  // The object is to prove that these quads are not coincident
  // (this is the usual case) in the cheapest way possible
  // We assume that no identical quads will exist

  // first, we will test to see whether they share any nodes
  int i;
  for ( i = 0; i < NUM_QUAD_NODES; i++)
  {
    if (quad0->contains(quad1->node_id(i)))
      return VERDE_FALSE;
  }

  // now, we will need to get some coordinates to check

  VerdeVector coords0[4],coords1[4];
   
  for (i = 0; i < NUM_QUAD_NODES; i++)
  {
    coords0[i] = quad0->coordinates(i);
  }

  // set up a tolerance as a function of one edge length of a quad

  VerdeVector diff = coords0[1]-coords0[0];
   
  double TOL = 1E-06 * diff.length_squared();
   

   // check each quad1 coordinate, try to prove it is not coincident with
   // any coord in quad0.
   

  VerdeBoolean coincident = VERDE_TRUE;
   
  for (i = 0; i < NUM_QUAD_NODES && coincident; i++)
  {
    coincident = VERDE_FALSE;
    coords1[i] = quad1->coordinates(i);
    for (int j = 0; j < NUM_QUAD_NODES; j++)
    {
      diff = coords1[i] - coords0[j];
      if (diff.length_squared() < TOL)
      {
        coincident = VERDE_TRUE;
        break;
      }
    }
  }

  return coincident;
}

VerdeBoolean SkinTool::coincident_tri(TriElem *tri0,
                                      TriElem *tri1)
{
  // The object is to prove that these tris are not coincident
  // (this is the usual case) in the cheapest way possible
  // We assume that no identical tris will exist

  // first, we will test to see whether they share any nodes
  int i;
  for (i = 0; i < NUM_TRI_NODES; i++)
  {
    if (tri0->contains(tri1->node_id(i)))
      return VERDE_FALSE;
  }

  // now, we will need to get some coordinates to check

  VerdeVector coords0[4],coords1[4];
   
  for (i = 0; i < NUM_TRI_NODES; i++)
  {
    coords0[i] = tri0->coordinates(i);
  }

  // set up a tolerance as a function of one edge length of a tri

  VerdeVector diff = coords0[1]-coords0[0];
   
  double TOL = 1E-06 * diff.length_squared();
   

   // check each tri1 coordinate, try to prove it is not coincident with
   // any coord in tri0.
   

  VerdeBoolean coincident = VERDE_TRUE;
   
  for (i = 0; i < NUM_TRI_NODES && coincident; i++)
  {
    coincident = VERDE_FALSE;
    coords1[i] = tri1->coordinates(i);
    for (int j = 0; j < NUM_TRI_NODES; j++)
    {
      diff = coords1[i] - coords0[j];
      if (diff.length_squared() < TOL)
      {
        coincident = VERDE_TRUE;
        break;
      }
    }
  }

  return coincident;
}

/*!
Resets all lists contained in the skintool.  Deletes any skin elements
currently defined.  Does not delete the lists themselves.
*/
void SkinTool::reset()
{

  // remove the volume boundary elements

  QuadElem* quad;
  TriElem* tri;
  std::deque<QuadElem*>::iterator localQuadList_iterator;
  std::multimap<int, TriElem*>::iterator localTriList_iterator;

  int i;
  for( i = 0; i < numberLocalQuadLists; i++ )
  {
    if(localQuadList && localQuadList[i])
    {
      for(localQuadList_iterator = localQuadList[i]->begin();
          localQuadList_iterator != localQuadList[i]->end();
          ++localQuadList_iterator)
      {
        if( (quad = *localQuadList_iterator) != NULL)
          delete quad;
      }
      localQuadList[i]->clear();
    }
  }

  // delete the tris in all the containers, then clear out
  // the containers
  for(i=0; i< numberLocalTriLists; i++)
  {		
    if(localTriList && localTriList[i])
    {
      for(localTriList_iterator = localTriList[i]->begin();
          localTriList_iterator != localTriList[i]->end();
          ++localTriList_iterator)
      {
        if( (tri = localTriList_iterator->second) != NULL)
          delete tri;
      }
      localTriList[i]->clear();
    }
  }


  // remove shell elements

  while(!localQuadShellList.empty())
  {
    delete localQuadShellList.back();
    localQuadShellList.pop_back();
  }
  localQuadShellList.resize(0);
 
  while(!localTriShellList.empty())
  {
    delete localTriShellList.back();
    localTriShellList.pop_back();
  }
  localTriShellList.resize(0);

  // remove edge (beam) elements
  while(!localBeamEdgeList.empty())
  {
    delete localBeamEdgeList.back();
    localBeamEdgeList.pop_back();
  }
  localBeamEdgeList.resize(0);
 
}


void SkinTool::reset_calculated()
{
  coincidentQuadList.clear();
  coincidentTriList.clear();
  coincidentNodeIdList.clear();
  nonConformalTriList.clear();

  // clean out the shareThreeNodesQuadList
  std::deque<QuadElem*>::iterator quadlist_iterator;
  for(quadlist_iterator = shareThreeNodesQuadList.begin();
      quadlist_iterator != shareThreeNodesQuadList.end();
      ++quadlist_iterator)
    delete *quadlist_iterator;

  shareThreeNodesQuadList.clear();

}



void SkinTool::clear_skin()
{
  int i;
  
  for(i=0; i<quadNumberLists; i++)
    quadList[i].clear();
  
  for(i=0; i<triNumberLists; i++)
    triList[i].clear();

  numberInteriorQuads = 0;
  numberInteriorTris = 0;

  reset_calculated();
  
}


void SkinTool::process_global_skin(ElementBlock* block, actionType action )
{
  // it has already been added
  if(block->skin_tool()->mInGlobalSkin == true && action == ADD)
    return;
  if(block->skin_tool()->mInGlobalSkin == false && action == REMOVE)
    return;

  // set the flag
  if(action == REMOVE)
    block->skin_tool()->mInGlobalSkin = false;
  if(action == ADD)
    block->skin_tool()->mInGlobalSkin = true;


  int i, list_size;

  //if element type is HEX or TET add to global lists
  if(strncmp(block->element_type().c_str(), "HEX", 3) ==0 ||
     strncmp(block->element_type().c_str(), "KNI", 3) ==0 || 
     strncmp(block->element_type().c_str(),"PYR",3) == 0 ||
     strncmp(block->element_type().c_str(),"WED",3) == 0)
  {

    //get all local exterior quads of the block  
    std::deque<QuadElem*> exterior_quads;  
    block->skin_tool()->local_exterior_quads( exterior_quads ); 
    list_size = exterior_quads.size();

    std::deque<QuadElem*> need_to_find_opposites;

    for(i=0; i<list_size; i++) 
    {
      QuadElem *quad = exterior_quads[i];

      if (action == REMOVE)
      {
        //search for quad in quadList 
        int index = quad->node_id(0) % quadNumberLists;

        std::deque<QuadElem*>::iterator quadList_iterator;
        std::deque<QuadElem*>::iterator end_list;

        end_list = quadList[index].end();

        for(quadList_iterator = quadList[index].begin();
        quadList_iterator != end_list;
        ++quadList_iterator)
        {
          QuadElem *test_quad = *quadList_iterator;
          if( quad == test_quad )
          {
            quadList[index].erase(quadList_iterator);
            break;
          } 
        }
        // we need to find the opposite of this quad
        if(quadList_iterator == end_list)
        {
          numberInteriorQuads--;
          need_to_find_opposites.push_back(quad);
        }
      }
      else
      {
        if (find_match(quad) == VERDE_FAILURE)
          add_quad(quad);
        else
          numberInteriorQuads++;
      }
    }

    // find opposites if we need to and add them to the global skin
    if(!need_to_find_opposites.empty())
    {
      std::deque<QuadElem*> opposites;
      get_opposite_quads( need_to_find_opposites, opposites );
      for(unsigned int k = 0; k<opposites.size(); k++)
        add_quad(opposites[k]);
    }

    // update global number interior quads
    if( action == REMOVE )
      numberInteriorQuads -= block->skin_tool()->numberLocalInteriorQuads;
    else
      numberInteriorQuads += block->skin_tool()->numberLocalInteriorQuads;

  }

  if(strncmp(block->element_type().c_str(), "TET", 3) == 0 ||
    strncmp(block->element_type().c_str(),"PYR",3) == 0 ||
    strncmp(block->element_type().c_str(),"WED",3) == 0)
  {

    std::deque<TriElem*> exterior_tris;  
    block->skin_tool()->local_exterior_tris( exterior_tris ); 
    list_size = exterior_tris.size();

    std::deque<TriElem*> need_to_find_opposites;

    std::multimap< int, TriElem* >::iterator triList_iterator;

    for(i=0; i<list_size; i++) 
    {
      TriElem *tri = exterior_tris[i];

      if( action == REMOVE )
      {
        // simple hashing function that says which container this
        // tri could match an existing tri in.
        int index = tri->node_id(0) % triNumberLists; 

        // the key the tri is indexed by
        int tri_key =
          tri->node_id(1) +
          tri->node_id(2) ;

        // find possible matches based on the key
        triList_iterator = (triList[index]).find(tri_key);

        // if we don't find a match, quit
        if(triList_iterator == triList[index].end() )
        {
          numberInteriorTris--;
          need_to_find_opposites.push_back(tri);
          continue;
        }

        // if we found possible matches, go through each one to 
        // do a complete matching
        while(triList_iterator->first == tri_key &&
          triList_iterator != triList[index].end() )
        {
          // if there is match, delete the match from the container
          if( tri == triList_iterator->second )
          {
            triList[index].erase(triList_iterator);
            break;
          }
          
          triList_iterator++;
        }
        if(triList_iterator == triList[index].end() )
        {
          numberInteriorTris--;
          need_to_find_opposites.push_back(tri);
        }
      }
      else
      {
        if (find_match(tri) == VERDE_FAILURE)
          add_tri(tri);
        else
          numberInteriorTris++;
      }
    }

    // find opposites if we need to and add them to the global skin
    if(!need_to_find_opposites.empty())
    {
      std::deque<TriElem*> opposites;
      get_opposite_tris( need_to_find_opposites, opposites );
      for(unsigned int k = 0; k<opposites.size(); k++)
        add_tri(opposites[k]);
    }

    // update global number interior tris 
    if( action == REMOVE )
      numberInteriorTris -= block->skin_tool()->numberLocalInteriorTris; 
    else
      numberInteriorTris += block->skin_tool()->numberLocalInteriorTris; 

  }
}		
		
		
void SkinTool::process_block(ElementBlock* block)
{
  skinned = VERDE_TRUE;
  int i;
  int lower_range = block->element_offset()+1;
  int upper_range = block->number_elements()+lower_range;
  MeshContainer *mesh_cont = block->mesh_container();
	
  if(strncmp(block->element_type().c_str(), "HEX", 3) ==0)
  {
    for(i=lower_range; i < upper_range; i++)
    {
      HexRef elem(*mesh_cont, i);
      process_ref(elem);
    }
  }
  else if(strncmp(block->element_type().c_str(), "TET", 3) == 0)
  {
    for(i=lower_range; i < upper_range; i++)
    {
      TetRef elem(*mesh_cont, i);
      process_ref(elem);
    }
  }
  else if(strncmp(block->element_type().c_str(), "SHELL", 5 ) == 0 ||
          strncmp(block->element_type().c_str(), "QUAD", 4) == 0)
  {
    std::deque<QuadElem*>quad_list;
    for(i=lower_range; i < upper_range; i++)
    {
      // for this case, create a quad and store in the quadShellList for now
      QuadRef elem(*mesh_cont, i);
      quad_list.clear();
      elem.quad_list(quad_list);
      localQuadShellList.push_back(quad_list[0]);
    }
  }
  else if(strncmp(block->element_type().c_str(),"PYR",3) == 0)
  {
    for(i=lower_range; i < upper_range; i++)
    {
      PyramidRef elem(*mesh_cont, i);
      process_ref(elem);
    }
  }
  else if (strncmp(block->element_type().c_str(),"WED",3) == 0)
  {
    for(i=lower_range; i < upper_range; i++)
    {
      WedgeRef elem(*mesh_cont, i);
      process_ref(elem);
    }
  }
  else if (strncmp(block->element_type().c_str(),"KNI",3) == 0)
  {
    for(i=lower_range; i < upper_range; i++)
    {
      KnifeRef elem(*mesh_cont, i);
      process_ref(elem);
    }
  }
  else if (strncmp(block->element_type().c_str(),"TRI",3) == 0)
  {
    std::deque<TriElem*> tri_list;
    for(i=lower_range; i < upper_range; i++)
    {
      // for this case, create a quad a store in the quadShellList for now
      TriRef elem(*mesh_cont, i);
      tri_list.clear();
      elem.tri_list(tri_list);
      localTriShellList.push_back(tri_list[0]);
    }
  }
  else if (strncmp(block->element_type().c_str(),"BEA",3) == 0 ||
           strncmp(block->element_type().c_str(),"BAR",3) == 0 ||
           strncmp(block->element_type().c_str(),"TRU",3) == 0) 
  {
    std::deque<EdgeElem*> edge_list;
    for(i=lower_range; i < upper_range; i++)
    {
      // for this case, create an edge and store in both beamEdgeList
      EdgeRef elem(*mesh_cont, i);
      edge_list.clear();
      elem.edge_list(edge_list);
      assert(edge_list[0] != 0);
      localBeamEdgeList.push_back(edge_list[0]);
    }
  }
  else
  {
    PRINT_INFO("Block type %s not yet supported by SkinTool\n",
               block->element_type().c_str());
    skinned = VERDE_FALSE;
  }
	
}

/*
void SkinTool::find_active_boundary()
{
  // make sure mesh is loaded

  Mesh *mesh = verde_app->mesh();
  if (!mesh)
  {
    return;
  }

  // reset skintool

  reset();

  // get list of blocks and process active ones

  std::deque<ElementBlock*> block_list;
  mesh->element_blocks(block_list);

  unsigned int list_size = block_list.size();
  for(unsigned int i=0; i<list_size; i++)
  {
    ElementBlock *block = block_list[i];
    if (block->is_active())
    {
      add_block(block);
    }
  }
}
*/

/*!
resets the quadShellList, and goes through the currently active block list
looking for quad shell blocks.  Reloads these into the quadShellList to
make the current boundary correct
*/
/*
void SkinTool::find_active_quad_shell_boundary()
{
  // make sure mesh is loaded

  Mesh *mesh = verde_app->mesh();
  if (!mesh)
  {
    return;
  }

  // reset quadShellList

  unsigned int i, list_size;
  list_size = localQuadShellList.size();
  for(i=0; i<list_size; i++)
    delete localQuadShellList[i];
  localQuadShellList.clear();
  
  // get list of blocks and process active ones

  std::deque<ElementBlock*> block_list;
  mesh->element_blocks(block_list);
  list_size = block_list.size();
  for(i=0; i<list_size; i++)
  {
    ElementBlock *block = block_list[i];
    if (block->is_active()) 
    {
      if(strncmp(block->element_type().c_str(), "SHELL", 5 ) == 0 ||
         strncmp(block->element_type().c_str(), "QUAD", 4) == 0)
      {
        add_block(block);
      }
    }
  }
}
*/
/*!
resets the triShellList, and goes through the currently active block list
looking for tri shell blocks.  Reloads these into the triShellList to
make the current boundary correct
*/
/*
void SkinTool::find_active_tri_shell_boundary()
{
  // make sure mesh is loaded

  Mesh *mesh = verde_app->mesh();
  if (!mesh)
  {
    return;
  }

  // reset triShellList

  unsigned int i, list_size;
  list_size = localTriShellList.size();
  for(i=0; i<list_size; i++)
    delete localTriShellList[i];
  localTriShellList.clear();

  // get list of blocks and process active ones

  std::deque<ElementBlock*> block_list;
  mesh->element_blocks(block_list);

  list_size = block_list.size();
  for(i=0; i<list_size; i++)
  {
    ElementBlock *block = block_list[i];
    if (block->is_active()) 
    {
       if (strncmp(block->element_type().c_str(),"TRI",3) == 0)
      {
        add_block(block);
      }
    }
  }
}
*/
/*!
resets the lobalBeamEdgeList, and goes through the currently active block list
looking for BEAM blocks.  Reloads these into the localBeamEdgeList and beamEdgeListo make the current boundary correct
*/
/*
void SkinTool::find_active_beam_edge_boundary()
{
  // make sure mesh is loaded

  Mesh *mesh = verde_app->mesh();
  if (!mesh)
  {
    return;
  }

  // reset localBeamEdgeList

  unsigned int i, list_size;
  list_size = localBeamEdgeList.size();
  for(i=0; i<list_size; i++)
    delete localBeamEdgeList[i];
  localBeamEdgeList.clear();

  // get list of blocks and process active ones

  std::deque<ElementBlock*> block_list;
  mesh->element_blocks(block_list);

  list_size = block_list.size();
  for(i=0; i<list_size; i++)
  {
    ElementBlock *block = block_list[i];
    if (block->is_active()) 
    {
      if (strncmp(block->element_type().c_str(),"BEA",3) == 0 ||
          strncmp(block->element_type().c_str(),"BAR",3) == 0 ||
          strncmp(block->element_type().c_str(),"TRU",3) == 0) 
      {
        add_block(block);
      }
    }
  }
}
*/
/*!
Deletes memory for all lists in the Skintool. 
*/
void SkinTool::delete_list_memory()
{
  
  if (localQuadList != NULL)
  {
    for (int i = 0; i < numberLocalQuadLists; i++)
    {
      delete localQuadList[i];
    }
    delete [] localQuadList;
  }

  // delete all the tri containers
  if (localTriList != NULL)
  {
    for (int i = 0; i < numberLocalTriLists; i++)
    {
      delete localTriList[i];
    }
    delete [] localTriList;
  }
  
  numberLocalQuadLists = 0;
  numberLocalTriLists = 0;
  localQuadList = NULL;
  localTriList = NULL;

}

/*
If there are triangles in the exterior boundary of the mesh, find any of
these which are non-conformal, that is, any tri which shares all three of
its nodes with any quad will have a non-conformal edge in the model
*/
void SkinTool::find_non_conformal_tris()
{

  // reset the current list

  nonConformalTriList.clear();
  
  // If we do not have both tris and quads in the boundary, we are done

  if (quadList.size() == 0 || number_exterior_tris() == 0 || number_exterior_quads() == 0)
  {
    return;
  }

  // get a single list of tris and go through each one

  std::deque<TriElem*> tri_list;
  std::deque<QuadElem*>::iterator quadList_iterator;
  exterior_tris(tri_list);

  unsigned int list_size = tri_list.size();
  for (unsigned int i=0; i<list_size; i++)
  {
    TriElem *tri = tri_list[i];

    // find any quads connected to this node also
    int index = tri->node_id(0) % quadNumberLists; 
    for(quadList_iterator = quadList[index].begin();
        quadList_iterator != quadList[index].end();
        ++quadList_iterator)
    {
      // check each quad to see if it contains both of the tris' other nodes
      QuadElem *quad = *quadList_iterator;
      if (quad->contains(tri->node_id(1)) 
          && quad->contains(tri->node_id(2)))
      {
        nonConformalTriList.push_back(tri);
        break;
      }
    }
  }
}

int SkinTool::number_non_conformal_tris()
{
  return nonConformalTriList.size();
}

void SkinTool::non_conformal_tri_list(std::deque<TriElem*> &tri_list)
{
  unsigned int list_size = nonConformalTriList.size();
  for(unsigned int i=0; i<list_size; i++)
    tri_list.push_back(nonConformalTriList[i]);
}

/*!
find_coincident_nodes uses STL containers and algorithms to improve the
efficiency of searching for coincident nodes.  First, we find the nodes
we wish to check, in this case all nodes on the boundary.  We create a
structure for each node containing only the x value of the node and the node
id.  We put these in an std::vector, and sort.  Finally, we invoke the 
std::binary_search algorithm to find candidate nodes which are close to
the testing node and compare only those as potential coincident nodes
*/
 
 
void SkinTool::find_all_coincident_nodes()  //looks for all concident nodes
{

  unsigned int i, j;
  double tolerance = 0.000001;

  // reset the list of coincident nodes
  coincidentNodeIdList.clear();

  //get all number nodes from mesh
  Mesh *mesh = verde_app->mesh();
  unsigned int number_nodes = mesh->number_nodes();

  //vector to store all nodes of model 
  std::vector<NodeRef*> nodes(number_nodes);

  //create all NodeRef's for the GridSearch
  NodeRef * current_node;
  for(i=1; i<=number_nodes; i++)
  {
    current_node = new NodeRef(i);
    nodes[i-1] = current_node ;
  }
 
  //create a GridSearch item
  GridSearch grid_search(nodes); 

  //add nodes to GridSearch
  grid_search.add_node(nodes);

  //unmark all the nodes
  mesh->node_container()->clear_marked_nodes();
 
  //check for coincidence 
  for(i=0; i<number_nodes; i++)
  {
    current_node = nodes[i];

    //if node is marked, it is coincident
    if( current_node->is_marked() )
      continue;

    VerdeVector node_coords = current_node->coordinates();
    grid_search.set_neighborhood_bounds(node_coords, 0.00001);
    std::deque<NodeRef*> nodes_in_cell;

    //get the nodes in cell of current node 
    grid_search.get_neighborhood_nodes(nodes_in_cell);

    if(nodes_in_cell.size() )
    {
      //now see if the node is within tolerance of the other
      //nodes in the cell
      for(j=0; j<nodes_in_cell.size(); j++)
      {
	NodeRef* other_node= nodes_in_cell[j];
	if( current_node != other_node )
        {
	  VerdeVector possible_match = other_node->coordinates(); 
	  if(fabs(node_coords.x() - possible_match.x()) < tolerance )
	    if(fabs(node_coords.y() - possible_match.y()) < tolerance )
	      if(fabs(node_coords.z() - possible_match.z()) < tolerance )
      	      { 
	        //mark both nodes
	        current_node->mark(); 
	        other_node->mark(); 
	        coincidentNodeIdList.push_back(current_node->index());
  		coincidentNodeIdList.push_back(other_node->index());
	      } 
	}
      }
    } 
  }
  for(i=0; i<number_nodes; i++)
  {
    current_node = nodes[i];
    delete current_node;
  }

  mesh->node_container()-> clear_marked_nodes();
}


void SkinTool::find_coincident_nodes() //looks for coin. nodes on skin
{

  unsigned int i, j;
  // reset the list of coincident nodes
  coincidentNodeIdList.clear();

  NodeRef * current_node;

  MeshContainer *node_container = ElemRef::node_container();
  node_container->clear_marked_nodes();
  //get nodes from skin only
  
  std::deque<QuadElem*> quad_list;
  exterior_quads(quad_list);
  QuadElem *quad;

  unsigned int list_size;
  list_size = quad_list.size();
  std::vector<NodeRef*> nodes;
  for(i=0; i<list_size; i++)
  {
    quad = quad_list[i];
    for(int j = 0; j < 4; j++)
    {
      current_node = new NodeRef(quad->node_id(j));
      if (!current_node->is_marked())
      {
        nodes.push_back(current_node);
        current_node->mark();
      }
      else
	delete current_node;
    }
  }

  std::deque<TriElem*> tri_list;
  exterior_tris(tri_list);
  TriElem *tri;

  list_size = tri_list.size();
  for(i=0; i<list_size; i++)
  {
    tri = tri_list[i];
    for(int j = 0; j < 3; j++)
    {
      current_node = new NodeRef(tri->node_id(j));
      if (!current_node->is_marked())
      {
        nodes.push_back(current_node);
        current_node->mark();
      }
      else
	delete current_node;
    }
  }
  
  std::deque<EdgeElem*> edge_list;
  exterior_edges(edge_list);
  EdgeElem *edge;

  list_size = edge_list.size();
  for(i=0; i<list_size; i++)
  {
    edge = edge_list[i];
    for(int j = 0; j < 2; j++)
    {
      current_node = new NodeRef(edge->node_id(j));
      if (!current_node->is_marked())
      {
        nodes.push_back(current_node);
        current_node->mark();
      }
      else
	delete current_node;
    }
  }

  //create a GridSearch item
  GridSearch grid_search(nodes); 

  //add nodes to GridSearch
  list_size = nodes.size();
  grid_search.add_node(nodes);

  //unmark all the nodes
  node_container->clear_marked_nodes();


  //set the tolerance, if any other nodes are within this tolerance,
  //we're going to get them
  double tolerance = 0.1 * grid_search.get_grid_cell_width(); 

 
  //check for coincidence 
  for(i=0; i<list_size; i++)
  {
    current_node = nodes[i];

    //if node is marked, it is coincident
    if( current_node->is_marked() )
      continue;

    VerdeVector node_coords = current_node->coordinates();
    grid_search.set_neighborhood_bounds(node_coords, tolerance);
    std::deque<NodeRef*> nodes_in_cell;

    grid_search.get_neighborhood_nodes(nodes_in_cell);
    for(j=0; j<nodes_in_cell.size(); j++)
    {
      NodeRef* other_node= nodes_in_cell[j];

      if( current_node == other_node )
        continue;

      VerdeVector possible_match = other_node->coordinates(); 
      // 
      if(fabs(node_coords.x() - possible_match.x()) < tolerance*0.01 )
        if(fabs(node_coords.y() - possible_match.y()) < tolerance*0.01 )
          if(fabs(node_coords.z() - possible_match.z()) < tolerance*0.01 )
          { 
            //mark both nodes
            current_node->mark(); 
            coincidentNodeIdList.push_back(current_node->index());
            if(!(other_node->is_marked()))
            {
               other_node->mark(); 
               coincidentNodeIdList.push_back(other_node->index());
            }
	  } 
    }
  }

  list_size = nodes.size();  
  for(i=0; i<list_size; i++)
  {
    current_node = nodes[i];
    delete current_node;
  }
  node_container->clear_marked_nodes();
}


int SkinTool::number_coincident_nodes()
{
  return coincidentNodeIdList.size();
}

void SkinTool::coincident_node_id_list(std::deque<int> &node_id_list)
{
  unsigned int list_size = coincidentNodeIdList.size();
  for(unsigned int i=0; i<list_size; i++)
    node_id_list.push_back(coincidentNodeIdList[i]);
}

/*! finds quads that share three nodes.  Modes supported are: none, exterior, all.
 exterior gets a list of quads that represent the skin and checks them with each other.
 all gets a list of all the quads in the model and checks them with each other.
*/
void SkinTool::find_quads_share_three_nodes()
{
  
  std::deque<QuadElem*>::iterator quadList_iterator;
  
  // clean out the shareThreeNodesQuadList
  for(quadList_iterator = shareThreeNodesQuadList.begin();
      quadList_iterator != shareThreeNodesQuadList.end();
      ++quadList_iterator)
    delete *quadList_iterator;

  shareThreeNodesQuadList.clear();
  
  std::deque<QuadElem*> quad_list;

  // get the exterior quads only
  if(quad_three_node_check_level == 1)
  {
    if ( quadList.size() )
    {
      for (int i = 0; i < quadNumberLists; i++)
      {
        for(quadList_iterator = quadList[i].begin();
            quadList_iterator != quadList[i].end();
            ++quadList_iterator)
          quad_list.push_back(new QuadElem(**quadList_iterator));
      }
    }
  }
  // get all 6 quads of every hex
  else if(quad_three_node_check_level == 2)
  {
    // note: we are responsible to delete these 
    // quads when we are done
    get_model_hex_quads(quad_list);
  }
  else
    return;

 
  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  std::deque<QuadElem*> shell_list;
  
  //loop through block, appending   
  unsigned int list_size = block_list.size();
  for(unsigned int i=0; i<list_size; i++)
  {
    ElementBlock *block;
    block = block_list[i];
    if(!block->is_active())
      continue;
    if(strncmp(block->element_type().c_str(), "QUAD", 4) ==0 ||
       strncmp(block->element_type().c_str(), "SHELL", 5 ) == 0 ) 
    { 
      std::deque<QuadElem*> quad_list;
      block->skin_tool()->local_exterior_quads( quad_list );
      unsigned int size = quad_list.size();
      for(unsigned int j=0; j<size; j++)
        shell_list.push_back(quad_list[j]);
    } 
  }


  QuadElem *tmp;
  for(unsigned int k=0; k<shell_list.size(); k++)
  {
    tmp = new QuadElem(*(shell_list[k]));
    quad_list.push_back(tmp);
    tmp = new QuadElem(*(shell_list[k]));
    tmp->reverse_connectivity();
    quad_list.push_back(tmp);
  }

  std::deque<QuadElem*>::const_iterator quad_list_iterator;
  std::deque<QuadElem*>::const_iterator curr_test_quad;
  std::deque<QuadElem*>::const_iterator quad_list_end =
    quad_list.end();
  
  QuadElem* curr_quad =0;
  int shared_nodes[3];
 
  // generate the adjacency information that we need 
  std::multimap< int, QuadElem* > node_quad_adjacencies;
  for(quad_list_iterator = quad_list.begin();
      quad_list_iterator != quad_list_end;
      ++quad_list_iterator)
  {
    for(int i=0; i<4; i++)
    {
      node_quad_adjacencies.insert(
          std::multimap< int, QuadElem* >::value_type
          ((*quad_list_iterator)->node_id(i), *quad_list_iterator) 
        );
    }
  }

  std::multimap< int, QuadElem* >::iterator nq_adj_iterator;
  std::multimap< QuadElem*, QuadElem* > quads_sharing_3_nodes;
  
  // check every quad with the other quads 
  for(curr_test_quad = quad_list.begin();
      curr_test_quad != quad_list_end;
      ++curr_test_quad)
  {
    // the current quad in consideration
    curr_quad = *curr_test_quad;

    std::set<QuadElem*> possible_matches;
    std::set<QuadElem*>::iterator possible_match_iterator;

    // get the possible matches based on adjacency information
    for(int i=0; i<4; i++)
    {
      nq_adj_iterator = node_quad_adjacencies.lower_bound(curr_quad->node_id(i));
      if(nq_adj_iterator != node_quad_adjacencies.end())
      {
        // append these find results to the possible matches list
        std::multimap<int, QuadElem*>::iterator nq_end =
          node_quad_adjacencies.upper_bound(curr_quad->node_id(i));
        for( ; nq_adj_iterator != nq_end; ++nq_adj_iterator)
          possible_matches.insert(nq_adj_iterator->second);
      }
    }

    // check the curr_quad against the possible matches    
    for(possible_match_iterator = possible_matches.begin();
        possible_match_iterator != possible_matches.end();
        ++possible_match_iterator)
    {
      // if we find a match, make a copy and put it in our list
      if((curr_quad != *possible_match_iterator)  &&
         (curr_quad->shares_three_nodes(*possible_match_iterator, shared_nodes)
            == VERDE_TRUE)  &&
         !curr_quad->has_opposite_connectivity(*possible_match_iterator))
      {
        // see if this match has already been found
        std::multimap< QuadElem*, QuadElem* >::iterator q3n_lbound =
          quads_sharing_3_nodes.lower_bound( curr_quad );
        std::multimap< QuadElem*, QuadElem* >::iterator q3n_ubound =
          quads_sharing_3_nodes.upper_bound( curr_quad );
        std::multimap< QuadElem*, QuadElem* >::iterator q3n_bound_iterator;

        bool new_match = true;
        for(q3n_bound_iterator = q3n_lbound;
            q3n_bound_iterator != q3n_ubound;
            ++q3n_bound_iterator)
        {
          if(q3n_bound_iterator->second == *possible_match_iterator)
            new_match = false;
        }
        
        // if it is indeed a new case, remember it
        if(new_match == true)
        {
          // add this case to the previous cases
          quads_sharing_3_nodes.insert(
              std::multimap< QuadElem*, QuadElem* >::value_type
              ( curr_quad, *possible_match_iterator ));
          quads_sharing_3_nodes.insert(
              std::multimap< QuadElem*, QuadElem* >::value_type
              ( *possible_match_iterator, curr_quad ));

          // print a warning and store the quad elems
          PRINT_WARNING("Nodes %d, %d and %d are shared by two quads.\n",
              shared_nodes[0], shared_nodes[1], shared_nodes[2]);
          QuadElem* dup_quad = new QuadElem(*curr_quad);
          shareThreeNodesQuadList.push_back(dup_quad);
          dup_quad = new QuadElem(**possible_match_iterator);
          shareThreeNodesQuadList.push_back(dup_quad);
        }
      }
    }
  }

  // delete the QuadElems if we need to
  for(quad_list_iterator = quad_list.begin();
      quad_list_iterator != quad_list_end;
      ++quad_list_iterator)
  {
    delete *quad_list_iterator;
  }

}


/*! note: calling function is responsible to delete these quads
 we are also only checking hexes currently
*/
void SkinTool::get_model_hex_quads(std::deque<QuadElem*>& quad_list)
{

  std::deque<ElementBlock*> element_blocks;
  verde_app->mesh()->element_blocks(element_blocks);

  // if the element block is composed of hexes
  for(unsigned int i=0; i<element_blocks.size(); i++)
  {
    if(strncmp(element_blocks[i]->element_type().c_str(), "HEX", 3) != 0)
      continue;

    // get the range of element ids in this block of hexes
    ElementBlock* block = element_blocks[i];
    int lower_range = block->element_offset()+1;
    int upper_range = block->number_elements()+lower_range;
    MeshContainer *mesh_cont = block->mesh_container();

    std::deque<QuadElem*> hex_quads;
    
    // go through each hex
    for(int j =lower_range; j < upper_range; j++)
    {
      hex_quads.clear();
      // create a hex
      HexRef elem(*mesh_cont, j);
      // get the quads of this hex
      elem.quad_list(hex_quads);
      // put the quads on the quad_list
      for(unsigned int k=0; k<hex_quads.size(); k++)
        quad_list.push_back(hex_quads[k]);        

    }
  }
}


void SkinTool::get_opposite_quads(std::deque<QuadElem*>& get_opposites, std::deque<QuadElem*>& opposites)
{

  opposites.clear();

  // get a list of active SkinTools
  std::vector<SkinTool*> skins;
  std::deque<ElementBlock*> blocks;
  verde_app->mesh()->element_blocks(blocks);
  for(std::deque<ElementBlock*>::iterator b_iter = blocks.begin(); b_iter != blocks.end(); ++b_iter)
  {
    if( (*b_iter)->is_active() )
      skins.push_back( (*b_iter)->skin_tool() );
  }

  // go through each skin to find opposites
  for(std::vector<SkinTool*>::iterator iter = skins.begin(); iter != skins.end(); ++iter)
  {
    if(get_opposites.empty())
      break;

    SkinTool* skintool = *iter;
    // attempt the local quad lists matching first, since it is more likely we are looking for matches
    // in these lists
    std::deque<QuadElem*>::iterator q_iter = get_opposites.begin();
    for( ; q_iter != get_opposites.end(); )
    {
      if(get_opposites.empty())
        break;
      
      int which_list = (*q_iter)->node_id(0) % skintool->numberLocalQuadLists;
      bool found_match = false;
      if (!skintool->localQuadList || !skintool->localQuadList[which_list] )
      {
        ++q_iter;
        continue;
      }
      std::deque<QuadElem*>::iterator jter;
      for(jter = skintool->localQuadList[which_list]->begin(); jter != skintool->localQuadList[which_list]->end(); ++jter)
      {
        if( (*jter)->has_opposite_connectivity( *q_iter ) )
        {
          opposites.push_back(*jter);
          if(q_iter == (get_opposites.end()-1))
          {
            get_opposites.erase(q_iter);
            q_iter = get_opposites.end();
          }
          else
            q_iter = get_opposites.erase(q_iter);
          found_match = true;
          break;
        }
      }
      if(found_match==false)
        ++q_iter;
    }

  }

}


void SkinTool::get_opposite_tris(std::deque<TriElem*>& get_opposites, std::deque<TriElem*>& opposites)
{

  opposites.clear();

  // get a list of active SkinTools
  std::vector<SkinTool*> skins;
  std::deque<ElementBlock*> blocks;
  verde_app->mesh()->element_blocks(blocks);
  for(std::deque<ElementBlock*>::iterator b_iter = blocks.begin(); b_iter != blocks.end(); ++b_iter)
  {
    if( (*b_iter)->is_active() )
      skins.push_back( (*b_iter)->skin_tool() );
  }

  // go through each skin to find opposites
  for(std::vector<SkinTool*>::iterator iter = skins.begin(); iter != skins.end(); ++iter)
  {
    if(get_opposites.empty())
      break;

    SkinTool* skintool = *iter;
    // attempt the local quad lists matching first, since it is more likely we are looking for matches
    // in these lists
    std::deque<TriElem*>::iterator t_iter = get_opposites.begin();
    for( ; t_iter != get_opposites.end(); )
    {

      if(get_opposites.empty())
        break;

      TriElem* test_tri = *t_iter;
      
      int which_list = test_tri->node_id(0) % skintool->numberLocalTriLists;
      bool found_match = false;
      if (!skintool->localTriList || !skintool->localTriList[which_list] )
      {
        ++t_iter;
        continue;
      }

      int tri_key = test_tri->node_id(1) + test_tri->node_id(2);
      std::multimap<int,TriElem*>::iterator jter = skintool->localTriList[which_list]->find(tri_key);
      if(jter == skintool->localTriList[which_list]->end())
      {
        ++t_iter;
        continue;
      }

      for( ; jter != skintool->localTriList[which_list]->end() && tri_key == jter->first; ++jter)
      {
        TriElem* tri1 = jter->second;
        if( tri1->has_opposite_connectivity( test_tri ) )
        {
          opposites.push_back(tri1);
          if(t_iter == (get_opposites.end()-1))
          {
            get_opposites.erase(t_iter);
            t_iter = get_opposites.end();
          }
          else
          {
            t_iter = get_opposites.erase(t_iter);
          }
          found_match = true;
          break;
        }
      }
      if(found_match==false)
        ++t_iter;
    }

  }

}


