//-------------------------------------------------------------------------
// Filename      : Mesh.hpp 
//
// Purpose       : This class contains the global mesh information and
//                 controls the MeshBlocks for the mesh.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------


#include <string.h>

#include "Mesh.hpp"
#include "ElementBlock.hpp"
#include "NodeBC.hpp"
#include "ElementBC.hpp"
#include "VerdeMessage.hpp"
#include "SkinTool.hpp"
#include "EulerTool.hpp"
#include "HexMetric.hpp"
#include "TetMetric.hpp"
#include "PyramidMetric.hpp"
#include "WedgeMetric.hpp"
#include "KnifeMetric.hpp"
#include "QuadMetric.hpp"
#include "TriMetric.hpp"
#include "EdgeMetric.hpp"
#include "EdgeTool.hpp"
#include "NodeRef.hpp"
#include "ErrorTool.hpp"
#include "VerdeApp.hpp"
#include "MDBInterface.hpp"

#include "exodusII.h"
//Mesh *Mesh::instance_ = NULL;

// Returns the singleton instance of the object.
// Creates the object if it does not yet exist

/*
Mesh* Mesh::instance()
{
   if (instance_ == NULL)
   {
      instance_ = new Mesh();
      if (!instance_ )
      {
         PRINT_ERROR(" *** Unable to create Mesh object ***\n");
         return NULL;
      }
   }
   return instance_;
}
*/
Mesh::Mesh()
{
   exodusFile = -1;

   numberDimensions = 0;
   numberNodes = 0;
   numberElements = 0;
   numberHexes = 0;
   numberTets = 0;
   numberPyramids = 0;
   numberWedges = 0;
   numberKnives = 0;
   numberQuads = 0;
   numberTris = 0;
   numberEdges = 0;
   numberElementBlocks = 0;   
   numberPreviousNodes = 0;

   meshVolume = 0.0;
   meshArea = 0.0;

   nodeContainer = new MeshContainer();

}

 
void Mesh::reset ()
{
   // close exodusFile if open
   if (exodusFile != -1)
   {
      ex_close(exodusFile);
      exodusFile = -1;
   }

   //- make sure block information is deleted
   ElementBlock* block;
   unsigned int i;
   for(i=0; i<elementBlocks.size(); i++)
   {
     block = elementBlocks[i];
     delete block;
   }
   elementBlocks.clear();
   
   //- make sure nodeBC information is deleted
   NodeBC *bc;
   for(i=0; i<nodeBCs.size(); i++)
   {
     bc = nodeBCs[i];
     delete bc;
   }
   nodeBCs.clear();
   
   
   //- make sure nodeBC information is deleted
   ElementBC *Ebc;
   for(i=0; i<elementBCs.size(); i++)
   {
     Ebc = elementBCs[i];
     delete Ebc;
   }
   elementBCs.clear();

   //- make sure qaRecord information is deleted
   char *qa_string;
   for(i=0; i<qaRecords.size(); i++)
   {
     qa_string = qaRecords[i];
     delete [] qa_string;
   }
   qaRecords.clear();

   delete nodeContainer;

   // need to reset the Mesh Object

   verde_app->MDB()->reset();

   SkinTool::clear_skin();
}


Mesh::~Mesh()
{
  // clean up and exit

  reset();
}


VerdeStatus Mesh::open_exodus_file ( const char *exodus_file_name,
    std::deque<int> *active_block_id_list)
{
  // read header information
  if(!read_exodus_header(exodus_file_name, active_block_id_list))
    return VERDE_FAILURE;

  // read qa information
  read_qa_information(exodusFile, qaRecords);


  // read block information
  load_active_element_blocks();


  PRINT_INFO("\nProcessing Skin...\n");

  EdgeTool::instance()->initialize(numberNodes);
  EdgeTool::instance()->find_exterior_edges();

  return VERDE_SUCCESS;

}

