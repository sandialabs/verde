//-------------------------------------------------------------------------
// Filename      : EulerTool.cc 
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


#include "EulerTool.hpp"

#include "EdgeTool.hpp"
#include "EdgeElem.hpp"
#include "TetRef.hpp"
#include "PyramidRef.hpp"
#include "WedgeRef.hpp"
#include "SkinTool.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"
#include "QuadRef.hpp"
#include "TriRef.hpp"


int EulerTool::NODES_PER_LIST = 8;


EulerTool *EulerTool::instance_ = NULL;

EulerTool *EulerTool::instance()
{
   if (!instance_)
      instance_ = new EulerTool();
   return instance_;
}

EulerTool::EulerTool()
{
   edgeList = NULL;
   numberEdges = 0;
}

void EulerTool::initialize(int number_nodes)
{
   numberLists = number_nodes/NODES_PER_LIST + 1;   
}

void EulerTool::initialize_edge_list()
{
  // this routine will
  // 1.  allocate memory if it has not been done, or
  // 2.  reset existing edge_arrays to recalculate
   
  // allocate the edgeList array

  if (edgeList == NULL)
  {
    
    edgeList = new std::deque<EdgeElem*> * [numberLists];
    
    // since the chance is small that any of the lists will never have
    // members, we allocate each list now.

    for (int i = 0; i < numberLists; i++)
    {
      edgeList[i] = new std::deque<EdgeElem*>;
    }
  }
  
  else
  {
    // cleanout lists
    for (int i = 0; i < numberLists; i++)
    {
      edgeList[i]->clear();
    }
    
  }
}


EulerTool::~EulerTool()
{
   reset();   
}

void EulerTool::reset()
{
  int i;

  if(edgeList)
  {
    for(i=0; i < numberLists; i++)
    {
      if(edgeList[i])
      {
        for(std::deque<EdgeElem*>::iterator iter = edgeList[i]->begin();
            iter != edgeList[i]->end(); ++iter)
          delete *iter;
        edgeList[i]->clear();
      }
    }
  }

  if (edgeList != NULL)
  {
    for (i = 0; i < numberLists; i++)
    {
      delete edgeList[i];
    }
    delete [] edgeList;
  }

  edgeList = NULL;
}


void EulerTool::add_edge(EdgeElem *new_edge)
{
   // find the right list and append

   int index = new_edge->node_id(0) / NODES_PER_LIST;
   edgeList[index]->push_back(new_edge);
   
}
VerdeStatus EulerTool::find_match(EdgeElem *edge)
{
   // find the list index

   int index = edge->node_id(0) / NODES_PER_LIST;
   std::deque<EdgeElem*>::iterator iter = edgeList[index]->begin();
   std::deque<EdgeElem*>::iterator jter = edgeList[index]->end();

   for (; iter != jter; ++iter )
   {
      EdgeElem *test_edge = *iter;
      if (edge->node_id(0) == test_edge->node_id(0) &&
          edge->node_id(1) == test_edge->node_id(1))
      {
         // mark the edge for tracking
         test_edge->mark();
         return VERDE_SUCCESS;
      }
   }
   return VERDE_FAILURE;
}

int number_processed = 0;

void EulerTool::add_ref(ElemRef &new_ref)
{
   // initialize edgeList if necessary

   if (edgeList == NULL)
      initialize_edge_list();

   static std::deque<EdgeElem*> edge_list;
   edge_list.clear();
   new_ref.edge_list(edge_list);
   
   // build the edges and add

   for (unsigned int i = 0; i < edge_list.size(); i++)
   {
      EdgeElem *edge = edge_list[i];
      
      if (find_match(edge) == VERDE_SUCCESS)
      {
         delete edge;
      }
      else
      {
         add_edge(edge);
      }
      number_processed++;
   }
}

void EulerTool::add_quad(QuadElem *quad)
{
   // initialize edgeList if necessary

   if (edgeList == NULL)
      initialize_edge_list();
   
   // build the four edges and add

   for (int i = 0; i < 4; i++)
   {
      EdgeElem *edge = quad->create_edge(i);
      if (find_match(edge) == VERDE_SUCCESS)
      {
         delete edge;
      }
      else
      {
         add_edge(edge);
      }
   }
}

void EulerTool::add_tri(TriElem *tri)
{
   // initialize edgeList if necessary

   if (edgeList == NULL)
      initialize_edge_list();
   
   // build the three edges and add

   for (int i = 0; i < 3; i++)
   {
      EdgeElem *edge = tri->create_edge(i);
      if (find_match(edge) == VERDE_SUCCESS)
      {
         delete edge;
      }
      else
      {
         add_edge(edge);
      }
   }
}


