//-------------------------------------------------------------------------
// Filename      : EdgeTool.cc 
//
// Purpose       : Given a set of bounding polygons, finds the inferred
//                 model edges based on a minimum dihedral angle between
//                 surface polygons
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 02/04/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include <algorithm>

#include "EdgeTool.hpp"


#include "VerdeMessage.hpp"
#include "EdgeElem.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"
#include "SkinTool.hpp"
#include "VerdeVector.hpp"
#include "ElemRef.hpp"



int EdgeTool::NODES_PER_LIST = 8;


EdgeTool *EdgeTool::instance_ = NULL;

EdgeTool *EdgeTool::instance()
{
   if (!instance_)
      instance_ = new EdgeTool();
   return instance_;
}

void EdgeTool::delete_instance()
{
  if (instance_)
  {
    delete instance_;
    instance_ = NULL;
  }
}

EdgeTool::EdgeTool()
{
   minDihedralAngleCosine = cos(20.0*VERDE_PI/180.0);

   numberLists = 0;
   quadList = NULL;
   triList = NULL;
   edgeList = NULL;
   
}

void EdgeTool::initialize(int number_nodes)
{
  // delete any existing junk
  reset();
  delete_list_memory();

  // set numberLists from number_nodes

  numberLists = number_nodes/NODES_PER_LIST + 1;
}

void EdgeTool::initialize_quad_list()
{

   // allocate the quadList array

   quadList = new std::deque<QuadElem*>* [numberLists];

   // since the chance is small that any of the lists will never have
   // members, we allocate each list now.

   for (int i = 0; i < numberLists; i++)
   {
      quadList[i] = new std::deque<QuadElem*>;
   }

}

void EdgeTool::initialize_tri_list()
{
   // allocate the triList array

   triList = new std::deque<TriElem*>* [numberLists];

   // since the chance is small that any of the lists will never have
   // members, we allocate each list now.

   for (int i = 0; i < numberLists; i++)
   {
      triList[i] = new std::deque<TriElem*>;
   }
}

void EdgeTool::initialize_edge_list()
{
   // allocate the edgeList array

   edgeList = new std::deque<EdgeElem*>* [numberLists];

   // since the chance is small that any of the lists will never have
   // members, we allocate each list now.

   for (int i = 0; i < numberLists; i++)
   {
      edgeList[i] = new std::deque<EdgeElem*>;
   }
}

EdgeTool::~EdgeTool()
{
  reset();
  delete_list_memory();
  
}



EdgeElem* EdgeTool::find_match(EdgeElem *edge)
{
   // find the list index

   int index = edge->node_id(0) / NODES_PER_LIST;
   std::deque<EdgeElem*>::const_iterator edgelist_iterator;
   std::deque<EdgeElem*>::const_iterator edgelist_end = edgeList[index]->end();

   for(edgelist_iterator = edgeList[index]->begin();
       edgelist_iterator != edgelist_end;
       ++edgelist_iterator)
   {
      EdgeElem *test_edge = *edgelist_iterator;  //(*edgeList[index])[i];
      if (edge->node_id(0) == test_edge->node_id(0) &&
          edge->node_id(1) == test_edge->node_id(1))
      {
        // return the matched edge
        return test_edge;
      }
   }
   return NULL;
}



void EdgeTool::add_edge(EdgeElem *new_edge)
{
   // find the right list and append

   int index = new_edge->node_id(0) / NODES_PER_LIST;
   edgeList[index]->push_back(new_edge);
   
}

void EdgeTool::exterior_edges (std::deque<EdgeElem*> &exterior_list)
{
  std::copy(inferredEdgeList.begin(), inferredEdgeList.end(),
      std::back_inserter(exterior_list));
 
  std::copy(boundaryEdgeList.begin(), boundaryEdgeList.end(),
      std::back_inserter(exterior_list)); 

   // add in the edges directly from Beam elements

   SkinTool::exterior_edges(exterior_list);
}

void EdgeTool::coincident_edges (std::deque<EdgeElem*> &edge_list)
{
  std::copy(coincidentEdgeList.begin(), coincidentEdgeList.end(),
      std::back_inserter(edge_list));
}