VerdeStatus Mesh::read_exodus_header  ( const char *exodus_file_name,
    std::deque<int> *active_block_id_list)
{

  // open exodus file
   
  float exodus_version;
  int CPU_WORD_SIZE = sizeof(double);  // With ExodusII version 2, all floats
  int IO_WORD_SIZE = sizeof(double);               // should be changed to doubles

  exodusFile = ex_open ( exodus_file_name, EX_READ, &CPU_WORD_SIZE,
                         &IO_WORD_SIZE, &exodus_version );
 
  if ( exodusFile == -1 )
  {
    PRINT_INFO("\nVerde:: problem opening genesis file %s\n",
               exodus_file_name);
    return VERDE_FAILURE;
  }

  // read in initial variables
   
  numberPreviousNodes = numberNodes;
  int numberDimensions_loading = 0;
  int numberNodes_loading = 0;
  int numberElements_loading = 0;
  int numberElementBlocks_loading = 0; 
  int prev_numberElements = numberElements; 

  char title[MAX_LINE_LENGTH+1];
  int number_node_sets;
  int number_side_sets;
  int error = ex_get_init ( exodusFile,
                            title,
                            &numberDimensions_loading,
                            &numberNodes_loading,
                            &numberElements_loading,
                            &numberElementBlocks_loading,
                            &number_node_sets,
                            &number_side_sets );
  if ( error == -1 )
  {
    PRINT_INFO("\nVerde:: error reading exodus init variables...\n\n");
    return VERDE_FAILURE;
  }
  
 
  if( verde_app->should_add_mesh() )
  { 
    numberDimensions    = numberDimensions_loading;
    numberNodes         += numberNodes_loading;
    numberElements      += numberElements_loading;
    numberElementBlocks += numberElementBlocks_loading;
  }
  else
  {
    numberDimensions    = numberDimensions_loading;
    numberNodes         = numberNodes_loading;
    numberElements      = numberElements_loading;
    numberElementBlocks = numberElementBlocks_loading;
  }
 
  // read the nodes into memory
  VerdeStatus success = node_container()->load_nodes();

  if (success == VERDE_FAILURE)
    return success;

  // read the element number map for original element ids
	
  success = ElementBlock::read_element_number_map(prev_numberElements, 
                                                  numberElements_loading, 
                                                  exodus_file() );
  if(success == VERDE_FAILURE)
  {
    PRINT_INFO("Verde:: error reading exodus element number map...\n");
    return VERDE_FAILURE;
  }
  

  // get the element block ids
  int *block_ids = new int[numberElementBlocks_loading];
  int result = ex_get_elem_blk_ids(exodusFile,block_ids);
  if ( result == -1 )
  {
    PRINT_INFO("Verde:: error reading exodus element block id array...\n");
    delete [] block_ids;
    return VERDE_FAILURE;
  }

  //reset offsets
  block_id_offset = 0;
  nodeset_id_offset = 0;
  sideset_id_offset = 0;

  // if not adding mesh, reset block_id_offset
  if( verde_app->should_add_mesh() )
  {
    block_id_offset = get_offset(1);
    nodeset_id_offset = get_offset(2);
    sideset_id_offset = get_offset(3);
  }

  // load the array of block ids into a list instead

  std::deque <int> file_block_id_list;

  int i = 0;
  for(i=0; i< numberElementBlocks_loading; i++)
  {
    file_block_id_list.push_back(block_ids[i]+block_id_offset);
  }

  delete [] block_ids;
	
  // now, we create a block for each block in the input model
  // if the active_block_id_list list exists and has elements in it
  // we will only activate the blocks in the list.  Otherwise
  // we will activate all blocks in the model

  ElementBlock *block;
  int block_id;

  // create a block for every block in the file
  for(i=0; i< numberElementBlocks_loading; i++)
  {
    block_id = file_block_id_list[i];
    block = new ElementBlock(block_id);
    elementBlocks.push_back(block);
        
    // only set the block as inactive if there is an active_block_id_list
    // and the block_id is not in it
    if (active_block_id_list && 
        !(std::find(active_block_id_list->begin(), active_block_id_list->end(),
          block_id) != active_block_id_list->end()))
    {
      block->active(VERDE_FALSE);
    }
    else
    {
      block->active(VERDE_TRUE);
    }
  }

  // read in the nodesets for the model

  // we will get the max size of the nodesets and sidesets and size the
  // id array for the largest, so that we can use it twice
    
  int array_size = VERDE_MAX(number_node_sets, number_side_sets);
  int *id_array = new int[array_size];

  // read in the nodeset ids

  if(number_node_sets)
    ex_get_node_set_ids(exodusFile, id_array);

  // create a node set for each one

  int *node_id_array;
  double *factor_array;
  for(i = 0; i < number_node_sets; i++)
  {
    // get nodeset parameters
    int number_nodes_in_set;
    int number_dist_factors_in_set;
    ex_get_node_set_param(exodusFile, id_array[i], &number_nodes_in_set,
                          &number_dist_factors_in_set);

    // size new arrays and get ids and distribution factors
    node_id_array = new int[number_nodes_in_set];
    ex_get_node_set(exodusFile, id_array[i], node_id_array);
    factor_array = new double[number_dist_factors_in_set];
    ex_get_node_set_dist_fact(exodusFile, id_array[i],
                              factor_array);

    // offset element ids if adding a mesh
    if( verde_app->should_add_mesh() )
      for(int j=0; j<number_nodes_in_set; j++)
      {
         int offset = numberNodes - numberNodes_loading;
         node_id_array[j] += offset;
      }

    // now, create a new node set and load 

    NodeBC *node_bc = new NodeBC(id_array[i]+nodeset_id_offset);
    nodeBCs.push_back(node_bc);
    node_bc->load(number_nodes_in_set, node_id_array,
                      number_dist_factors_in_set, factor_array);
    // note, we do not delete the node_id_array or factor_array
    // these will be deleted by the NodeBC
  }

  // now, read in the sidesets


  // read in the sideset ids

  if(number_side_sets)
    ex_get_side_set_ids(exodusFile, id_array);

  // create a side set for each one

  int *side_list;
  int *element_list;
  int number_sides_in_set;
  for(i = 0; i < number_side_sets; i++)
  {
    // get sideset parameters
    //int number_side_in_set;
    int number_dist_factors_in_set;
    ex_get_side_set_param(exodusFile, id_array[i], &number_sides_in_set,
                          &number_dist_factors_in_set);

    // size new arrays and get element and side lists
    side_list = new int[number_sides_in_set];
    element_list = new int[number_sides_in_set];
    ex_get_side_set(exodusFile, id_array[i], element_list, side_list);

    // offset element ids if adding a mesh
    if( verde_app->should_add_mesh() )
      for(int j=0; j<number_sides_in_set; j++)
      {
         int offset = numberElements - numberElements_loading;
         element_list[j] += offset;
      }

    // now get factors

    factor_array = new double[number_dist_factors_in_set];
    ex_get_side_set_dist_fact(exodusFile, id_array[i],
                              factor_array);

    // now, create a new side set and load 

    ElementBC *element_bc = new ElementBC(id_array[i] + sideset_id_offset);
    elementBCs.push_back(element_bc);
    element_bc->load(number_sides_in_set, element_list, side_list,
                      number_dist_factors_in_set, factor_array);
    // note, we do not delete the node_id_array or factor_array
    // these will be deleted by the NodeBC
  }

  // delete the id_array
  delete [] id_array;

	
  EdgeTool::instance()->initialize(numberNodes);

  return VERDE_SUCCESS;
}