VerdeBoolean EulerTool::calculate_euler(int number_hexes,
                                        int number_tets,
                                        int number_pyramids,
                                        int number_wedges,
                                        int number_knives)
{
   VerdeBoolean success = VERDE_TRUE;

   // see if there are 3d elements to process
   // for now, either process 2d or 3d, but not both
   
   int number_3d_elements = number_hexes + number_tets
       + number_pyramids + number_wedges + number_knives;
   int number_2d_elements = (!number_3d_elements);
   
   
   // gather quad and tri information from skin tool
   // for 3d, these are exterior quads and tris
   // for 2d, these are all quads and tris 

   std::deque<QuadElem*> exterior_quad_list;
   SkinTool::exterior_quads(exterior_quad_list);
   std::deque<TriElem*> exterior_tri_list;
   SkinTool::exterior_tris(exterior_tri_list);

   // gather total edge and node numbers
   // nodes were marked in ElementBlock::process

   int total_edges = number_edges();
   int total_nodes = ElemRef::node_container()->
       count_marked_nodes();

  
   if (number_2d_elements)
   {
      // calculate 2D euler for the surface(s)

      int euler_exterior = exterior_quad_list.size()
          + exterior_tri_list.size() - total_edges + total_nodes;
   

      // calculate remainer
      // unmarked edges will contain the number of boundary edges,
      // that is, the number of unmatched edges in the surface(s)

      int number_boundary_edges = number_unmarked_edges();
      int number_surface_elements = exterior_quad_list.size() +
          exterior_tri_list.size();
      if (number_surface_elements > 0)
      {
      
         int remainder_exterior = 2 * total_edges -
             4 * exterior_quad_list.size() -
             3 * exterior_tri_list.size() - number_boundary_edges;

         if(remainder_exterior != 0)
         {
            PRINT_INFO("   WARNING: inconsistent edge to quad/tri count "
                       "in exterior mesh...\n\n");
            success = VERDE_FALSE;
         }
         //else
         //   PRINT_INFO("   Edge to Quad/Tri counts appear correct...\n\n");
      }
      
/*
      PRINT_INFO("\nExterior Euler Calculation:\n"
                 "   Total Quads:    %d\n"
                 "   Total Tris:     %d\n"
                 "   Total Edges:    %d\n"
                 "   Total Nodes:    %d\n"
                 "   Euler Number:   %d\n"
                 "   Boundary Edges: %d\n"
                 "   Remainder:      %d\n\n",
                 exterior_quad_list.size(), exterior_tri_list.size(),
                 total_edges, total_nodes,
                 euler_exterior, number_boundary_edges,remainder_exterior);
*/

      predict_2d_topology(euler_exterior, number_boundary_edges);
      
   }
   
   if (number_3d_elements)
   {
      // calculate the exterior euler number for the 3d case
      // for this, we need the number of exterior edges and nodes


      ElemRef::node_container()->clear_marked_nodes();
      initialize_edge_list();
      QuadElem *quad;
      TriElem *tri;
      unsigned int i;
      for ( i=0; i<exterior_quad_list.size(); i++)
      {
         quad = exterior_quad_list[i];
         quad->mark_nodes();
         add_quad(quad);
      }
   
      for (i=0; i<exterior_tri_list.size(); i++)
      {
         tri = exterior_tri_list[i];
         tri->mark_nodes();
         add_tri(tri);
      }
   
      int number_exterior_edges = number_edges();
      int number_exterior_nodes = ElemRef::node_container()->
          count_marked_nodes();

   
      // calculate the exterior euler number

      int euler_exterior = exterior_quad_list.size() +
          exterior_tri_list.size() -
          number_exterior_edges +
          number_exterior_nodes;
   


      int remainder_exterior = 2 * number_exterior_edges -
          4 * exterior_quad_list.size() -
          3 * exterior_tri_list.size();

      if(remainder_exterior != 0)
      {
         PRINT_INFO("   WARNING: inconsistent edge to quad/tri count "
                    "in exterior mesh...\n");
         success = VERDE_FALSE;
      }
      //else
      //   PRINT_INFO("   Edge to Quad/Tri counts appear correct...\n");
   
/*
      PRINT_INFO("\nExterior Euler Calculation:\n"
                 "   Total Quads:    %d\n"
                 "   Total Tris:     %d\n"
                 "   Total Edges:    %d\n"
                 "   Total Nodes:    %d\n"
                 "   Euler Number:   %d\n"
                 "   Remainder:      %d\n\n",
                 exterior_quad_list.size(), exterior_tri_list.size(),
                 number_exterior_edges, number_exterior_nodes,
                 euler_exterior,remainder_exterior);
 */
      // now, calculate 3d euler number

      // gather interior numbers
      
      int number_interior_quads = SkinTool::number_interior_quads();
      int number_interior_tris = SkinTool:: number_interior_tris();

      int total_quads = number_interior_quads + exterior_quad_list.size();
      int total_tris = number_interior_tris + exterior_tri_list.size();

      // calculate 3d euler
      
      //int euler_3d = number_3d_elements - total_quads - total_tris
      //    + total_edges - total_nodes;
      
/*
    PRINT_INFO("Volume Euler Calculation:\n"
               "   Total Elements: %d\n"
              "   Total Quads:    %d\n"
              "   Total Tris:     %d\n"
              "   Total Edges:    %d\n"
              "   Total Nodes:    %d\n"
              "   Euler Number:   %d\n\n",
              number_elements, total_quads, total_tris, total_edges,
              total_nodes, euler_3d);
*/ 
      int remainder_interior = 2 * (total_quads + total_tris) -
          exterior_quad_list.size() -
          exterior_tri_list.size() -
          6 * number_hexes -
          4 * number_tets -
          5 * number_pyramids -
          5 * number_wedges -
          5 * number_knives;
   
      if(remainder_interior != 0)
      {
         PRINT_INFO("   WARNING: inconsistent quad to element count "
                    "in mesh...\n\n");
         success = VERDE_FALSE;
      }
   
      //else
      //{
      //   PRINT_INFO("   Quad/Tri to element counts appear correct...\n\n");
      //}


      if (success)
         PRINT_INFO("   Mesh appears fully conformal\n\n");
      else
         PRINT_INFO("   Mesh is not fully conformal\n\n");

      predict_3d_topology(euler_exterior);   
   }
   return success;  
}

              