void EdgeTool::non_manifold_edges (std::deque<EdgeElem*> &edge_list)
{
  std::copy(nonManifoldEdgeList.begin(), nonManifoldEdgeList.end(),
      std::back_inserter(edge_list));

  std::copy(nonManifoldBarEdgeList.begin(), nonManifoldBarEdgeList.end(),
      std::back_inserter(edge_list));
  
}

void EdgeTool::non_manifold_bar_edges (std::deque<EdgeElem*> &edge_list)
{
  std::copy(nonManifoldBarEdgeList.begin(), nonManifoldBarEdgeList.end(),
      std::back_inserter(edge_list));
}


void EdgeTool::print_summary()
{
   // print out results

  std::deque<EdgeElem*> edge_list;
   exterior_edges(edge_list);

   if (edge_list.size())
      PRINT_INFO("\nNumber of Model Edges: %d\n\n",edge_list.size());

   // print out each quad also

   for (unsigned int i=0; i<edge_list.size(); i++)
   {
      edge_list[i]->print();
   }
}

void EdgeTool::find_exterior_edges()
{
   // get exterior quads
   
   std::deque<QuadElem*> quad_list;
   SkinTool::exterior_quads(quad_list);
   std::deque<TriElem*> tri_list;
   SkinTool::exterior_tris(tri_list);

   // find inferred edges
   find_inferred_edges(quad_list, tri_list);
   
   // find boundary edges
   std::deque<EdgeElem*> edge_list;
   SkinTool::exterior_edges(edge_list);
   find_boundary_edges(quad_list, tri_list,edge_list);
}

 

void EdgeTool::find_inferred_edges(std::deque<QuadElem*> &quad_list,
    std::deque<TriElem*> &tri_list)
{

   // load into the internal hashed lists

   QuadElem *quad;
   TriElem *tri;

   int number_quads = quad_list.size();
   int number_tris = tri_list.size();
   
   if (!quad_list.empty())
   {
      initialize_quad_list();
   }
   
   std::deque<QuadElem*>::iterator qter;
   const std::deque<QuadElem*>::iterator qter_end = quad_list.end();

   for(qter = quad_list.begin(); qter != qter_end; ++qter)
      add_quad(*qter);
   
   if (!tri_list.empty())
   {
      initialize_tri_list();
   }
  
   std::deque<TriElem*>::iterator tter;
   const std::deque<TriElem*>::iterator tter_end = tri_list.end();
   for(tter = tri_list.begin(); tter != tter_end; ++tter) 
      add_tri(*tter);


   // now, process each quad face
   
   QuadElem *quad_neighbor;
   TriElem *tri_neighbor;
   EdgeElem *edge;
   VerdeBoolean delete_edge;
   
   for(qter = quad_list.begin(); qter != qter_end; ++qter)
   {
      quad = *qter;
      
      // remove the quad from the hashed lists
      
      remove_quad(quad);

      // now, look for potential neighbors for each edge
      
      for (int j = 0; j < 4; j++)
      {
         edge = quad->create_edge(j);
         delete_edge = VERDE_TRUE;

         // first, look for a quad on the edge
         
         if (number_quads)
         {
            quad_neighbor = find_quad_containing_edge(edge);
            if (quad_neighbor)
            {
               if (small_dihedral_angle(quad,quad_neighbor))
               {
                  inferredEdgeList.push_back(edge);
                  delete_edge = VERDE_FALSE;
               }
            }
         }

         // if no quad, look for a triangle
         
         if (number_tris && delete_edge)
         {   
            tri_neighbor = find_tri_containing_edge(edge);
            if (tri_neighbor)
            {
               if (small_dihedral_angle(quad,tri_neighbor))
               {
                  inferredEdgeList.push_back(edge);
                  delete_edge = VERDE_FALSE;
               }
            }
         }
         
         if (delete_edge)
            delete edge;
      }
   }

   // test any leftover tris against themselves

   for(tter = tri_list.begin(); tter != tter_end; ++tter)
   {
      tri = *tter;
      remove_tri(tri);
      for (int j = 0; j < 3; j++)
      {
         edge = tri->create_edge(j);
         delete_edge = VERDE_TRUE;
         tri_neighbor = find_tri_containing_edge(edge);
         if (tri_neighbor)
         {
            if (small_dihedral_angle(tri,tri_neighbor))
            {
               inferredEdgeList.push_back(edge);
               delete_edge = VERDE_FALSE;
            }
         }
         if (delete_edge)
            delete edge;
      }
   }

   // Go through the inferred edge list to clean up any
   // duplicates.  Duplicates can occur when we have 
   // connectivity problems on the surface of the mesh.
   

   if(!inferredEdgeList.empty())
   {
     std::sort(inferredEdgeList.begin(), 
         inferredEdgeList.end(), nodes_less());
     std::deque<EdgeElem*>::iterator iter;

     for(iter = inferredEdgeList.begin()+1; iter!= inferredEdgeList.end(); )
     {
       if(( (*(iter-1))->node_id(1) == (*iter)->node_id(1) ) &&
           ( (*(iter-1))->node_id(0) == (*iter)->node_id(0) ) )
       {
         delete *iter; // Must delete the extra pointer before it is erased.
         iter = inferredEdgeList.erase(iter);
       }
       else
         ++iter;
     }
   }   

   // Clean up lists
   int i;

   for( i=0; i<numberLists; i++)
   {
     if(quadList && quadList[i]) 
       delete quadList[i];
     if(triList && triList[i]) 
       delete triList[i];
   }

   delete [] quadList;
   delete [] triList;

   quadList = 0;
   triList  = 0;
}