/*! 
iterates through Mesh::elementBlocks.  For each active block,
loads the block into memory
*/
void Mesh::load_active_element_blocks()
{
	PRINT_INFO("Reading block:");
  for(unsigned int i=0; i<elementBlocks.size(); i++)
  {
    ElementBlock *block = elementBlocks[i];
    if (block->is_active())
    {
      PRINT_INFO(" %i,", block->block_id());
      load_element_block(block);
    }
  }
}

/*! 
Searches Mesh::elementBlocks for a block with the given id.
if found, loads the desired block into memory and returns VERDE_SUCCESS
if not found, prints error and returns VERDE_FAILURE
*/
VerdeStatus Mesh::load_element_block(int block_id)
{
  for(unsigned int i=0; i<elementBlocks.size(); i++)
  {
    ElementBlock *block = elementBlocks[i];
    if (block->block_id() == block_id)
    {
      return load_element_block(block);
    }
  }

  // if we get here, the block id was bad

  PRINT_ERROR("Cannot Load block %d, not found...\n",block_id);
  return VERDE_FAILURE;

}

/*! 
if the block is already loaded, returns VERDE_SUCCESS
if the block is inactive, loads the block into memory.
if successful, returns VERDE_SUCCESS.  Otherwise, returns
VERDE_FAILURE
*/
VerdeStatus Mesh::load_element_block(ElementBlock *block)
{
  // see if the block is already loaded
  if (block->elements_loaded() == VERDE_TRUE )
  {
    return VERDE_SUCCESS;
  }

  // attempt to load the block
  if (block->load_elements())
  {
    //skin the block too
    block->skin_tool()->initialize( block->number_elements() );
    block->skin_tool()->add_block(block);
 
    return VERDE_SUCCESS;
  }

  // failed

  return VERDE_FAILURE;
}