void EulerTool::print_summary()
{
   // print out results

   //PRINT_INFO("\nNumber of Edges: %d\n\n",
   //           number_edges());
   
   // PRINT_INFO("\nNumber of Edges Processed: %d\n\n",
   //           number_processed);

   // calculate_euler();

}

int EulerTool::number_edges()
{
   int number = 0;

   if (edgeList)
   {  
      for (int i = 0; i < numberLists; i++)
      {
         number += edgeList[i]->size();
      }
   }
   return number;
   
}

int EulerTool::number_unmarked_edges()
{
  int number = 0;
  
  if (edgeList)
  {  
    for (int i = 0; i < numberLists; i++)
    {
      for(std::deque<EdgeElem*>::iterator iter = edgeList[i]->begin();
          iter != edgeList[i]->end(); ++iter)
      {
        if(!((*iter)->is_marked()))
          number++;
      }
    }
  }
  return number;
  
}

void EulerTool::predict_2d_topology(int euler,
                                    int number_boundary_edges)
{
   // if the number of boundary edges is zero, this is a
   // closed surface, and we can guess at the topology.
   // if we were to subdivide the volume into voids, we could
   // guess even better.  For now, look semi-smart
   //
   // with boundary edges, we know we have an open surface, and we
   // guess that there is one with possible holes.
   // We have the edge lists, we can in the future find out exactly
   // how many loops there are and give a very good guess...
  
    
   if (number_boundary_edges == 0)
   {
      int number_surfaces = euler/2;

      // zero is a probable torus

      if (euler == 0)
      {
         PRINT_INFO("   Probable Topology: Torus (Closed surface "
                    "with through hole)\n");
         PRINT_INFO("   Possible Topologies: N Closed surfaces with "
                    "N holes\n");
      }
      else
      {
         // guess that there are no holes
         
         PRINT_INFO("   Probable Topology: %d closed surface(s))\n",
                    number_surfaces);
         PRINT_INFO("   Possible Topologies: N Closed surfaces with "
                    "N%+d holes\n", -number_surfaces);
      }

   }
   else
   {
      int number_holes = 1 - euler;
      
      // these are probable open surface(s)

      if (euler > 0)
      {
         // guess this is an open surface

         PRINT_INFO("   Probable Topology:  %d Open Surface(s)\n",
                    euler);
      }
      else
      {
         // guess a single surface with multiple holes
         
         PRINT_INFO("   Probable Topology:  Open Surface with %d hole(s)\n",
                    number_holes);
      }

      // other possibilities

      if (number_holes == 1)
      {
         PRINT_INFO("   Possible Topologies: N Open surfaces with "
                    "N hole(s)\n", number_holes-1);
      }
      else
      {
         PRINT_INFO("   Possible Topologies: N Open surfaces with "
                    "N%+d hole(s)\n", number_holes-1);
      }
         
   }
}

void EulerTool::predict_3d_topology(int euler)
{
   // From the euler, we know the ratio of volumes to holes
   // for now, make an intelligent guess.
   // if we were to subdivide the volume into voids, we could
   // guess even better.  For now, look semi-smart


   int number_surfaces = euler/2;

   // zero is a probable torus

   if (euler == 0)
   {
      PRINT_INFO("   Probable Topology: Torus (Volume "
                 "with through hole)\n");
      PRINT_INFO("   Possible Topologies: N Volumes with "
                 "N holes\n");
   }
   else if (euler > 0)
   {
      // guess that there are no holes
         
      PRINT_INFO("   Probable Topology: %d Volume(s))\n",
                 number_surfaces);
      PRINT_INFO("   Possible Topologies: N Volumes with "
                 "N-%d holes\n", number_surfaces);
   }
   else if (euler < 0)
   {
      // guess single volume with n holes
     int number_holes = -number_surfaces + 1;
      PRINT_INFO("   Probable Topology: 1 Volume with %d holes\n",
                 number_holes);
      PRINT_INFO("   Possible Topologies: N Volumes with "
                 "N-%d holes\n", number_holes);
   }
}

         
         
      