VerdeBoolean EdgeTool::small_dihedral_angle(QuadElem *quad, TriElem *tri)
{
   // compare normals to get angle.  We assume that the surface quads
   // which we test here will be approximately planar

   VerdeVector quad_norm = quad->normal();

   VerdeVector tri_norm = tri->normal();

   double cosine = quad_norm % tri_norm;

   if (cosine < minDihedralAngleCosine)
      return VERDE_TRUE;
   

   return VERDE_FALSE;
}

VerdeBoolean EdgeTool::small_dihedral_angle(QuadElem *quad1,
                                            QuadElem *quad2)
{
   // compare normals to get angle.  We assume that the surface quads
   // which we test here will be approximately planar

   VerdeVector norm1 = quad1->normal();
   VerdeVector norm2 = quad2->normal();

   double cosine = norm1 % norm2;

   if (cosine < minDihedralAngleCosine)
      return VERDE_TRUE;
   

   return VERDE_FALSE;
}

VerdeBoolean EdgeTool::small_dihedral_angle(TriElem *tri1, TriElem *tri2)
{
   // compare normals to get angle. 

   VerdeVector norm1 = tri1->normal();
   VerdeVector norm2 = tri2->normal();

   double cosine = norm1 % norm2;

   if (cosine < minDihedralAngleCosine)
      return VERDE_TRUE;
   

   return VERDE_FALSE;
}

QuadElem* EdgeTool::find_quad_containing_edge(EdgeElem *edge)
{
   
   // look up the proper list of quads, and test

   int min_id = VERDE_MIN(edge->node_id(0),edge->node_id(1));
   int index = min_id / NODES_PER_LIST;
   
   std::deque<QuadElem*>::iterator iter = (*quadList[index]).begin();
   const std::deque<QuadElem*>::iterator iter_end = (*quadList[index]).end();
   for( ; iter != iter_end; ++iter)
   {
     if ((*iter)->contains(edge))
       return *iter;
   }

   return NULL;
}

TriElem* EdgeTool::find_tri_containing_edge(EdgeElem *edge)
{
   
   // look up the proper list of tris, and test

   int min_id = VERDE_MIN(edge->node_id(0),edge->node_id(1));
   int index = min_id / NODES_PER_LIST;
   
   std::deque<TriElem*>::iterator iter = (*triList[index]).begin();
   const std::deque<TriElem*>::iterator iter_end = (*triList[index]).end();

   for( ; iter != iter_end; ++iter )
   {
     if ((*iter)->contains(edge))
       return *iter;
   }

   return NULL;
}