VerdeStatus Mesh::verify_mesh(VerdeBoolean do_metrics,
                              VerdeBoolean do_topology,
                              VerdeBoolean do_interface,
                              VerdeBoolean print_failed_elements,
                              VerdeBoolean do_individual,
                              VerdeBoolean &metrics_success,
                              VerdeBoolean &topology_success,
                              VerdeBoolean &interface_success)
{


  metrics_success = VERDE_FALSE;
  topology_success = VERDE_FALSE;
  interface_success = VERDE_FALSE;

  // check to make sure the mesh is loaded first
  if(elementBlocks.size() == 0)
  {
    // check to see that we have a file name
    if(strcmp(verde_app->input_file().c_str(), "") != 0)
    {
      open_exodus_file(verde_app->input_file().c_str(), NULL);
    }
    else
    {
      PRINT_INFO("ERROR: No mesh was loaded.\n");
      return VERDE_FAILURE;
    }
  }
	 
  // check to see if any element blocks are active 
  std::deque<ElementBlock*>::iterator iter;
  int are_active_blocks = 0;

  for(iter = elementBlocks.begin(); iter != elementBlocks.end(); ++iter)
  { 
    if((*iter)->is_active())
    {
      are_active_blocks = 1;
      break;
    } 
  }
  
  if( are_active_blocks == 0 )
  {
    PRINT_INFO("ERROR: No mesh was loaded.\n");
    return VERDE_FAILURE;
  }


  metrics_success = VERDE_TRUE;
  topology_success = VERDE_TRUE;
  interface_success = VERDE_TRUE;

  VerdeBoolean do_skin = VERDE_TRUE;
  VerdeBoolean do_edges = VERDE_TRUE;
   

  // there are several reasons we need the model skin, so decide here
   
  VerdeBoolean do_skin_calcs = VERDE_FALSE;
  if (do_skin || do_edges || do_topology || do_interface)
  {
    do_skin_calcs = VERDE_TRUE;
//  SkinTool::instance()->initialize(numberNodes);
  }

  VerdeBoolean do_edge_calcs = VERDE_FALSE;
  if(do_topology || do_edges)
  {
    do_edge_calcs = VERDE_TRUE;
    EulerTool::instance()->reset();
    EulerTool::instance()->initialize(numberNodes);
  }
   
  if(do_metrics)
    PRINT_INFO("\n---------------\n"
               "Element Metrics\n"
               "---------------\n");
  
  /*VerdeBoolean do_error =*/ ErrorTool::instance()->do_error();
  
  // create each mesh block and process.

  ElementBlock *block;
  //int block_id;
  //elementBlocks.reset();
  //blockList.reset();
  numberHexes = 0;
  numberTets = 0;
  numberPyramids = 0;
  numberWedges = 0;
  numberKnives = 0;
  numberTris = 0;
  numberQuads = 0;
  numberEdges = 0;
  meshVolume = 0.0;
  meshArea = 0.0;
  HexMetric::instance()->reset_all();
  TetMetric::instance()->reset_all();
  PyramidMetric::instance()->reset_all();
  WedgeMetric::instance()->reset_all();
  KnifeMetric::instance()->reset_all();
  TriMetric::instance()->reset_all();
  QuadMetric::instance()->reset_all();
  EdgeMetric::instance()->reset_all();
  SkinTool::reset_calculated();
  EdgeTool::instance()->reset_calculated();
   
  for (unsigned int i = 0; i < elementBlocks.size(); i++)
  {
    // process active blocks

    block = elementBlocks[i];

    if (block->is_active())
    {
	              
      block->process(do_skin_calcs, do_metrics, do_individual,
                    do_edge_calcs);
         
      // keep track of elements processed, volume and area
      // we wait until now to check on the element type because
      // for degenerate types, block::process may change types
      
      // note: resetting of areas and volumes are done inside
      // the ElementBlock::process*() function
         
      const char *element_type = block->element_type().c_str();
      if (strncmp(element_type,"HEX",3) == 0)
      {
	  numberHexes += block->number_elements();
        meshVolume += HexMetric::instance()->volume();
      }
      else if(strncmp(element_type,"TET",3) == 0)
      {
	 numberTets += block->number_elements();
        meshVolume += TetMetric::instance()->volume();
      }
         
      else if(strncmp(element_type,"PYR",3) == 0)
      {
        numberPyramids += block->number_elements();
        meshVolume += PyramidMetric::instance()->volume();
      }
      else if(strncmp(element_type,"WED",3) == 0)
      {
	numberWedges += block->number_elements();
        meshVolume += WedgeMetric::instance()->volume();
      }
      else if(strncmp(element_type,"KNI",3) == 0)
      {
        numberKnives += block->number_elements();
        meshVolume += KnifeMetric::instance()->volume();
      }
      else if(strncmp(element_type,"QUAD",4) == 0 ||
              strncmp(element_type,"SHELL",5) == 0)
      {
        numberQuads += block->number_elements();
        meshArea += QuadMetric::instance()->area();
      }
      else if(strncmp(element_type,"TRI",3) == 0)
      {
        meshArea += TriMetric::instance()->area();
        numberTris += block->number_elements();
      }
      else if(strncmp(element_type,"BEA",3) == 0 ||
              strncmp(element_type,"BAR",3) == 0 ||
              strncmp(element_type,"TRU",3) == 0)
      {
        numberEdges += block->number_elements();
      }
    }
  }


   

  // print out element summary
  if (do_metrics)
  {
    // print out summary metrics by element type

    if (numberHexes)
    {
      PRINT_INFO("\nSummary Hex Metrics, total = %d Hexes:\n\n",
                 numberHexes);
      HexMetric::instance()->print_summary();
    }
    if (numberTets)
    {
      PRINT_INFO("\nSummary Tet Metrics, total = %d Tets:\n\n",
                 numberTets);
      TetMetric::instance()->print_summary();
    }
    if (numberPyramids)
    {
      PRINT_INFO("\nSummary Pyramid Metrics, total = %d Pyramids:\n\n",
                 numberPyramids);
      PyramidMetric::instance()->print_summary();
    }
    if (numberWedges)
    {
      PRINT_INFO("\nSummary Wedge Metrics, total = %d Wedges:\n\n",
                 numberWedges);
      WedgeMetric::instance()->print_summary();
    }
    if (numberKnives)
    {
      PRINT_INFO("\nSummary Knife Metrics, total = %d Knives:\n\n",
                 numberKnives);
      KnifeMetric::instance()->print_summary();
    }
    if (numberQuads)
    {
      PRINT_INFO("\nSummary Quad Metrics, total = %d Quads:\n\n",
                 numberQuads);
      QuadMetric::instance()->print_summary();
    }
    if (numberTris)
    {
      PRINT_INFO("\nSummary Tri Metrics, total = %d Tris:\n\n",
                 numberTris);
      TriMetric::instance()->print_summary();
    }
    if (numberEdges)
    {
      PRINT_INFO("\nSummary Edge Metrics, total = %d Edges:\n\n",
                 numberEdges);
      EdgeMetric::instance()->print_summary();
    }
     
         
    PRINT_INFO("\n---------------\n"
               "Metrics Summary\n"
               "---------------\n");
    int num_failed;
    if (numberHexes)
    {
      num_failed = HexMetric::instance()->
        print_failed_hexes(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
      
    if (numberTets)
    {
      num_failed = TetMetric::instance()->
        print_failed_tets(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
    if (numberPyramids)
    {
      num_failed = PyramidMetric::instance()->
        print_failed_pyramids(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
    if (numberWedges)
    {
      num_failed = WedgeMetric::instance()->
        print_failed_wedges(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
    if (numberKnives)
    {
      num_failed = KnifeMetric::instance()->print_failed_knives(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
    if (numberQuads)
    {
      num_failed = QuadMetric::instance()->
        print_failed_quads(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
    if (numberTris)
    {
      num_failed = TriMetric::instance()->print_failed_tris(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
    if (numberEdges)
    {
      num_failed = EdgeMetric::instance()->print_failed_edges(print_failed_elements);
      if (num_failed)
        metrics_success = VERDE_FALSE;
    }
  }
   
  
  if(SkinTool::get_quads_sharing_three_nodes_completeness() > 0)
  {
    PRINT_INFO("\n----------------------------\n"
               "Quads Sharing 3 Nodes\n"
               "----------------------------\n");
    // check for quads sharing three nodes
    SkinTool::find_quads_share_three_nodes();
    if(SkinTool::number_quads_share_three_nodes())
      PRINT_INFO("\n");
    PRINT_INFO("   Number Quads sharing three nodes:  %d\n",
        SkinTool::number_quads_share_three_nodes());

  }

  // print out Topology results

  if (do_topology)
  {

    // write out Euler banner
   
    PRINT_INFO("\n----------------------------\n"
                "Mesh Topology Information\n"
                "----------------------------\n");

    // if non_manifold edges are present, it is non-manifold

    if (EdgeTool::instance()->number_non_manifold_edges() > 0)
    {
      PRINT_INFO("   Topology: Non-Manifold\n");
      PRINT_INFO("   Number Non-Manifold Edges:  %d\n",
                  EdgeTool::instance()->number_non_manifold_edges());
      topology_success = VERDE_FALSE;
    }
      // if there are bar elements in the model, we cannot calculate
      // an Euler number with bars around. However, we do not cause
      // a failure for this reason...
    else if (SkinTool::number_exterior_edges() > 0)
    {
      PRINT_INFO("   Topology: Unknown (Bar/Beam/Truss elements present)\n");
      topology_success = VERDE_TRUE;
    }
    else
    {

      topology_success = EulerTool::instance()->
                         calculate_euler(numberHexes,
                         numberTets,
                         numberPyramids,
                         numberWedges,
                         numberKnives);
    }
}
 
  // do interface checking stuff

  if (do_interface)
  {

    // first, check for coincident nodes.  If there are none, there can
    // not be any coincident elements (SkinTool will quit if there aren't
    // any coincident nodes )

    SkinTool::find_coincident_nodes();
    SkinTool::find_coincident_elements();

    // check for non-conformal triangles on the boundary
    SkinTool::find_non_conformal_tris();
    
  }

  // print out model skin and edge information

  if (do_interface)
  {
    PRINT_INFO("\n-------------------------\n"
               "Mesh Interface Information\n"
               "-------------------------\n");

    // print out skin information
    if (do_skin)
    {
      PRINT_INFO("   Number Surface Quads:     %d\n"
                 "   Number Surface Tris:      %d\n",
                 SkinTool::number_exterior_quads(),
                 SkinTool::number_exterior_tris());
    }
      
    if (do_interface)
    {
      PRINT_INFO("   Number Coincident Nodes:  %d\n"
                 "   Number Coincident Quads:   %d\n"
                 "   Number Coincident Tris:   %d\n",
                 SkinTool::number_coincident_nodes(),
                 SkinTool::number_coincident_quads(),
                 SkinTool::number_coincident_tris());
      PRINT_INFO("   Number Model Edges:       %d\n",
                 EdgeTool::instance()->number_exterior_edges());
      
      PRINT_INFO("\n");
    }


      
    if (do_skin )
      PRINT_INFO("\n");
      
         
    if (do_interface)
    {
      // output warning if there are coincident and/or identical edges

      if (EdgeTool::instance()->number_coincident_edges() ||
          SkinTool::number_coincident_quads() ||
          SkinTool::number_coincident_quads() )
      {
        PRINT_INFO("   WARNING: model contains coincident nodes, quads "
                   "and/or tris.\n"
                   "   This may mean either:\n"
                   "   1) There are unmerged surfaces in the model, "
                   "and/or\n"
                   "   2) There are contact surfaces in the model.\n");
        topology_success = VERDE_FALSE;
      }
      if (SkinTool::number_non_conformal_tris())
      {
        PRINT_INFO("   WARNING: Model contains %d non-conformal "
                   "triangles\n", 
                   SkinTool::number_non_conformal_tris());
      }
    } 
  }
  
   
  return VERDE_SUCCESS;
}


void Mesh::element_blocks(std::deque<ElementBlock*> &block_list)
{
  block_list = elementBlocks;
}

void Mesh::get_node_bc_list(std::deque<NodeBC*> &node_bc_list )
{
  node_bc_list = nodeBCs;
}

void Mesh::get_element_bc_list(std::deque<ElementBC*> &element_bc_list )
{
  element_bc_list = elementBCs;
}


// int Mesh::node_id_of_element(int element_index, int node_index)
// {
//   int block_element_offset = 0;
//   elementBlocks.reset();
//   ElementBlock* tmp_block = 0;
	
//   for(int i = 0; i < elementBlocks.size(); i++)
//   {
//     tmp_block = elementBlocks.get_and_step();
		
//     if(element_index <= (block_element_offset + tmp_block->number_elements() ))
//     {
//       return tmp_block->node_id(element_index - block_element_offset,
//                                 node_index);
//     }
//     block_element_offset += tmp_block->number_elements();
//   }
//   return 0;
// }

int Mesh::original_id(int index)
{
	        return ElementBlock::original_id(index);
}

VerdeVector Mesh::coordinates(int node_index)
{
	return nodeContainer->coordinates(node_index);
}  


VerdeStatus Mesh::find_current_boundary()
{

  // create a skintool to calculate the boundary, and reset

  //SkinTool *skin_tool = SkinTool::instance(); 
  //skinTool.reset();
  //skinTool.initialize(numberNodes);

  // get each active block and input its elements

  return VERDE_SUCCESS;

}

/*!
Retrieves a block of the Mesh from Mesh::elementBlocks by id
If no block with that id exists, it returns NULL
*/
ElementBlock *Mesh::get_element_block(int id)
{
   for(unsigned int i=0; i<elementBlocks.size(); i++)
   {
     ElementBlock *block = elementBlocks[i];
     if(block->block_id() == id)
     {
       return block;
     }
   }
   return NULL;
}

/*!
Retrieves a NodeBC of the Mesh from Mesh::nodeBCs by id
If no NodeBC with that id exists, it returns NULL
*/
NodeBC *Mesh::get_node_bc(int id)
{
   for(unsigned int i=0; i<nodeBCs.size(); i++)
   {
     NodeBC *bc = nodeBCs[i];
     if(bc->id() == id)
     {
       return bc;
     }
   }
   return NULL;
}

/*!
Retrieves a ElementBC of the Mesh from Mesh::elementBCs by id
If no ElementBC with that id exists, it returns NULL
*/
ElementBC *Mesh::get_element_bc(int id)
{
   for(unsigned int i=0; i<elementBCs.size(); i++)
   {
     ElementBC *bc = elementBCs[i];
     if(bc->id() == id)
     {
       return bc;
     }
   }
   return NULL;
}

ElementBlock *Mesh::find_block_for_element(int element_id)
{
  
  // find which block this id is located in

  ElementBlock *block;
  for(unsigned int i=0; i<elementBlocks.size(); i++)
  {
    block = elementBlocks[i];
    int start_id = block->element_offset();
    if (start_id < element_id && 
        element_id <= start_id + block->number_elements())
    {
      // this is the block!
      return block;
    }
  }
  return NULL;
}

int Mesh::get_offset( int flag )
{
  unsigned int i;
  int high_id = 0; 

  switch(flag)
  {
    case 1:
    {
      //go through all blocks....find one with highest id
      for(i=0; i<elementBlocks.size(); i++ )
      {
        ElementBlock* block = elementBlocks[i];
        if( block->block_id() > high_id )
          high_id = block->block_id();
      } 
      return high_id;
    }
    case 2:
    {
      //go through all node BC's....find one with highest id
      for(i=0; i<nodeBCs.size(); i++)
      {
        NodeBC* node_bc = nodeBCs[i];
        if( node_bc->id() > high_id )
          high_id = node_bc->id();
      } 
      return high_id;
    }
    case 3:
    {
      //go through all element BC's....find one with highest id
      for(i=0; i<elementBCs.size(); i++)
      {
        ElementBC* element_bc = elementBCs[i];
        if( element_bc->id() > high_id )
          high_id = element_bc->id();
      } 
      return high_id;
    }
  }
   
  return 0;
}

VerdeStatus Mesh::read_qa_string(int genesis_file_id,
                                 char *string,
                                 int record_number,
                                 int record_position)
{
      // get the variable id in the exodus file

  int varid;
    // temp change of variable string until read_qa_string and write_qa_string
    // return to the same file
  //if ((varid = ncvarid (genesis_file_id, VAR_QA_TITLE)) == -1)
  if ((varid = ncvarid (genesis_file_id, "qa_records")) == -1)
  {
    PRINT_ERROR("failed to find qa record array in file id %d", genesis_file_id);
    return VERDE_FAILURE;
  }


//  read in the record one char at a time

  long start[3];

  start[0] = record_number;
  start[1] = record_position;
  start[2] = 0;

  // always read in the first char

  char *current_pos = string;
   if (ncvarget1 (genesis_file_id, varid, start,
              VOID_PTR current_pos) == -1)
  {
    PRINT_ERROR("failed to read qa string in file id %d\n", genesis_file_id);
    return (VERDE_FAILURE);
  }

   // now, read until the terminator is found

   int k = 0;
   while( ( (*current_pos++) != '\0') && (k < MAX_STR_LENGTH) )
   {
     start[2] = ++k;
     if (ncvarget1 (genesis_file_id, varid, start,
                VOID_PTR current_pos) == -1)
     {
       PRINT_ERROR("failed to read qa string in file id %d\n", genesis_file_id);
       return (VERDE_FAILURE);
     }
   }

   // the string is in, now strip out trailing blanks
   // note that current_pos now points beyond the \0

   if (start[2] != 0)
   {
      current_pos -=2;
      while( (current_pos > string) && (*current_pos == ' ') )
      {
        // replace ' ' with '\0'
        *current_pos-- = '\0';
      }
   }


  return VERDE_SUCCESS;
}


VerdeStatus Mesh::read_qa_information(int genesis_file_id,
                                std::deque<char*> &qa_record_list)
{
  // inquire on the genesis file to find the number of qa records

  int number_records;

  ex_inquire(genesis_file_id, EX_INQ_QA, &number_records, NULL, NULL);

  for(int i = 0; i < number_records; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      char *data = new char[80];
      if (read_qa_string(genesis_file_id, data, i, j) != VERDE_SUCCESS)
      {
        return VERDE_FAILURE;
      }
      qa_record_list.push_back(data);
    }
  }
  return VERDE_SUCCESS;
}
         