void EdgeTool::find_coincident_edges()
{

  // go through the current inferredEdgeList, see if any two edges in the list
  // have endpoints in the same locations.

  std::deque<EdgeElem*>::iterator iter = inferredEdgeList.begin();
  const std::deque<EdgeElem*>::iterator iter_end = inferredEdgeList.end();
  std::deque<EdgeElem*>::iterator jter;

  for( ; iter != iter_end; ++iter)
  {
    for(jter = iter+1; jter != iter_end; ++jter)
    {
      EdgeCondition condition = coincident_edge(*iter,*jter);
      
      if (condition == COINCIDENT)
      {
        coincidentEdgeList.push_back(*iter);
      }
    }
  }
}

EdgeCondition EdgeTool::coincident_edge(EdgeElem *edge0,
                                        EdgeElem *edge1)
{
   // first, test to see if edges are IDENTICAL

   if (edge0->node_id(0) == edge1->node_id(0))
   {
      if (edge0->node_id(1) == edge1->node_id(1))
      {
         return IDENTICAL;
      }
   }
   else if (edge1->node_id(0) == edge1->node_id(1))
   {
      if (edge1->node_id(1) == edge1->node_id(0))
      {
         return IDENTICAL;
      }
   }
   
   // test to see if the coordinates of the two end points are in the
   // same location to a small tolerance.  We will attempt to calculate the
   // tolerance as a ratio of the edge length of the edges

   const double TOL_RATIO = 1E-06;

   MeshContainer *node_container = ElemRef::node_container();

   VerdeVector edge0_0 = node_container->coordinates(edge0->node_id(0));
   VerdeVector edge0_1 = node_container->coordinates(edge0->node_id(1));
   VerdeVector edge1_0 = node_container->coordinates(edge1->node_id(0));
   VerdeVector edge1_1 = node_container->coordinates(edge1->node_id(1));
   

   VerdeVector diff = edge0_0 - edge0_1;

   double edge_length = diff.length();

   // test endpoint 0 of edge 0 against endpoint 0 of edge 1
   diff = edge0_0 - edge1_0;
   double length = diff.length();

   if (length < edge_length * TOL_RATIO)
   {
      // test the second endpoints for match
      diff = edge0_1 - edge1_1;
      length = diff.length();
      if (length < edge_length * TOL_RATIO)
      {
         return COINCIDENT;
      }
   }

   // test endpoint 0 of edge 0 against endpoint 1 of edge 1
   diff = edge0_0 - edge1_1;
   length = diff.length();

   if (length < edge_length * TOL_RATIO)
   {
      // test the second endpoints for match
      diff = edge0_1 - edge1_0;
      length = diff.length();
      if (length < edge_length * TOL_RATIO)
      {
         return COINCIDENT;
      }
   }

   return NO_MATCH;
   
}

void EdgeTool::add_quad(QuadElem *new_quad)
{
   // add the quad to lists based on nodes,
   // up to once for each node

   int index[4];

   // add for node 0
   
   index[0] = new_quad->node_id(0) / NODES_PER_LIST;
   quadList[index[0]]->push_back(new_quad);

   // add for node 1
   
   index[1] = new_quad->node_id(1) / NODES_PER_LIST;
   if (index[1] != index[0])
   {
      quadList[index[1]]->push_back(new_quad);
   }
   
   // add for node 2
   
   index[2] = new_quad->node_id(2) / NODES_PER_LIST;
   if (index[2] != index[0] && index[2] != index[1])
   {
      quadList[index[2]]->push_back(new_quad);
   }


   // add for node 3
   
   index[3] = new_quad->node_id(3) / NODES_PER_LIST;
   if (index[3] != index[0] && index[3] != index[1] && index[3] != index[2])
   {
      quadList[index[3]]->push_back(new_quad);
   }
   
}

void EdgeTool::add_tri(TriElem *new_tri)
{
   // add the tri to lists based on nodes,
   // up to once for each node

   int index[3];

   // add for node 0
   
   index[0] = new_tri->node_id(0) / NODES_PER_LIST;
   triList[index[0]]->push_back(new_tri);

   // add for node 1
   
   index[1] = new_tri->node_id(1) / NODES_PER_LIST;
   if (index[1] != index[0])
   {
      triList[index[1]]->push_back(new_tri);
   }
   
   // add for node 2
   
   index[2] = new_tri->node_id(2) / NODES_PER_LIST;
   if (index[2] != index[0] && index[2] != index[1])
   {
      triList[index[2]]->push_back(new_tri);
   }   
}


void EdgeTool::remove_quad(QuadElem *new_quad)
{
   // remove the quad from all potential lists

   int index[4];
   std::deque<QuadElem*>::iterator iter;

   // remove for node 0
   
   index[0] = new_quad->node_id(0) / NODES_PER_LIST;
   iter = std::find(quadList[index[0]]->begin(), quadList[index[0]]->end(), new_quad);
   if( iter != quadList[index[0]]->end())
     quadList[index[0]]->erase(iter);

   // remove for node 1
   
   index[1] = new_quad->node_id(1) / NODES_PER_LIST;
   if (index[1] != index[0])
   {
     iter = std::find(quadList[index[1]]->begin(), quadList[index[1]]->end(), new_quad);
     if(iter != quadList[index[1]]->end())
       quadList[index[1]]->erase(iter);
   }
   
   // remove for node 2
   
   index[2] = new_quad->node_id(2) / NODES_PER_LIST;
   if (index[2] != index[0] && index[2] != index[1])
   {
     iter = std::find(quadList[index[2]]->begin(), quadList[index[2]]->end(), new_quad);
     if(iter != quadList[index[2]]->end())
       quadList[index[2]]->erase(iter);
   }


   // remove for node 3
   
   index[3] = new_quad->node_id(3) / NODES_PER_LIST;
   if (index[3] != index[0] && index[3] != index[1] && index[3] != index[2])
   {
     iter = std::find(quadList[index[3]]->begin(), quadList[index[3]]->end(), new_quad);
     if(iter != quadList[index[3]]->end())
       quadList[index[3]]->erase(iter);
   }
   
}


void EdgeTool::remove_tri(TriElem *new_tri)
{
   // remove the tri from all potential lists

   int index[3];
   std::deque<TriElem*>::iterator iter;

   // remove for node 0
   
   index[0] = new_tri->node_id(0) / NODES_PER_LIST;
   iter = std::find(triList[index[0]]->begin(), triList[index[0]]->end(), new_tri);
   if(iter != triList[index[0]]->end())
     triList[index[0]]->erase(iter);

   // remove for node 1
   
   index[1] = new_tri->node_id(1) / NODES_PER_LIST;
   if (index[1] != index[0])
   {
     iter = std::find(triList[index[1]]->begin(), triList[index[1]]->end(), new_tri);
     if(iter != triList[index[1]]->end())
       triList[index[1]]->erase(iter);
   }
   
   // remove for node 2
   
   index[2] = new_tri->node_id(2) / NODES_PER_LIST;
   if (index[2] != index[0] && index[2] != index[1])
   {
     iter = std::find(triList[index[2]]->begin(), triList[index[2]]->end(), new_tri);
     if(iter != triList[index[2]]->end())
       triList[index[2]]->erase(iter);
   }   
}

/*!
Resets all lists contained in the EdgeTool.  Deletes any edge elements
currently defined.  Does not delete the lists themselves.
*/
void EdgeTool::reset()
{
  // just reset the boundary and non-manifold edge lists, they are acutally
  // deleted below in the edgeLists

  boundaryEdgeList.clear();
  nonManifoldEdgeList.clear();
  nonManifoldBarEdgeList.clear();
  
  // delete the inferred edges while cleaning out the list

  while(!inferredEdgeList.empty())
  {
    delete inferredEdgeList.back();
    inferredEdgeList.pop_back();
  }
  inferredEdgeList.resize(0);

  // reset the edgeLists

  for(int k = 0; k < numberLists; k++)
  {
    if (edgeList && edgeList[k])
    {
      while(!edgeList[k]->empty())
      {
        delete edgeList[k]->back();
        edgeList[k]->pop_back();
      }
      edgeList[k]->resize(0);
    }
  }

   // Clean up edge list
   int i;

   for( i=0; i<numberLists; i++)
   {
     if(edgeList && edgeList[i]) 
       delete edgeList[i];
   }

   delete [] edgeList;

   edgeList = 0;
}


void EdgeTool::reset_calculated()
{
  coincidentEdgeList.clear();

  // these two guys below are not calculated each time
  // a mesh is analyzed.  This probably needs to change
  // because between analysis, we can say that we don't
  // want toplogy calculations done, but these still show up

  //  nonManifoldEdgeList.clear();
  //  nonManifoldBarEdgeList.clear();
}


/*!
Deletes memory for all lists in the EdgeTool. Does not delete any quads
or tris in the lists, this is taken care of elsewhere. does delete the
*/
void EdgeTool::delete_list_memory()
{  

  int i;

  for( i=0; i<numberLists; i++)
  {
    if(quadList && quadList[i]) 
      delete quadList[i];
    if(triList && triList[i]) 
      delete triList[i];
    if(edgeList && edgeList[i]) 
      delete edgeList[i];
  }

  if(quadList) delete [] quadList;
  if(triList) delete [] triList;
  if (edgeList) delete [] edgeList;

  numberLists = 0;
  quadList = NULL;
  triList = NULL;
  edgeList = NULL;

}

void EdgeTool::find_boundary_edges(std::deque<QuadElem*> &quad_list,
    std::deque<TriElem*> &tri_list,
    std::deque<EdgeElem*> &edge_list)
{
  // Search for edges in the input polygon lists which are attached
  // to a single polygon

  // process all edges in the quad_list

  initialize_edge_list();
 
  std::deque<QuadElem*>::iterator qter;
  const std::deque<QuadElem*>::iterator qter_end = quad_list.end();

  for( qter = quad_list.begin(); qter != qter_end; ++qter)
    add_quad_edges(*qter);
  
  // repeat for tri list
  std::deque<TriElem*>::iterator tter;
  const std::deque<TriElem*>::iterator tter_end = tri_list.end();

  for(tter = tri_list.begin(); tter != tter_end; ++tter)
    add_tri_edges(*tter);

  
  // edge list consists of bar elements.  See if any of these are in the list already
  // any that are must be non-manifold bar elements

  std::deque<EdgeElem*>::iterator eter;
  const std::deque<EdgeElem*>::iterator eter_end = edge_list.end();

  EdgeElem *match_edge;
  for(eter = edge_list.begin(); eter != eter_end; ++eter)
  {
    if ((match_edge = find_match(*eter)) != NULL )
    {
      // if matched, put in list
      nonManifoldBarEdgeList.push_back(*eter);
    }
  }

  // load the lists from the edgeList.  If not marked, it is a boundary (free) edge
  // if marked more than twice, it is non-manifold


  for(int k=0; k<numberLists; k++)
  {
    std::deque<EdgeElem*>::iterator eter2 = edgeList[k]->begin();
    std::deque<EdgeElem*>::iterator eter2_end = edgeList[k]->end();

    for( ; eter2 != eter2_end; ++eter2 )
    {
      EdgeElem *edge = *eter2;
      int num_marked = edge->times_marked();
      if (num_marked == 0)
      {
        boundaryEdgeList.push_back(edge);
      }
      else if (num_marked > 1 && verde_app->get_topology() == VERDE_TRUE)
      {
        nonManifoldEdgeList.push_back(edge);
      }
    }
  }
}


void EdgeTool::add_quad_edges(QuadElem *quad)
{
   // build the four edges and add

   EdgeElem *match_edge;
   for (int i = 0; i < 4; i++)
   {
      EdgeElem *edge = quad->create_edge(i);
      if ((match_edge = find_match(edge)) != NULL )
      {
         // mark the matched edge, delete this edge
         match_edge->mark();
         delete edge;
      }
      else
      {
         add_edge(edge);
      }
   }
}

void EdgeTool::add_tri_edges(TriElem *tri)
{
   // build the three edges and add

   EdgeElem *match_edge;
   for (int i = 0; i < 3; i++)
   {
      EdgeElem *edge = tri->create_edge(i);
      if ((match_edge = find_match(edge)) != NULL )
      {
         // mark the matched edge, delete this edge
         match_edge->mark();
         delete edge;
      }
      else
      {
         add_edge(edge);
      }
   }
}


