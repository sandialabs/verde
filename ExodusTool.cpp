//-------------------------------------------------------------------------
// Filename      : ExodusTool.cc 
//
// Purpose       : Class to output Exodus File for VERDE
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/06/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------


#include <algorithm>
#include <time.h>

#include <qstring.h>

#include "ExodusTool.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"
#include "NodeRef.hpp"
#include "SkinTool.hpp"
#include "VerdeString.hpp"
#include "HexMetric.hpp"
#include "TetMetric.hpp"
#include "PyramidMetric.hpp"
#include "WedgeMetric.hpp"
#include "KnifeMetric.hpp"
#include "QuadMetric.hpp"
#include "TriMetric.hpp"
#include "EdgeMetric.hpp"
#include "FailedHex.hpp"
#include "FailedTet.hpp"
#include "FailedPyramid.hpp"
#include "FailedWedge.hpp"
#include "FailedKnife.hpp"
#include "FailedQuad.hpp"
#include "FailedTri.hpp"
#include "FailedEdge.hpp"
#include "EdgeTool.hpp"
#include "EdgeElem.hpp"
#include "ElementBlock.hpp"
#include "NodeBC.hpp"
#include "ElementBC.hpp"

#include "exodusII.h"

const int TIME_STR_LEN = 11;

ExodusTool::ExodusTool()
{
   quadConnectivity = NULL;
   triConnectivity = NULL;
   nodeMap = NULL;
   xCoords = NULL;
   yCoords = NULL;
   zCoords = NULL;


   numDimensions = 3;
   numElementBlocks = 0;
   currentElementBlockID = 0;
   numberTriElements = 0;
   numberQuadElements = 0;
   totalElements = 0;
   
}

ExodusTool::~ExodusTool()
{
   if (quadConnectivity)
      delete [] quadConnectivity;
   if (triConnectivity)
      delete [] triConnectivity;
   if (nodeMap)
      delete [] nodeMap;
   if (xCoords)
      delete [] xCoords;
   if (yCoords)
      delete [] yCoords;
   if (zCoords)
      delete [] zCoords;

   unsigned int i;
   // exteriorQuads are obtained from the SkinTool.  Let the
   // SkinTool clean them up!
   exteriorQuads.clear();

   for(i=0; i<exteriorNodes.size(); i++)
     delete exteriorNodes[i];
   exteriorNodes.clear();
   
}

   

VerdeStatus ExodusTool::write_results_output_file(VerdeString output_file,
                                          VerdeBoolean do_skin,
                                          VerdeBoolean do_edges)
{
   // write status info
   PRINT_INFO("\n------------------\n"
              "Output Information\n"
              "------------------\n");
   
   // gather data for block output

   VerdeStatus success = gather_data(do_skin,do_edges);

   if (success == VERDE_FAILURE)  // nothing to output
   {
      PRINT_INFO("   INFO: Output File empty, not saved...\n");
      return VERDE_FAILURE;
   }
   
   success = open_file(output_file.c_str());
   if (success == VERDE_FAILURE)
      return VERDE_FAILURE;

   outputFile = output_file;

   PRINT_INFO("   Writing Output to exodus file %s:\n",outputFile.c_str());
   


   // write Header information

   success = write_header();

   // write the nodal information

   PRINT_INFO("   Writing %d exterior nodes...\n",numberNodes);
   success = write_nodes();

   // write the surface element data
   
   success = write_surface_element_blocks();

   // write the model edge block

   success = write_model_edge_blocks();

   // Write out coincident quad/tri information

   success = write_coincident_element_blocks();

   // write failed hex and tet data

   success = write_failed_element_blocks();

   // write the element number map

   success = write_element_number_map();
   
   // close the file
   
   ex_close(genesisFileId);
   
   return success;
}


VerdeStatus ExodusTool::output_mesh_file(Mesh* mesh, QString filename)
{

  VerdeStatus success;
  
  this->outputFile = (const char*)filename;

  // try to get mesh information
  ExodusMeshInfo mesh_info;
  std::deque<BlockData> block_info;
  std::deque<SideSetData> sideset_info;
  std::deque<NodeSetData> nodeset_info;
  
  if(gather_mesh_information(mesh, mesh_info, block_info, sideset_info,
                             nodeset_info) != VERDE_SUCCESS)
    return VERDE_FAILURE;
  
  // try to open the file after gather mesh info succeeds
  if(this->open_file(filename) != VERDE_SUCCESS)
    return VERDE_FAILURE;

  success = write_header(mesh_info, block_info, sideset_info, 
                         nodeset_info,filename);

  success = write_qa_records(mesh_info);

  success = write_nodes(mesh_info);

  success = write_elementblocks(mesh_info);

  // write the three maps
  success = write_global_node_order_map(mesh_info);
  success = write_global_element_order_map(mesh_info);
  success = write_element_order_map(mesh_info);
  
  //success = write_elementmap(mesh_info);

  success = write_BCs(mesh_info);

  // close the file now that we are done
  ex_close(genesisFileId);

  return VERDE_SUCCESS;
}

VerdeStatus ExodusTool::open_file(QString filename)
{
   // not a valid filname
   if(strlen((const char*)filename) == 0)
   {
      PRINT_ERROR("Output Exodus filename not specified\n");
      return VERDE_FAILURE;
   }

   // size of floating point values in verde
   int verde_word_size = sizeof(double);
   // size of floating point values that will go in the file
   int file_word_size = sizeof(double);
   
   genesisFileId = ex_create( 
         filename,                 // filename
         EX_CLOBBER,               // overwrite if needed
         &verde_word_size,         // verde real size
         &file_word_size );        // file real size

   // file couldn't be opened
   if(this->genesisFileId < 0)
   {
      PRINT_ERROR("Cannot open %s\n", (const char*)filename);
      return VERDE_FAILURE;
   }
   return VERDE_SUCCESS;
}


VerdeStatus ExodusTool::gather_mesh_information(Mesh* mesh,
                        ExodusMeshInfo& mesh_info, 
                        std::deque<BlockData> &block_info,
                        std::deque<SideSetData> &sideset_info, 
                        std::deque<NodeSetData> &nodeset_info)
{

  // we have to have an existing mesh and add a node check for fun too.
  if(!mesh || !mesh->node_container())
    return VERDE_FAILURE;

  // initialize mesh_info and other stuff
  mesh->node_container()->clear_marked_nodes();
  mesh_info.mesh = mesh;
  mesh_info.num_dim = 3;
  mesh_info.num_nodes = 0;
  mesh_info.num_elements = 0;
  mesh_info.num_elementblocks = 0;
  mesh_info.num_sidesets = 0;
  mesh_info.num_nodesets = 0;
  mesh_info.num_qa_records = mesh->number_qa_strings()/4 +1;  // add one for Verde time stamp
  
  // find the number of nodes, elements, and element blocks
  std::deque<ElementBlock*> element_blocks;
  mesh->element_blocks(element_blocks);
  unsigned int i;
  unsigned int new_node_id=1, new_elem_id=1;
  int j;
  
  for(i = 0; i < element_blocks.size(); i++)
  {
    // get the block
    ElementBlock* block = element_blocks[i];
    
    // if the block is inactive or invalid
    if(!block || block->is_active() == VERDE_FALSE)
      continue;

    // add one to the number of element blocks
    mesh_info.num_elementblocks++;
    
    // get information before we go through each element
    int j = block->element_offset()+1;
    int num_elems_plus1 = j + block->number_elements();
    MeshContainer* mesh_cont = block->mesh_container();
    int num_nodes_per_elem = ElemRef(*mesh_cont,j).num_nodes();

    // load information into the BlockData for initialization
    BlockData block_data;
    block_data.id = block->block_id();
    block_data.number_elements = block->number_elements();
    block_data.number_nodes_per_element = num_nodes_per_elem;
    //! todo: implement attributes
    block_data.number_attributes = block->mesh_container()->number_attributes();
    block_data.element_type = block->element_type().c_str();
    block_info.push_back(block_data);
    
    // add the number of elements
    mesh_info.num_elements += block_data.number_elements;

    // go through each element and mark it, mark the nodes, and count nodes
    for( ; j<num_elems_plus1; j++)
    {
      ElemRef elem(*mesh_cont, j-(block->element_offset()) );
      elem.mark();
      elem.set_exodus_id(new_elem_id);
      new_elem_id++;

      int z;
      elem.set(mesh_cont, j);
      for(z=0; z<num_nodes_per_elem; z++)
      {
        NodeRef node(elem.node_id(z));
        if(!node.is_marked())
        {
          node.mark();
          mesh_info.num_nodes++;
          node.set_exodus_id(new_node_id);
          new_node_id++;
        }
      }
    }
  }

  // find the number of boundary conditions
  std::deque<NodeBC*> nodesets;
  std::deque<ElementBC*> sidesets;
  mesh->get_node_bc_list(nodesets);
  mesh->get_element_bc_list(sidesets);

  for(i=0; i<nodesets.size(); i++)
  {
    NodeBC* node_bc = nodesets[i]; 

    //see if nodset is active
    if(node_bc->is_active())
    {
      //see if any underlying nodes are active
 
      //get number of nodes in set
      int number_nodes = node_bc->number_active_nodes();

      //get node array
      int* list_id_ptr = node_bc->get_nodeIds();

      // load the NodeSet data structure
      NodeSetData nodeset_data;
      nodeset_data.id = node_bc->id();
      nodeset_data.number_nodes = number_nodes;
      nodeset_info.push_back(nodeset_data);
      
       
      //iterate through nodes
      for( j=0; j<number_nodes; j++ )
      {
         int id = list_id_ptr[j];
         NodeRef node(id);
	     if(node.is_marked() )
         {
       	   mesh_info.num_nodesets++;
	       break;
         }
      }
    }
  }

  for(i=0; i<sidesets.size(); i++)
  {
    ElementBC* element_bc = sidesets[i];

    if(element_bc->is_active())
    {
      //get number of sidesets in set
      int number_elements = element_bc->number_elements();

      //get element array
      int* list_id_ptr = element_bc->get_elementIds();

      // load the SideSet data structure
      SideSetData sideset_data;
      sideset_data.id = element_bc->id();
      sideset_data.number_nodes = element_bc->number_dist_factors();
      sideset_data.number_elements = element_bc->number_active_elements(mesh_info.mesh);
      sideset_info.push_back(sideset_data);
      

      //iterate through sidesets 
      for( j=0; j<number_elements; j++ )
      {
         //get original id of element
        int id = list_id_ptr[j];

        //get block element 
        ElementBlock* block = mesh_info.mesh->find_block_for_element(id); 

        //get out if block of element is not active
        if( block->is_active() )
	{
      	  mesh_info.num_sidesets++;
            break;
	}
      }
    }
  }

  return VERDE_SUCCESS;
  
}

VerdeStatus ExodusTool::gather_data(VerdeBoolean do_skin,
                                    VerdeBoolean do_edges)
{
   int i;

   // clear any marks on nodes

   ElemRef::node_container()->clear_marked_nodes();

   // gather skin data
   
   if (do_skin)
   {
      gather_skin_data();
   }
   
   // gather edge data
   // check for model edge blocks

   if (do_edges)
   {
      gather_edge_data(do_skin);
   }

   // gather coincident data

   gather_coincident_data(do_skin);
   
   
   // gather failed element data

   gather_failed_data();
   

   
   if (exteriorNodes.size() == 0)
      return VERDE_FAILURE;
   
   // allocate and populate the node map array

   nodeMap = new int[exteriorNodes.size()];
   xCoords = new double[exteriorNodes.size()];
   yCoords = new double[exteriorNodes.size()];
   zCoords = new double[exteriorNodes.size()];
   VerdeVector coords;

   for (unsigned int j = 0; j < exteriorNodes.size(); j++)
   {
      NodeRef *node_ref =  exteriorNodes[j];
      nodeMap[j] = node_ref->index();
      node_ref->set_exodus_id(j+1);
      coords = node_ref->coordinates();
      xCoords[j] = double(coords.x());
      yCoords[j] = double(coords.y());
      zCoords[j] = double(coords.z());
   }
   
   // substitute in exodus_id in the connectivity array

   int conn_length = exteriorQuads.size() *4;
   
   for (i = 0; i < conn_length; i++)
   {
      NodeRef node_ref(quadConnectivity[i]);
      quadConnectivity[i] = node_ref.exodus_id();
   }

   conn_length = exteriorTris.size() *3;
   
   for (i = 0; i < conn_length; i++)
   {
      NodeRef node_ref(triConnectivity[i]);
      triConnectivity[i] = node_ref.exodus_id();
   }


   // record number of nodes to output

   numberNodes = exteriorNodes.size();

   return VERDE_SUCCESS;
   
}

void time_and_date(char* time_string, char* date_string)
{
   struct tm* local_time;
   time_t calendar_time;

   calendar_time = time(NULL);
   local_time = localtime(&calendar_time);

   strftime(time_string, TIME_STR_LEN, "%H:%M:%S", local_time);
   strftime(date_string, TIME_STR_LEN, "%m/%d/%Y", local_time);
   
   // I don't understand why this isn't working right without this,...
   time_string[8] = (char)NULL;
   date_string[10] = (char)NULL;
}


VerdeStatus ExodusTool::write_header(ExodusMeshInfo& mesh_info, 
                                     std::deque<BlockData> &block_info,
                                     std::deque<SideSetData> &sideset_info,
                                     std::deque<NodeSetData> &nodeset_info,
                                     QString filename)
{
  // get the date and time
  char time[TIME_STR_LEN];
  char date[TIME_STR_LEN];
  time_and_date(time,date);

  QString title_string = QString("VERDE").upper() + QString("(") 
    + filename + QString("): ") + QString(date) + QString(": ") + QString(time);

  if(title_string.length() > MAX_LINE_LENGTH)
    title_string.truncate( MAX_LINE_LENGTH );

 
  // initialize the exodus file

  int result = initialize_exodus_file(genesisFileId, mesh_info,
                                      block_info, sideset_info,
                                      nodeset_info, title_string);

  if(result == EX_FATAL)
    return VERDE_FAILURE;
  

/*
  ex_put_init(genesisFileId, title_string, mesh_info.num_dim,
      mesh_info.num_nodes, mesh_info.num_elements,
      mesh_info.num_elementblocks, mesh_info.num_nodesets,
      mesh_info.num_sidesets);
*/


  // todo: write qa records ?


  return VERDE_SUCCESS;
}


VerdeStatus ExodusTool::write_header()
{
   // get the date and time
   char time[TIME_STR_LEN];
   char date[TIME_STR_LEN];
   time_and_date(time,date);
   
   VerdeString title_string;
   VerdeString title_after;

   if (title_string.length() == 0)
   {
     title_string = "VERDE";
     title_string.to_upper_case();
     title_string += "(";
     title_after = VerdeString("): ") + date + VerdeString(": ") + time;
      VerdeString fname = outputFile;
      int length = title_string.length() + fname.length() + title_after.length()
;
      if (length > 80)
      {
           // reduce string length to 80 due to Exodus limit
         fname = fname.substr(length-79);
      }

      title_string += outputFile + title_after;
   }

   int numberNodeSets = 0;
   int numberSideSets = 0;

   char test_string[10];
   strcpy(test_string,"Testing ");

   //int error = ex_put_init(1,"Test",3,1,1,1,0,0);

   int error = ex_put_init(genesisFileId, title_string.c_str(), numDimensions,
                           numberNodes, totalElements, numElementBlocks,
                           numberNodeSets, numberSideSets);
   

   if(error < 0)
   {
      PRINT_ERROR("Failed initializing genesis database.\n"  );
      return VERDE_FAILURE;
   }

   // PRINT_INFO("\n\nWriting %d quads, %d tris, and %d nodes to "
   //            "exodus file %s...\n",
   //           numberQuadElements, numberTriElements, numberNodes,
   //           outputFile.c_str());
   return VERDE_SUCCESS;
}


VerdeStatus ExodusTool::write_nodes(ExodusMeshInfo& mesh_info)
{
  // write coordinates names
  char *names[3] = { "x", "y", "z" };
  int success = ex_put_coord_names(genesisFileId, names);
  // todo: is failure denoted by < 0 ??
  if(success < 0)
    return VERDE_FAILURE;
  
  double* coordsx = new double[mesh_info.num_nodes];
  double* coordsy = new double[mesh_info.num_nodes];
  double* coordsz = new double[mesh_info.num_nodes];
  MeshContainer* node_cont = mesh_info.mesh->node_container();

  unsigned int num_total_nodes = node_cont->number_nodes();
  unsigned int i, new_node_id_min1;
  for(i=0; i<num_total_nodes; i++)
  {
    NodeRef node(i+1);
    if(node.is_marked())
    {
      VerdeVector node_coord = node.coordinates();
      new_node_id_min1 = node.exodus_id()-1;
      coordsx[new_node_id_min1] = node_coord.x();
      coordsy[new_node_id_min1] = node_coord.y();
      coordsz[new_node_id_min1] = node_coord.z();
    }
  }

  success = ex_put_coord(genesisFileId, coordsx, coordsy, coordsz);

  // todo: verify the data is not needed anymore before we delete it
  delete [] coordsx;
  delete [] coordsy;
  delete [] coordsz;

  return success < 0 ? VERDE_FAILURE : VERDE_SUCCESS;

}


VerdeStatus ExodusTool::write_nodes()
{
   // write out the nodal coordinate names

   const char *names[3];
   names[0] = "x";
   names[1] = "y";
   names[2] = "z";
   
   int success = ex_put_coord_names(genesisFileId,(char**)names);

   // write out coords

   success = ex_put_coord(genesisFileId,xCoords,yCoords,zCoords);

   if (success != 0)
   {
      PRINT_INFO("Error writing Exodus Coordinate arrays...\n");
      return VERDE_FAILURE;
   }

   // write out node number map
   
   success = ex_put_node_num_map(genesisFileId,nodeMap);

   if (success != 0)
   {
      PRINT_INFO("Error writing Exodus Coordinate arrays...\n");
      return VERDE_FAILURE;
   }
   
   return VERDE_SUCCESS;
}

   
VerdeStatus ExodusTool::write_surface_element_blocks()
{

   // write out the element block parameters for quads
   
   if(numberQuadElements)
   {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d exterior quads to block %d...\n",
                 numberQuadElements, currentElementBlockID);
      int num_nodes_per_element = 4;
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId, currentElementBlockID,
                                      "QUAD4",numberQuadElements,
                                      num_nodes_per_element,
                                      num_attributes);

      // write out quad elements

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                 quadConnectivity);

      if (success != 0)
      {
         PRINT_INFO("Error writing Exodus surface quads...\n");
         return VERDE_FAILURE;
      }
   }

      // write out the element block parameters for tris
   
   if(numberTriElements)
   {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d exterior tris to block %d...\n",
                 numberTriElements, currentElementBlockID);
      int num_nodes_per_element = 3;
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId, currentElementBlockID,
                                      "TRIANGLE",numberTriElements,
                                      num_nodes_per_element,
                                      num_attributes);

      // write out tri elements

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                 triConnectivity);

      if (success != 0)
      {
         PRINT_INFO("Error writing Exodus surface tris...\n");
         return VERDE_FAILURE;
      }
   }
   return VERDE_SUCCESS;

}

   
VerdeStatus ExodusTool::write_failed_hex_blocks()
{
   // go through the failed hex elements, writing a block for each

   int num_nodes_per_element = NUM_HEX_NODES;
   
   int number_hex_metrics = HexMetric::instance()->number_metrics();
   FailedHex *hex;
   int *connectivity;
   
   for (int i = 0; i < number_hex_metrics; i++)
   {
      // gather the hex list for the metric
      
     std::deque<FailedHex*> hex_list;
      HexMetric::instance()->failed_hex_list(i,hex_list);
      int num_elements = hex_list.size();

      connectivity = new int[num_elements*num_nodes_per_element];
      int index = 0;
      
      // process each hex
      
      for (int j =0; j< num_elements; j++)
      {
         hex = hex_list[j];
         elementNumberMap.push_back(hex->original_id());
         for (int k = 0; k < NUM_HEX_NODES; k++)
         {
            NodeRef node_ref(hex->node_id(k));
            connectivity[index++] = node_ref.exodus_id();
         }
      }
      
      // write out the element block
   
      if(index)
      {
         currentElementBlockID++;
         PRINT_INFO("   Writing %d failed hexes (%s) to block %d...\n",
                    num_elements,
                    HexMetric::instance()->metric_name(i),
                    currentElementBlockID);
         int num_attributes = 0;
         int success = ex_put_elem_block(genesisFileId,
                                         currentElementBlockID,
                                         "HEX8",num_elements,
                                         num_nodes_per_element,
                                         num_attributes);

         // write out connectivity

         success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                    connectivity);
         delete [] connectivity;
         if (success != 0)
         {
            PRINT_INFO("Error writing Exodus failed hex blocks...\n");
            return VERDE_FAILURE;
         }
      }
   }
   
   return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_failed_tet_blocks()
{
   // go through the failed tet elements, writing a block for each

   int num_nodes_per_element = NUM_TET_NODES;
   
   int number_tet_metrics = TetMetric::instance()->number_metrics();
   FailedTet *tet;
   int *connectivity;

   for (int i = 0; i < number_tet_metrics; i++)
   {
      // gather the tet list for the metric
      
     std::deque<FailedTet*> tet_list;
     TetMetric::instance()->failed_tet_list(i,tet_list);
      int num_elements = tet_list.size();

      connectivity = new int[num_elements*num_nodes_per_element];
      int index = 0;
      
      // process each tet
      
      for (int j=0; j<num_elements; j++)
      {
         tet = tet_list[j];
         elementNumberMap.push_back(tet->original_id());
         for (int k = 0; k < NUM_TET_NODES; k++)
         {
            NodeRef node_ref(tet->node_id(k));
            connectivity[index++] = node_ref.exodus_id();
         }
      }
      
      // write out the element block
   
      if(index)
      {
         currentElementBlockID++;
         PRINT_INFO("   Writing %d failed tets (%s) to block %d...\n",
                    num_elements,
                    TetMetric::instance()->metric_name(i),
                    currentElementBlockID);
         int num_attributes = 0;
         int success = ex_put_elem_block(genesisFileId,
                                         currentElementBlockID,
                                         "TETRA",num_elements,
                                         num_nodes_per_element,
                                         num_attributes);

         // write out connectivity

         success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                    connectivity);
         delete [] connectivity;
         if (success != 0)
         {
            PRINT_INFO("Error writing Exodus failed tet blocks...\n");
            return VERDE_FAILURE;
         }
      }
   }
   
   return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_failed_pyramid_blocks()
{
   // go through the failed pyramid elements, writing a block for each

   int num_nodes_per_element = NUM_PYRAMID_NODES;
   
   int number_pyramid_metrics = PyramidMetric::instance()->number_metrics();
   FailedPyramid *pyramid;
   int *connectivity;

   for (int i = 0; i < number_pyramid_metrics; i++)
   {
      // gather the pyramid list for the metric
      
     std::deque<FailedPyramid*> pyramid_list;
      PyramidMetric::instance()->failed_pyramid_list(i,pyramid_list);
      int num_elements = pyramid_list.size();

      connectivity = new int[num_elements*num_nodes_per_element];
      int index = 0;
      
      // process each pyramid
      
      for (int j =0; j< num_elements; j++)
      {
         pyramid = pyramid_list[j];
         elementNumberMap.push_back(pyramid->original_id());
         for (int k = 0; k < NUM_PYRAMID_NODES; k++)
         {
            NodeRef node_ref(pyramid->node_id(k));
            connectivity[index++] = node_ref.exodus_id();
         }
      }
      
      // write out the element block
   
      if(index)
      {
         currentElementBlockID++;
         PRINT_INFO("   Writing %d failed pyramids (%s) to block %d...\n",
                    num_elements,
                    PyramidMetric::instance()->metric_name(i),
                    currentElementBlockID);
         int num_attributes = 0;
         int success = ex_put_elem_block(genesisFileId,
                                         currentElementBlockID,
                                         "PYRAMID",num_elements,
                                         num_nodes_per_element,
                                         num_attributes);

         // write out connectivity

         success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                    connectivity);
         delete [] connectivity;
         if (success != 0)
         {
            PRINT_INFO("Error writing Exodus failed pyramid blocks...\n");
            return VERDE_FAILURE;
         }
      }
   }
   
   return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_failed_wedge_blocks()
{
   // go through the failed wedge elements, writing a block for each

   int num_nodes_per_element = NUM_WEDGE_NODES;
   
   int number_wedge_metrics = WedgeMetric::instance()->number_metrics();
   FailedWedge *wedge;
   int *connectivity;

   for (int i = 0; i < number_wedge_metrics; i++)
   {
      // gather the wedge list for the metric
      
     std::deque<FailedWedge*> wedge_list;
     WedgeMetric::instance()->failed_wedge_list(i,wedge_list);
     int num_elements = wedge_list.size();
     
     connectivity = new int[num_elements*num_nodes_per_element];
     int index = 0;
     
     // process each wedge
            
     for (int j = 0; j < num_elements; j++)
     {
       wedge = wedge_list[j];
       elementNumberMap.push_back(wedge->original_id());
       for (int k = 0; k < NUM_WEDGE_NODES; k++)
       {
         NodeRef node_ref(wedge->node_id(k));
         connectivity[index++] = node_ref.exodus_id();
       }
     }
     
      // write out the element block
   
      if(index)
      {
         currentElementBlockID++;
         PRINT_INFO("   Writing %d failed wedges (%s) to block %d...\n",
                    num_elements,
                    WedgeMetric::instance()->metric_name(i),
                    currentElementBlockID);
         int num_attributes = 0;
         int success = ex_put_elem_block(genesisFileId,
                                         currentElementBlockID,
                                         "WEDGE",num_elements,
                                         num_nodes_per_element,
                                         num_attributes);

         // write out connectivity

         success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                    connectivity);
         delete [] connectivity;
         if (success != 0)
         {
            PRINT_INFO("Error writing Exodus failed wedge blocks...\n");
            return VERDE_FAILURE;
         }
      }
   }
   
   return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_failed_knife_blocks()
{
  // go through the failed knife elements, writing a block for each

  int num_nodes_per_element = NUM_KNIFE_NODES;
  
  int number_knife_metrics = KnifeMetric::instance()->number_metrics();
  FailedKnife *knife;
  int *connectivity;
  
  for (int i = 0; i < number_knife_metrics; i++)
  {
    // gather the knife list for the metric      
    std::deque<FailedKnife*> knife_list;
    KnifeMetric::instance()->failed_knife_list(i,knife_list);
    int num_elements = knife_list.size();
    
    connectivity = new int[num_elements*num_nodes_per_element];
    int index = 0;
    
    // process each knife
      
    for (int j=0; j<num_elements; j++)
    {
      knife = knife_list[j];
      elementNumberMap.push_back(knife->original_id());
      for (int k = 0; k < NUM_KNIFE_NODES; k++)
      {
        NodeRef node_ref(knife->node_id(k));
        connectivity[index++] = node_ref.exodus_id();
      }
    }
      
    // write out the element block
   
    if(index)
    {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d failed knifes (%s) to block %d...\n",
          num_elements,
          KnifeMetric::instance()->metric_name(i),
          currentElementBlockID);
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId,
          currentElementBlockID,
          "KNIFE",num_elements,
          num_nodes_per_element,
          num_attributes);
      
      // write out connectivity

     success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
         connectivity);
     delete [] connectivity;
     if (success != 0)
     {
       PRINT_INFO("Error writing Exodus failed knife blocks...\n");
       return VERDE_FAILURE;
     }
    }
  }
  
  return VERDE_SUCCESS;
  
}

VerdeStatus ExodusTool::write_failed_quad_blocks()
{
  // go through the failed quad elements, writing a block for each

  int num_nodes_per_element = NUM_QUAD_NODES;
  
  int number_quad_metrics = QuadMetric::instance()->number_metrics();
  FailedQuad *quad;
  int *connectivity;
  
  for (int i = 0; i < number_quad_metrics; i++)
  {
    // gather the quad list for the metric
      
    std::deque<FailedQuad*> quad_list;
    QuadMetric::instance()->failed_quad_list(i,quad_list);
    int num_elements = quad_list.size();
    
    connectivity = new int[num_elements*num_nodes_per_element];
    int index = 0;
    
    // process each quad
      
    for (int j=0; j<num_elements; j++)
    {
      quad = quad_list[j];
      elementNumberMap.push_back(quad->original_id());
      for (int k = 0; k < NUM_QUAD_NODES; k++)
      {
        NodeRef node_ref(quad->node_id(k));
        connectivity[index++] = node_ref.exodus_id();
      }
    }
    
      
    // write out the element block
       
    if(index)
    {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d failed quads (%s) to block %d...\n",
          num_elements,
          QuadMetric::instance()->metric_name(i),
          currentElementBlockID);
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId,
          currentElementBlockID,
          "QUADRA",num_elements,
          num_nodes_per_element,
          num_attributes);
      
      // write out connectivity

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
          connectivity);
      delete [] connectivity;
      if (success != 0)
      {
        PRINT_INFO("Error writing Exodus failed quad blocks...\n");
        return VERDE_FAILURE;
      }
    }
  }
  
  return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_failed_tri_blocks()
{
   // go through the failed tri elements, writing a block for each

   int num_nodes_per_element = NUM_TRI_NODES;
   
   int number_tri_metrics = TriMetric::instance()->number_metrics();
   FailedTri *tri;
   int *connectivity;

   for (int i = 0; i < number_tri_metrics; i++)
   {
      // gather the tri list for the metric
      
     std::deque<FailedTri*> tri_list;
     TriMetric::instance()->failed_tri_list(i,tri_list);
     int num_elements = tri_list.size();
     
     connectivity = new int[num_elements*num_nodes_per_element];
     int index = 0;
     
     // process each tri
      
     for (int j = 0; j < num_elements; j++)
     {
       tri = tri_list[j];
       elementNumberMap.push_back(tri->original_id());
       for (int k = 0; k < NUM_TRI_NODES; k++)
       {
         NodeRef node_ref(tri->node_id(k));
         connectivity[index++] = node_ref.exodus_id();
       }
     }
      
      // write out the element block
   
      if(index)
      {
         currentElementBlockID++;
         PRINT_INFO("   Writing %d failed tris (%s) to block %d...\n",
                    num_elements,
                    TriMetric::instance()->metric_name(i),
                    currentElementBlockID);
         int num_attributes = 0;
         int success = ex_put_elem_block(genesisFileId,
                                         currentElementBlockID,
                                         "TRI",num_elements,
                                         num_nodes_per_element,
                                         num_attributes);

         // write out connectivity

         success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                    connectivity);
         delete [] connectivity;
         if (success != 0)
         {
            PRINT_INFO("Error writing Exodus failed tri blocks...\n");
            return VERDE_FAILURE;
         }
      }
   }
   
   return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_failed_edge_blocks()
{
  // go through the failed edge elements, writing a block for each

  int num_nodes_per_element = NUM_EDGE_NODES;
  
  int number_edge_metrics = EdgeMetric::instance()->number_metrics();
  FailedEdge *edge;
  int *connectivity;
  
  for (int i = 0; i < number_edge_metrics; i++)
  {
    // gather the edge list for the metric
      
    std::deque<FailedEdge*> edge_list;
    EdgeMetric::instance()->failed_edge_list(i,edge_list);
    int num_elements = edge_list.size();
    
    connectivity = new int[num_elements*num_nodes_per_element];
    int index = 0;
    
    // process each edge
      
    for (int j=0; j < num_elements; j++)
    {
      edge = edge_list[j];
      elementNumberMap.push_back(edge->original_id());
      for (int k = 0; k < NUM_EDGE_NODES; k++)
      {
        NodeRef node_ref(edge->node_id(k));
        connectivity[index++] = node_ref.exodus_id();
      }
    }
      
    // write out the element block
   
    if(index)
    {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d failed edges (%s) to block %d...\n",
          num_elements,
          EdgeMetric::instance()->metric_name(i),
          currentElementBlockID);
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId,
          currentElementBlockID,
          "BAR2",num_elements,
          num_nodes_per_element,
          num_attributes);
      
      // write out connectivity

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
          connectivity);
      delete [] connectivity;
      if (success != 0)
      {
        PRINT_INFO("Error writing Exodus failed edge blocks...\n");
        return VERDE_FAILURE;
      }
    }
  }
  
  return VERDE_SUCCESS;
}

VerdeStatus ExodusTool::write_model_edge_blocks()
{

   // first, write the coincident edge block so it overlaps 

  std::deque<EdgeElem*> edge_list;
   EdgeTool::instance()->coincident_edges(edge_list);


   // write out the element block parameters

   int number_edges = edge_list.size();
   
   if(number_edges)
   {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d coincident model edges to block %d...\n",
                 number_edges, currentElementBlockID);
      int num_nodes_per_element = 2;
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId, currentElementBlockID,
                                      "BEAM",number_edges,
                                      num_nodes_per_element,
                                      num_attributes);

      // write out edge elements
      
      int *conn = new int[number_edges*num_nodes_per_element];
      int conn_index = 0;
      EdgeElem *edge;
      for(int i = 0; i < number_edges; i++)
      {
         edge = edge_list[i];
         elementNumberMap.push_back(0);
         NodeRef *node_ref = new NodeRef(edge->node_id(0));
         conn[conn_index++] = node_ref->exodus_id();
         delete node_ref;
         node_ref = new NodeRef(edge->node_id(1));
         conn[conn_index++] = node_ref->exodus_id();
      }
      

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                 conn);

      delete [] conn;
      
      if (success != 0)
      {
         PRINT_INFO("Error writing Exodus coincident model edges...\n");
         return VERDE_FAILURE;
      }
   }

   // next, write the identical edge block 

   edge_list.clear();
   //EdgeTool::instance()->identical_edges(edge_list);


   // write out the element block parameters

   number_edges = edge_list.size();
   
   if(number_edges)
   {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d identical model edges to block %d...\n",
                 number_edges, currentElementBlockID);
      int num_nodes_per_element = 2;
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId, currentElementBlockID,
                                      "BEAM",number_edges,
                                      num_nodes_per_element,
                                      num_attributes);

      // write out edge elements
      
      int *conn = new int[number_edges*num_nodes_per_element];
      int conn_index = 0;
      EdgeElem *edge;
      for(int i = 0; i < number_edges; i++)
      {
         edge = edge_list[i];
         elementNumberMap.push_back(0);
         NodeRef *node_ref = new NodeRef(edge->node_id(0));
         conn[conn_index++] = node_ref->exodus_id();
         delete node_ref;
         node_ref = new NodeRef(edge->node_id(1));
         conn[conn_index++] = node_ref->exodus_id();
      }
      

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                 conn);

      delete [] conn;
      
      if (success != 0)
      {
         PRINT_INFO("Error writing Exodus coincident model edges...\n");
         return VERDE_FAILURE;
      }
   }
   
   
   // get the model edge information
   edge_list.clear();
   EdgeTool::instance()->exterior_edges(edge_list);
   

   // write out the element block parameters

   number_edges = edge_list.size();
   
   if(number_edges)
   {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d model edges to block %d...\n",
                 number_edges, currentElementBlockID);
      int num_nodes_per_element = 2;
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId, currentElementBlockID,
                                      "BEAM",number_edges,
                                      num_nodes_per_element,
                                      num_attributes);

      // write out edge elements
      
      int *conn = new int[number_edges*num_nodes_per_element];
      EdgeElem *edge;
      int conn_index = 0;
      for(int i = 0; i < number_edges; i++)
      {
         edge = edge_list[i];
         elementNumberMap.push_back(0);
         NodeRef *node_ref = new NodeRef(edge->node_id(0));
         conn[conn_index++] = node_ref->exodus_id();
         delete node_ref;
         node_ref = new NodeRef(edge->node_id(1));
         conn[conn_index++] = node_ref->exodus_id();
      }
      

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                 conn);

      delete [] conn;
      
      if (success != 0)
      {
         PRINT_INFO("Error writing Exodus model edges...\n");
         return VERDE_FAILURE;
      }
   }

   
   return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_coincident_element_blocks()
{

   // write out the element block parameters for quads

  std::deque<QuadElem*> quad_list;
  SkinTool::coincident_quads(quad_list);
   
   if(quad_list.size())
   {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d coincident quads to block %d...\n",
                 quad_list.size(), currentElementBlockID);
      int num_nodes_per_element = 4;
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId, currentElementBlockID,
                                      "QUAD4",quad_list.size(),
                                      num_nodes_per_element,
                                      num_attributes);
      // gather connectivity
      
      int *conn = new int[quad_list.size()*num_nodes_per_element];
      QuadElem *quad;
      int conn_index = 0;
      for(unsigned int i=0; i<quad_list.size(); i++)
      {
         quad = quad_list[i];
         elementNumberMap.push_back(0);
         for (int j = 0; j < NUM_QUAD_NODES; j++)
         {
            NodeRef *node_ref = new NodeRef(quad->node_id(j));
            conn[conn_index++] = node_ref->exodus_id();
            delete node_ref;
         }
      }

      // write out quad elements

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                 conn);

      if (success != 0)
      {
         PRINT_INFO("Error writing Exodus surface quads...\n");
         return VERDE_FAILURE;
      }
   }

   // write out the element block parameters for tris

   std::deque<TriElem*> tri_list;
   SkinTool::coincident_tris(tri_list);
   
   if(tri_list.size())
   {
      currentElementBlockID++;
      PRINT_INFO("   Writing %d coincident tris to block %d...\n",
                 tri_list.size(), currentElementBlockID);
      int num_nodes_per_element = 3;
      int num_attributes = 0;
      int success = ex_put_elem_block(genesisFileId, currentElementBlockID,
                                      "TRI",tri_list.size(),
                                      num_nodes_per_element,
                                      num_attributes);
      // gather connectivity
      
      int *conn = new int[tri_list.size()*num_nodes_per_element];
      TriElem *tri;
      int conn_index = 0;
      for(unsigned int i = 0; i < tri_list.size(); i++ )
      {
         tri = tri_list[i];
         elementNumberMap.push_back(0);
         for (int j = 0; j < NUM_TRI_NODES; j++)
         {
            NodeRef *node_ref = new NodeRef(tri->node_id(j));
            conn[conn_index++] = node_ref->exodus_id();
            delete node_ref;
         }
      }

      // write out tri elements

      success = ex_put_elem_conn(genesisFileId,currentElementBlockID,
                                 conn);

      if (success != 0)
      {
         PRINT_INFO("Error writing Exodus surface tris...\n");
         return VERDE_FAILURE;
      }
   }
   
    return VERDE_SUCCESS;

}

void ExodusTool::gather_skin_data()
{
      
   // gather the current model skin of quads

   SkinTool::exterior_quads(exteriorQuads);
   numberQuadElements = exteriorQuads.size();
   if (numberQuadElements)
   {
      numElementBlocks++;
      totalElements += numberQuadElements;
   }

   // gather up a unique list of exterior nodes (by NodeRef)
   // at the same time, make the initial connectivity array for quads
   
   QuadElem *quad;
   quadConnectivity = new int[exteriorQuads.size()*4];
   int conn_index = 0;

   unsigned int i;
   for ( i=0; i<exteriorQuads.size(); i++)
   {
      quad = exteriorQuads[i];
      elementNumberMap.push_back(0);
      for (int j = 0; j < NUM_QUAD_NODES; j++)
      {
         NodeRef *node_ref = new NodeRef(quad->node_id(j));
         quadConnectivity[conn_index++] = node_ref->index();
         if (node_ref->is_marked() == VERDE_FALSE)
         {
            exteriorNodes.push_back(node_ref);
            node_ref->mark();
         }
         else
         {
            delete node_ref;
         }
      }
   }

   // gather the current model skin of tris

   SkinTool::exterior_tris(exteriorTris);
   numberTriElements = exteriorTris.size();
   if (numberTriElements)
   {
      numElementBlocks++;
      totalElements += numberTriElements;
   }
   

   // gather up a unique list of exterior nodes (by NodeRef)
   // at the same time, make the initial connectivity array for tris
   
   TriElem *tri;
   triConnectivity = new int[exteriorTris.size()*3];
   conn_index = 0;

   for (i=0; i<exteriorTris.size(); i++)
   {
      tri = exteriorTris[i];
      elementNumberMap.push_back(0);
      for (int j = 0; j < NUM_TRI_NODES; j++)
      {
         NodeRef *node_ref = new NodeRef(tri->node_id(j));
         triConnectivity[conn_index++] = node_ref->index();
         if (node_ref->is_marked() == VERDE_FALSE)
         {
            exteriorNodes.push_back(node_ref);
            node_ref->mark();
         }
         else
         {
            delete node_ref;
         }
      }
   }
}

void ExodusTool::gather_edge_data(VerdeBoolean do_skin)
{
   EdgeElem *edge = NULL;
      
      // handle model edges
      
   std::deque<EdgeElem*> edge_list;
   EdgeTool::instance()->exterior_edges(edge_list);
   if (edge_list.size())
   {
      numElementBlocks++;
      totalElements += edge_list.size();
      if (!do_skin)
      {
         // gather nodes from edges in this case
         for (unsigned int i=0; i<edge_list.size(); i++)
         {
            edge = edge_list[i];
            for (int j = 0; j < NUM_EDGE_NODES; j++)
            {
               NodeRef *node_ref = new NodeRef(edge->node_id(j));
               if (node_ref->is_marked() == VERDE_FALSE)
               {
                  exteriorNodes.push_back(node_ref);
                  node_ref->mark();
               }
               else
               {
                  delete node_ref;
               }
            }
         }
      }
   }

   // Do coincident Edges
      
   edge_list.clear();
   EdgeTool::instance()->coincident_edges(edge_list);
   if (edge_list.size())
   {
      numElementBlocks++;
      totalElements += edge_list.size();
      if (!do_skin)
      {
         // gather nodes from edges in this case
         for (int i = edge_list.size(); i>0; i--)
         {
            for (int j = 0; j < NUM_EDGE_NODES; j++)
            {
               NodeRef *node_ref = new NodeRef(edge->node_id(j));
               if (node_ref->is_marked() == VERDE_FALSE)
               {
                  exteriorNodes.push_back(node_ref);
                  node_ref->mark();
               }
               else
               {
                  delete node_ref;
               }
            }
         }
      }
   }

   // Do Identical Edges
      
   edge_list.clear();
   //EdgeTool::instance()->identical_edges(edge_list);
   if (edge_list.size())
   {
      numElementBlocks++;
      totalElements += edge_list.size();
      if (!do_skin)
      {
         // gather nodes from edges in this case
         for (unsigned int i=0; i<edge_list.size(); i++)
         {
            edge = edge_list[i];
            for (int j = 0; j < NUM_EDGE_NODES; j++)
            {
               NodeRef *node_ref = new NodeRef(edge->node_id(j));
               if (node_ref->is_marked() == VERDE_FALSE)
               {
                  exteriorNodes.push_back(node_ref);
                  node_ref->mark();
               }
               else
               {
                  delete node_ref;
               }
            }
         }
      }     
   }
}

void ExodusTool::gather_coincident_data(VerdeBoolean do_skin)
{
   
   // Do Coincident Quads

  std::deque<QuadElem*> quad_list;
   SkinTool::coincident_quads(quad_list);
   if (quad_list.size())
   {
      numElementBlocks++;
      totalElements += quad_list.size();
      if (!do_skin)
      {
         QuadElem *quad;
         // gather nodes from quads in this case
         for (unsigned int i=0; i<quad_list.size(); i++)
         {
            quad = quad_list[i];
            for (int j = 0; j < NUM_QUAD_NODES; j++)
            {
               NodeRef *node_ref = new NodeRef(quad->node_id(j));
               if (node_ref->is_marked() == VERDE_FALSE)
               {
                  exteriorNodes.push_back(node_ref);
                  node_ref->mark();
               }
               else
               {
                  delete node_ref;
               }
            }
         }
      }
   }
      
      
   // Do Coincident Tris

   std::deque<TriElem*> tri_list;
   SkinTool::coincident_tris(tri_list);
   if (tri_list.size())
   {
     numElementBlocks++;
     totalElements += tri_list.size();
     if (!do_skin)
     {
       TriElem *tri;
       // gather nodes from tris in this case
       for (unsigned int i = 0; i < tri_list.size(); i++)
       {
         tri = tri_list[i];
         for (int j = 0; j < NUM_TRI_NODES; j++)
         {
           NodeRef *node_ref = new NodeRef(tri->node_id(j));
           if (node_ref->is_marked() == VERDE_FALSE)
           {
             exteriorNodes.push_back(node_ref);
             node_ref->mark();
           }
           else
           {
             delete node_ref;
           }
         }
       }
     }     
   }
}

void ExodusTool::gather_failed_data()
{
  // gather up any additional nodes from failed hexes to be output

  int number_hex_metrics = HexMetric::instance()->number_metrics();
  FailedHex *hex;
  
  int i;
  for ( i = 0; i < number_hex_metrics; i++)
  {
    // gather the hex list for the metric
      
    std::deque<FailedHex*> hex_list;
    HexMetric::instance()->failed_hex_list(i,hex_list);
    
    if (hex_list.size())
    {
      numElementBlocks++;
      totalElements += hex_list.size();
    }
    
    // process each hex
      
    for (unsigned int j=0; j< hex_list.size(); j++)
    {
      hex = hex_list[j];
      for (int k = 0; k < NUM_HEX_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(hex->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  
  // gather up any additional nodes from failed tets to be output

  int number_tet_metrics = TetMetric::instance()->number_metrics();
  FailedTet *tet;
  
  for (i = 0; i < number_tet_metrics; i++)
  {
    // gather the tet list for the metric
      
    std::deque<FailedTet*> tet_list;
    TetMetric::instance()->failed_tet_list(i,tet_list);
    
    if (tet_list.size())
    {
      numElementBlocks++;
      totalElements += tet_list.size();
    }
    
    // process each tet      
    for (unsigned int j=0; j<tet_list.size(); j++)
    {
      tet = tet_list[j];
      for (int k = 0; k < NUM_TET_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(tet->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  
  // gather up any additional nodes from failed pyramids to be output

  int number_pyramid_metrics = PyramidMetric::instance()->number_metrics();
  FailedPyramid *pyramid;
  
  for (i = 0; i < number_pyramid_metrics; i++)
  {
    // gather the pyramid list for the metric      
    std::deque<FailedPyramid*> pyramid_list;
    PyramidMetric::instance()->failed_pyramid_list(i,pyramid_list);
    
    if (pyramid_list.size())
    {
      numElementBlocks++;
      totalElements += pyramid_list.size();
    }
    
    // process each pyramid
      
    for (unsigned int j=0; j< pyramid_list.size(); j++)
    {
      pyramid = pyramid_list[j];
      for (int k = 0; k < NUM_PYRAMID_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(pyramid->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  
  // gather up any additional nodes from failed wedges to be output

  int number_wedge_metrics = WedgeMetric::instance()->number_metrics();
  FailedWedge *wedge;
  
  for (i = 0; i < number_wedge_metrics; i++)
  {
    // gather the wedge list for the metric
      
    std::deque<FailedWedge*> wedge_list;
    WedgeMetric::instance()->failed_wedge_list(i,wedge_list);
     
    if (wedge_list.size())
    {
      numElementBlocks++;
      totalElements += wedge_list.size();
    }
    
    // process each wedge
      
    for (unsigned int j = 0; j < wedge_list.size(); j++)
    {
      wedge = wedge_list[j];
      for (int k = 0; k < NUM_WEDGE_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(wedge->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  
  // gather up any additional nodes from failed knives to be output

  int number_knife_metrics = KnifeMetric::instance()->number_metrics();
  FailedKnife *knife;
  
  for (i = 0; i < number_knife_metrics; i++)
  {
    // gather the knife list for the metric
    std::deque<FailedKnife*> knife_list;
    KnifeMetric::instance()->failed_knife_list(i,knife_list);
    
    if (knife_list.size())
    {
      numElementBlocks++;
      totalElements += knife_list.size();
    }
    
    // process each knife
      
    for (unsigned int j=0; j<knife_list.size(); j++)
    {
      knife = knife_list[j];
      for (int k = 0; k < NUM_KNIFE_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(knife->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  // gather up any additional nodes from failed quads to be output

  int number_quad_metrics = QuadMetric::instance()->number_metrics();
  FailedQuad *quad;
  
  for (i = 0; i < number_quad_metrics; i++)
  {
    // gather the quad list for the metric
          
    std::deque<FailedQuad*> quad_list;
    QuadMetric::instance()->failed_quad_list(i,quad_list);
    
    if (quad_list.size())
    {
      numElementBlocks++;
      totalElements += quad_list.size();
    }
    
    // process each quad
      
    for (unsigned int j=0; j<quad_list.size(); j++)
    {
      quad = quad_list[j];
      for (int k = 0; k < NUM_QUAD_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(quad->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  
  // gather up any additional nodes from failed tris to be output

  int number_tri_metrics = TriMetric::instance()->number_metrics();
  FailedTri *tri;
  
  for (i = 0; i < number_tri_metrics; i++)
  {
    // gather the tri list for the metric
      
    std::deque<FailedTri*> tri_list;
    TriMetric::instance()->failed_tri_list(i,tri_list);
    
    if (tri_list.size())
    {
      numElementBlocks++;
      totalElements += tri_list.size();
    }
    
    // process each tri
      
    for (unsigned int j = 0; j < tri_list.size(); j++)
    {
      tri = tri_list[j];
      for (int k = 0; k < NUM_TRI_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(tri->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  
  // gather up any additional nodes from failed edges to be output

  int number_edge_metrics = EdgeMetric::instance()->number_metrics();
  FailedEdge *edge;
  
  for (i = 0; i < number_edge_metrics; i++)
  {
    // gather the edge list for the metric
      
    std::deque<FailedEdge*> edge_list;
    EdgeMetric::instance()->failed_edge_list(i,edge_list);
    
    if (edge_list.size())
    {
      numElementBlocks++;
      totalElements += edge_list.size();
    }
    
    // process each edge

    for (unsigned int j=0; j < edge_list.size(); j++)
    {
      edge = edge_list[j];
      for (int k = 0; k < NUM_EDGE_NODES; k++)
      {
        NodeRef *node_ref = new NodeRef(edge->node_id(k));
        if (node_ref->is_marked() == VERDE_FALSE)
        {
          exteriorNodes.push_back(node_ref);
          node_ref->mark();
        }
        else
        {
          delete node_ref;
        }
      }
    }
  }
  
}


VerdeStatus ExodusTool::write_failed_element_blocks()
{
   VerdeStatus success;

   success = write_failed_hex_blocks();
   success = write_failed_tet_blocks();
   success = write_failed_pyramid_blocks();
   success = write_failed_wedge_blocks();
   success = write_failed_knife_blocks();
   success = write_failed_quad_blocks();
   success = write_failed_tri_blocks();
   success = write_failed_edge_blocks();

   return success;
   
}

VerdeStatus ExodusTool::write_element_number_map()
{
  // convert the element number map list to an array

  assert(elementNumberMap.size() == (unsigned int)totalElements);
  
  int *map = new int [totalElements];
  
  for(int i = 0; i < totalElements; i++)
  {
    map[i] = elementNumberMap[i];
  }
  
  // write out to exodus

  int error = ex_put_elem_num_map(genesisFileId,map);

  delete [] map;
  
  if(error < 0)
  {
    PRINT_ERROR("Failed writing exodus element number map.\n"  );
    return VERDE_FAILURE;
  }

  return VERDE_SUCCESS;
  
}


VerdeStatus ExodusTool::write_elementblocks(ExodusMeshInfo& mesh_info)
{
  std::deque<ElementBlock*> blocks;
  mesh_info.mesh->element_blocks(blocks);

  unsigned int i;
  int block_index = 0;  // index into block list, may != 1 if there are inactive blocks
  
  for(i=0; i< blocks.size(); i++)
  {
    ElementBlock* block = blocks[i];
    if(!block || block->is_active() == VERDE_FALSE)
      continue;

    MeshContainer* elem_cont = block->mesh_container();

    unsigned int num_nodes_per_elem = ElemRef(*(block->mesh_container()),
                                       block->element_offset()+1).num_nodes();

    // write out the id for the block

    int id = block->block_id();
    int num_values = 1;

    VerdeStatus result = write_exodus_integer_variable(genesisFileId, VAR_ID_EL_BLK,
                                                       &id, block_index, num_values);

    if (result != VERDE_SUCCESS)
    {
      PRINT_ERROR("Problem writing element block id %d for file %d\n", 
                  id, genesisFileId);
    }
    
    // write out the block status

    int status = 1;
    if (!block->number_elements())
      status = 0;

    result = write_exodus_integer_variable(genesisFileId, VAR_STAT_EL_BLK,
                                           &status, block_index, num_values);

    if (result != VERDE_SUCCESS)
    {
      PRINT_ERROR("Problem writing element block status for file %d\n", 
                  genesisFileId);
    }

    // map the connectivity to the new nodes
    const unsigned int num_elem = block->number_elements();
    const unsigned int num_nodes = num_nodes_per_elem * num_elem; 
    int* connectivity = new int[num_nodes];

    // go through each node and find out the new connectivity
    unsigned int j,k,l=0;
    unsigned int old_id, new_id;
    for(j=1; j<=num_elem; j++)
    {
      for(k=0; k<num_nodes_per_elem; k++)
      {
        old_id = elem_cont->node_id(j, k);
        new_id = NodeRef(old_id).exodus_id();
        connectivity[l++] = new_id;
      }
    }

    // write out the connectivity 
    int error = ex_put_elem_conn(genesisFileId, block->block_id(), connectivity);       

    // if there are attributes, write these out also

    if (elem_cont->number_attributes())
    {
      error = ex_put_elem_attr(genesisFileId, block->block_id(),
                               elem_cont->attribute_data());
    }

  block_index++;
  delete [] connectivity;
    
        
  }

  return VERDE_SUCCESS;
}


VerdeStatus ExodusTool::write_elementmap(ExodusMeshInfo& mesh_info)
{
  // convert the element number map list to an array

  int *map = new int [mesh_info.num_elements];
  
  for(unsigned int i = 0; i < mesh_info.num_elements; i++)
  {
    map[i] = i+1;
  }
  
  // write out to exodus

  int error = ex_put_elem_num_map(genesisFileId,map);

  delete [] map;
  
  if(error < 0)
  {
    PRINT_ERROR("Failed writing exodus element number map.\n"  );
    return VERDE_FAILURE;
  }

  return VERDE_SUCCESS;

}

VerdeStatus ExodusTool::write_BCs(ExodusMeshInfo& mesh_info )
{
  //get all node bcs
  std::deque<NodeBC*> node_bc_list;
  std::deque<ElementBC*> element_bc_list;
  mesh_info.mesh->get_node_bc_list(node_bc_list);
  mesh_info.mesh->get_element_bc_list(element_bc_list);
  int* list_id_ptr;
 
  unsigned int i;
  int j;   

  for( i=0; i<node_bc_list.size(); i++ )
  { 
    NodeBC* node_bc = node_bc_list[i];
    if(node_bc->is_active())
    {
      //get the node set id 
      int node_set_id = node_bc->id();  
 
      //get number of nodes in set
      int number_nodes = node_bc->number_nodes();

      //get node array
      list_id_ptr = node_bc->get_nodeIds();
       
      //build new array to old exodus ids
      int * exodus_id_array = new int[number_nodes];

      int incrementor = 0;

      //iterate through nodes
      for( j=0; j<number_nodes; j++ )
      {
         int id = list_id_ptr[j];
         NodeRef node(id);

	     if(node.is_marked())
         {
	       // if it's marked, it's added
           exodus_id_array[incrementor++] = node.exodus_id();
         }	
      }


      if(incrementor)
      {
        // write out the id for the nodeset

        int num_values = 1;
    
        VerdeStatus result = write_exodus_integer_variable(genesisFileId, 
                                                           VAR_NS_IDS,
                                                           &node_set_id, 
                                                           i, num_values);

        if (result != VERDE_SUCCESS)
        {
          PRINT_ERROR("Problem writing node set id %d for file %d\n", 
                      node_set_id, genesisFileId);
        }
    
        // write out the nodeset status

        int status = 1;
        if (!number_nodes)
          status = 0;

        result = write_exodus_integer_variable(genesisFileId, VAR_NS_STAT,
                                               &status, i, num_values);

        if (result != VERDE_SUCCESS)
        {
          PRINT_ERROR("Problem writing node set status for file %d\n", 
                      genesisFileId);
        }

        //write it out
        ex_put_node_set(genesisFileId, node_set_id, exodus_id_array );

        // write out distribution factors
        
        if (node_bc->number_dist_factors())
        {
          double *dist_factors = new double[number_nodes];
          node_bc->active_dist_factor_data(dist_factors);
          ex_put_node_set_dist_fact( genesisFileId, node_set_id,
                                     dist_factors);
          delete [] dist_factors;
        }
      }
      
      delete [] exodus_id_array;
    }
  }


  //now do element bcs
  for( i=0; i<element_bc_list.size(); i++)
  { 
    ElementBC* element_bc = element_bc_list[i];
    if(element_bc->is_active())
    {
      //get the element set id 
      int side_set_id = element_bc->id();  
 
      //get number of sidesets in set
      int number_elements = element_bc->number_elements();

      //get element array
      list_id_ptr = element_bc->get_elementIds();
      int * output_element_ids = new int[number_elements];
      int * output_element_side_numbers= new int[number_elements];

      int * side_set_side_list = element_bc->get_elementSides();      
 
      //build new array to old exodus ids and side list
      int * exodus_id_array = new int[number_elements];

      int incrementor = 0;

      //iterate through sidesets 
      for( j=0; j<number_elements; j++ )
      {
         //get original id of element
         int id = list_id_ptr[j];

         //get block element 
         ElementBlock* block = mesh_info.mesh->find_block_for_element(id); 

         //get out if block of element is not active
         if( !block->is_active() )
           continue;

         //get pointer to mesh cont.
         MeshContainer* mesh_cont = block->mesh_container();

         //build ElemRef with MC pointer and id
         ElemRef elem(*mesh_cont,id-(block->element_offset()) );
         
         output_element_ids[incrementor] = elem.exodus_id(); 
         output_element_side_numbers[incrementor++] = side_set_side_list[j]; 
         
      }

      if(incrementor)
      { 
        // write out the id for the nodeset

        int num_values = 1;
    
        VerdeStatus result = write_exodus_integer_variable(genesisFileId, 
                                                           VAR_SS_IDS,
                                                           &side_set_id, 
                                                           i, num_values);

        if (result != VERDE_SUCCESS)
        {
          PRINT_ERROR("Problem writing side set id %d for file %d\n", 
                      side_set_id, genesisFileId);
        }
    
        // write out the nodeset status

        int status = 1;
        if (!number_elements)
          status = 0;

        result = write_exodus_integer_variable(genesisFileId, VAR_SS_STAT,
                                               &status, i, num_values);

        if (result != VERDE_SUCCESS)
        {
          PRINT_ERROR("Problem writing side set status for file %d\n", 
                      genesisFileId);
        }

       //write it Element bcs 
        ex_put_side_set(genesisFileId, side_set_id, output_element_ids, 
                        output_element_side_numbers );

        // write the distribution factors

        if (element_bc->number_dist_factors())
        {
          double *dist_factors = new double[number_elements];
          element_bc->active_dist_factor_data(mesh_info.mesh,dist_factors);
          ex_put_side_set_dist_fact( genesisFileId, side_set_id,
                                     dist_factors);
          delete [] dist_factors;
        }


      }
      
      delete [] exodus_id_array;
      delete [] output_element_ids;
      delete [] output_element_side_numbers;
    }
  }


  return VERDE_SUCCESS;
}


int ExodusTool::initialize_exodus_file(int genesis_file_id, 
                                       ExodusMeshInfo &mesh_info,
                                       std::deque<BlockData> &block_data,
                                       std::deque<SideSetData> & sideset_data,
                                       std::deque<NodeSetData> & nodeset_data,
                                       const char* title_string,
                                       bool write_maps,
                                       bool write_sideset_distribution_factors)
{

  // This routine takes the place of the exodusii routine ex_put_init,
  // and additionally pre-defines variables such as qa, element blocks,
  // sidesets and nodesets in a single pass.
  //
  // This is necessary because of the way exodusII works.  Each time the
  // netcdf routine endef is called to take the file out of define mode,
  // the entire file is copied before the new information is added.
  //
  // With very large files, this is simply not workable.  This routine takes
  // the definition portions of all applicable exodus routines and puts them
  // in a single definition, preventing repeated copying of the file.
  //
  // Most of the code is copied directly from the applicable exodus routine,
  // and thus this routine may not seem very consistent in usage or variable
  // naming, etc.

  // perform the initializations in ex_put_init

   int numdimdim, numnoddim, elblkdim, strdim, dim[2], dimid, varid;
   int header_size, fixed_var_size, iows;

   int element_block_index, /*elem_blk_stat,*/ numelbdim, nelnoddim;
   int numattrdim, connid;
   //int num_attributes = 0;

   exerrval = 0; /* clear error code */

   if ((ncdimid (genesis_file_id, DIM_NUM_DIM)) != -1)
   {
     exerrval = EX_MSG;
     PRINT_ERROR("initialization already done for file id %d\n",genesis_file_id);
     return (EX_FATAL);
   }


   // inquire on defined string dimension and general dimension for qa

   if ((strdim = ncdimid (genesis_file_id, DIM_STR)) < 0)
   {
     exerrval = ncerr;
     PRINT_ERROR(
             "failed to get string length in file id %d\n",genesis_file_id);
     //ex_err("ex_put_init",errmsg,exerrval);
     return (EX_FATAL);         
   }

   int n4dim;
   if ((n4dim = ncdimid (genesis_file_id, DIM_N4)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
             "failed to locate record length in file id %d\n", genesis_file_id);
     //ex_err("ex_put_qa",errmsg,exerrval);
     return (EX_FATAL);
   }



/* put file into define mode */

   if (ncredef (genesis_file_id) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
            "failed to put file id %d into define mode\n", genesis_file_id);
     //ex_err("ex_put_init",errmsg,exerrval);
     return (EX_FATAL);
   }


   // it is possible that an NT filename using backslashes is in the title string
   // this can give fits to unix codes where the backslash is assumed to be an escape
   // sequence.  For the exodus file, just flip the backslash to a slash to prevent
   // this problem

   // get a working copy of the title_string;

   char working_title[80];
   strncpy(working_title,title_string,79);
   
   int length = strlen(working_title);
   for(int pos = 0; pos < length; pos++)
   {
	   if (working_title[pos] == '\\')
		   strncpy(&working_title[pos],"/",1);
   }


   if (ncattput (genesis_file_id, NC_GLOBAL, CONST_CHAR ATT_TITLE, 
                 NC_CHAR, strlen(working_title)+1, 
                 (void *)working_title) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
            "failed to define title attribute to file id %d\n", genesis_file_id);
     return (EX_FATAL);         
   }

   // set up number of dimensions

   if ((numdimdim = ncdimdef (genesis_file_id, DIM_NUM_DIM, 
                              (long)mesh_info.num_dim)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
            "failed to define number of dimensions in file id %d\n",genesis_file_id);
     return (EX_FATAL);         
   }

   if ((numnoddim = ncdimdef (genesis_file_id, DIM_NUM_NODES, 
                              (long)mesh_info.num_nodes)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
            "failed to define number of nodes in file id %d\n",genesis_file_id);
     return (EX_FATAL);         
   }

   int numelemdim;
   
   if ((numelemdim = ncdimdef (genesis_file_id, DIM_NUM_ELEM, 
                               (long)mesh_info.num_elements)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
            "failed to define number of elements in file id %d\n",genesis_file_id);
     return (EX_FATAL);         
   }


   if ((elblkdim = ncdimdef (genesis_file_id, DIM_NUM_EL_BLK, 
                             (long)mesh_info.num_elementblocks)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
            "failed to define number of element blocks in file id %d\n",
             genesis_file_id);
     return (EX_FATAL);         
   }

/* ...and some variables */

   /* element block id status array */

   dim[0] = elblkdim;
   if ((varid = ncvardef (genesis_file_id, VAR_STAT_EL_BLK, NC_LONG, 1, dim)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
      "failed to define element block status array in file id %d\n",genesis_file_id);
     return (EX_FATAL);         
   }



   /* element block id array */

   if ((varid = ncvardef (genesis_file_id, VAR_ID_EL_BLK, NC_LONG, 1, dim)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
          "failed to define element block id array in file id %d\n",genesis_file_id);
     return (EX_FATAL);         
   }

   /*   store property name as attribute of property array variable */
     if ((ncattput (genesis_file_id, varid, ATT_PROP_NAME, NC_CHAR, 3, "ID")) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
          "failed to store element block property name %s in file id %d\n",
              "ID",genesis_file_id);
       return (EX_FATAL);
     }

     // define element blocks


     for (unsigned int i = 0; i < block_data.size(); i++)
     {
       BlockData block = block_data[i];
       
       element_block_index = i+1;
       
       /* define number of elements in this block */
          
       if ((numelbdim = ncdimdef (genesis_file_id,
                                  DIM_NUM_EL_IN_BLK(element_block_index), 
                                  (long)block.number_elements)) == -1)
       {
         if (ncerr == NC_ENAMEINUSE)	/* duplicate entry */
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "element block %d already defined in file id %d\n",
                   block.id,genesis_file_id);
         }
         else
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "failed to define number of elements/block for block %d file id %d\n",
                   i+1,genesis_file_id);
         }
         return (EX_FATAL);         
       }
       

       /* define number of nodes per element for this block */

       if ((nelnoddim = ncdimdef (genesis_file_id,
                                  DIM_NUM_NOD_PER_EL(element_block_index), 
                                  (long)block.number_nodes_per_element)) == -1)
       {
         exerrval = ncerr;
         PRINT_ERROR(
                 "failed to define number of nodes/element for block %d in file id %d\n",
                 block.id,genesis_file_id);
         return (EX_FATAL);         
       }

       /* define element attribute array for this block */

       if (block.number_attributes > 0)
       {

         if ((numattrdim = ncdimdef (genesis_file_id, 
                                     DIM_NUM_ATT_IN_BLK(element_block_index), 
                                     (long)block.number_attributes)) == -1)
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "failed to define number of attributes in block %d in file id %d\n",
                   block.id,genesis_file_id);
           return (EX_FATAL);         
         }

         dim[0] = numelbdim;
         dim[1] = numattrdim;

         if ((ncvardef (genesis_file_id, 
                        VAR_ATTRIB(element_block_index), 
                        nc_flt_code(genesis_file_id), 2, dim)) == -1)
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "failed to define attributes for element block %d in file id %d\n",
                   block.id,genesis_file_id);
           return (EX_FATAL);         
         }
       }
       

       /* define element connectivity array for this block */

       dim[0] = numelbdim;
       dim[1] = nelnoddim;

       if ((connid = ncvardef (genesis_file_id, 
                               VAR_CONN(element_block_index), 
                               NC_LONG, 2, dim)) == -1)
       {
         exerrval = ncerr;
         PRINT_ERROR(
                 "failed to create connectivity array for block %d in file id %d\n",
                 i+1,genesis_file_id);
         return (EX_FATAL);         
       }

       /* store element type as attribute of connectivity variable */

     
       if ((ncattput (genesis_file_id, connid, ATT_NAME_ELB, NC_CHAR, 
                      (block.element_type.length())+1, 
                      VOID_PTR block.element_type.c_str())) == -1)
       {
         exerrval = ncerr;
         PRINT_ERROR(
                 "failed to store element type name %s in file id %d\n",
                 block.element_type.c_str(),genesis_file_id);
         return (EX_FATAL);         
       }
     }
     
     
/* node set id array: */

   if (nodeset_data.size() > 0)
   {

     if ((dimid = ncdimdef (genesis_file_id, DIM_NUM_NS, (long)nodeset_data.size())) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
             "failed to define number of node sets in file id %d\n",genesis_file_id);
       return (EX_FATAL);         
     }

     /* node set id status array: */

     dim[0] = dimid;
     if ((varid = ncvardef (genesis_file_id, VAR_NS_STAT, NC_LONG, 1, dim)) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
        "failed to create node sets status array in file id %d\n",genesis_file_id);
       return (EX_FATAL);         
     }



     /* node set id array: */

     dim[0] = dimid;
     if ((varid = ncvardef (genesis_file_id, VAR_NS_IDS, NC_LONG, 1, dim)) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
        "failed to create node sets property array in file id %d\n",genesis_file_id);
       return (EX_FATAL);         
     }


/*   store property name as attribute of property array variable */
     if ((ncattput (genesis_file_id, varid, ATT_PROP_NAME, NC_CHAR, 3, "ID")) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
              "failed to store node set property name %s in file id %d\n",
              "ID",genesis_file_id);
       return (EX_FATAL);
     }

     // now, define the arrays needed for each node set


     int index = 0;

     for(unsigned int i = 0; i <nodeset_data.size(); i++)
     {
       NodeSetData node_set = nodeset_data[i];

       if (node_set.number_nodes == 0)
         continue;
       index++;

       if ((dimid = ncdimdef (genesis_file_id, DIM_NUM_NOD_NS(index),
                              (long)node_set.number_nodes)) == -1)
       {
         if (ncerr == NC_ENAMEINUSE)
         {
           exerrval = ncerr;
           PRINT_ERROR(
             "node set %d size already defined in file id %d\n",
             node_set.id,genesis_file_id);
         }
         else
         {
           exerrval = ncerr;
           PRINT_ERROR(
             "failed to define number of nodes for set %d in file id %d\n",
             node_set.id,genesis_file_id);
         }
         return (EX_FATAL);
       }
       
       


/* create variable array in which to store the node set node list */

       dim[0] = dimid;

       if (ncvardef(genesis_file_id, VAR_NODE_NS(index), NC_LONG,1,dim) == -1)
       {
         if (ncerr == NC_ENAMEINUSE)
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "node set %d node list already defined in file id %d\n",
                   node_set.id,genesis_file_id);
         }
         else
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "failed to create node set %d node list in file id %d\n",
                   node_set.id,genesis_file_id);
          }
         return (EX_FATAL);
       }

       // create distribution factor array
       if (ncvardef(genesis_file_id, VAR_FACT_NS(index), 
                    nc_flt_code(genesis_file_id),1,dim) == -1)
       {
         if (ncerr == NC_ENAMEINUSE)
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "node set %d node set distribution factor "
                   "list already defined in file id %d\n",
                   node_set.id,genesis_file_id);
         }
         else
         {
           exerrval = ncerr;
           PRINT_ERROR(
                   "failed to create node set %d distribution "
                   "factor list in file id %d\n",
                   node_set.id,genesis_file_id);
          }
         return (EX_FATAL);
       }

     }

   }

/* side set id array: */

   if (sideset_data.size() > 0) {

     if ((dimid = ncdimdef (genesis_file_id, DIM_NUM_SS , (long)sideset_data.size())) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
         "failed to define number of side sets in file id %d\n",genesis_file_id);
       return (EX_FATAL);         
     }

     /* side set id status array: */

     dim[0] = dimid;
     if ((varid = ncvardef (genesis_file_id, VAR_SS_STAT, NC_LONG, 1, dim)) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
         "failed to define side set status in file id %d\n",genesis_file_id);
       return (EX_FATAL);         
     }

     /* side set id array: */

     if ((varid = ncvardef (genesis_file_id, VAR_SS_IDS, NC_LONG, 1, dim)) == -1) 
     {
       exerrval = ncerr;
       PRINT_ERROR(
         "failed to define side set property in file id %d\n",genesis_file_id);
       return (EX_FATAL);         
     }

/*   store property name as attribute of property array variable */
     if ((ncattput (genesis_file_id, varid, ATT_PROP_NAME, NC_CHAR, 3, "ID")) == -1)
     {
       exerrval = ncerr;
       PRINT_ERROR(
         "failed to store side set property name %s in file id %d\n",
         "ID",genesis_file_id);
       return (EX_FATAL);
     }

     // now, define the arrays needed for each side set
     
     int index = 0;
     for(unsigned int i = 0; i < sideset_data.size(); i++)
     {
       SideSetData side_set = sideset_data[i];

       // dont define an empty set
       if (side_set.number_elements == 0)
         continue;
       
       index++;
       if ((dimid = ncdimdef(genesis_file_id,DIM_NUM_SIDE_SS(index),
                             (long)side_set.number_elements)) == -1)
       {
         exerrval = ncerr;
         PRINT_ERROR(
           "failed to define number of sides in side "
           "set %d in file id %d\n", side_set.id,genesis_file_id);
         return(EX_FATAL);
       }

       dim[0] = dimid;

       if (ncvardef (genesis_file_id, VAR_ELEM_SS(index), NC_LONG, 1, dim) == -1)
       {
         if (ncerr == NC_ENAMEINUSE)
         {
           exerrval = ncerr;
           PRINT_ERROR(
             "element list already exists for side set %d "
             "in file id %d\n",side_set.id,genesis_file_id);
         }
         else
         {
           exerrval = ncerr;
           PRINT_ERROR(
             "failed to create element list for side set "
             "%d in file id %d\n",side_set.id,genesis_file_id);
         }
         return(EX_FATAL);            /* exit define mode and return */
       }
       if (ncvardef (genesis_file_id, VAR_SIDE_SS(index), NC_LONG, 1, dim) == -1)
       {
         if (ncerr == NC_ENAMEINUSE)
         {
           exerrval = ncerr;
           PRINT_ERROR(
             "side list already exists for side set %d in file id %d\n",
             side_set.id,genesis_file_id);
         }
         else
         {
           exerrval = ncerr;
           PRINT_ERROR(
             "failed to create side list for side set %d in file id %d\n",
             side_set.id,genesis_file_id);
         }
         return(EX_FATAL);         /* exit define mode and return */

       }
       
       //  sideset distribution factors
       if (write_sideset_distribution_factors )
       {
         if ((dimid = ncdimdef (genesis_file_id, DIM_NUM_DF_SS(index),
                                (long)side_set.number_nodes)) == -1)
         {
           exerrval = ncerr;
           PRINT_ERROR(
             "failed to define number of dist factors in "
             "side set %d in file id %d\n", side_set.id,genesis_file_id);
           return(EX_FATAL);          /* exit define mode and return */
         }

/* create variable array in which to store the side set distribution factors */

         dim[0] = dimid;

         if (ncvardef (genesis_file_id, VAR_FACT_SS(index),
                       nc_flt_code(genesis_file_id), 1, dim) == -1)
         {
           if (ncerr == NC_ENAMEINUSE)
           {
             exerrval = ncerr;
             PRINT_ERROR(
               "dist factors list already exists for side "
               "set %d in file id %d\n",side_set.id,genesis_file_id);
           }
           else
           {
             exerrval = ncerr;
             PRINT_ERROR(
               "failed to create dist factors list for side "
               "set %d in file id %d\n",side_set.id,genesis_file_id);
           }
           return(EX_FATAL);            /* exit define mode and return */
         }

       }
     }
   }
   

/* node coordinate arrays: */

   dim[0] = numdimdim;
   dim[1] = numnoddim;
   if (ncvardef (genesis_file_id, VAR_COORD, nc_flt_code(genesis_file_id), 2, dim) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
           "failed to define node coordinate array in file id %d\n",genesis_file_id);
     return (EX_FATAL);         
   }


/* coordinate names array */

   dim[0] = numdimdim;
   dim[1] = strdim;

   if (ncvardef (genesis_file_id, VAR_NAME_COOR, NC_CHAR, 2, dim) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
           "failed to define coordinate name array in file id %d\n",genesis_file_id);
     return (EX_FATAL);         
   }

   // define genesis maps if required

   if (write_maps)
   {
     // element map (ex_put_map)
     dim[0] = numelemdim;

     if ((ncvardef (genesis_file_id, VAR_MAP, NC_LONG, 1, dim)) == -1)
     {
       if (ncerr == NC_ENAMEINUSE)
       {
         PRINT_ERROR(
                 "element map already exists in file id %d\n",
                 genesis_file_id);
       }
       else
       {
         PRINT_ERROR(
                 "failed to create element map array in file id %d\n",
                 genesis_file_id);
       }
       return (EX_FATAL);         /* exit define mode and return */
     }

     // create the element number map (ex_put_elem_num_map)

     dim[0] = numelemdim;

     if ((ncvardef (genesis_file_id, VAR_ELEM_NUM_MAP, NC_LONG, 1, dim)) == -1)
     {
       if (ncerr == NC_ENAMEINUSE)
       {
          PRINT_ERROR(
                 "element numbering map already exists in file id %d\n",
                 genesis_file_id);
       }
       else
       {
         PRINT_ERROR(
                 "failed to create element numbering map in file id %d\n",
                 genesis_file_id);
       }
       return(EX_FATAL);         /* exit define mode and return */
     }

     // create node number map (ex_put_node_num_map)

     dim[0] = numnoddim;

     if ((ncvardef (genesis_file_id, VAR_NODE_NUM_MAP, NC_LONG, 1, dim)) == -1)
     {
       if (ncerr == NC_ENAMEINUSE)
       {
         PRINT_ERROR(
                 "node numbering map already exists in file id %d\n",
                 genesis_file_id);
       }
       else
       {
         PRINT_ERROR(
                 "failed to create node numbering map array in file id %d\n",
                 genesis_file_id);
       }
       return(EX_FATAL);         /* exit define mode and return */
     }

     
   }
   
   // define qa records to be used

   int num_qa_rec = mesh_info.num_qa_records;
   int num_qa_dim;

   if ((num_qa_dim = ncdimdef (genesis_file_id,DIM_NUM_QA,(long)num_qa_rec)) == -1)
   {
     if (ncerr == NC_ENAMEINUSE)      /* duplicate entry? */
     {
       exerrval = ncerr;
       PRINT_ERROR("qa records already exist in file id %d\n", genesis_file_id);
       //ex_err("ex_put_qa",errmsg,exerrval);
     }
     else
     {
       exerrval = ncerr;
       PRINT_ERROR("failed to define qa record array size in file id %d\n", 
                   genesis_file_id);
     }
     return (EX_FATAL);         
   }
   
   // define qa array

   int dims[3];
   dims[0] = num_qa_dim;
   dims[1] = n4dim;
   dims[2] = strdim;

   if ((varid = ncvardef (genesis_file_id, VAR_QA_TITLE, NC_CHAR, 3, dims)) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
             "failed to define qa record array in file id %d\n", genesis_file_id);
     return (EX_FATAL);         
   }

   /* estimate (guess) size of header of netCDF file */

   header_size = 1200 + 
                 block_data.size() * 800 + 
                 nodeset_data.size() * 220 + 
                 nodeset_data.size() * 300;

   if (header_size > MAX_HEADER_SIZE) header_size = MAX_HEADER_SIZE;

/* estimate (guess) size of fixed size variable section of netCDF file */

   if (nc_flt_code(genesis_file_id) == NC_DOUBLE) 
     iows = 8;
   else
     iows = 4;

   int num_elem = mesh_info.num_elements; //totalElements;
   fixed_var_size = mesh_info.num_dim * mesh_info.num_nodes * iows + //numDimensions * numberNodes * iows +
                    mesh_info.num_nodes /*numberNodes*/ * sizeof(int) +
                    num_elem * 16 * sizeof(int) +
                    mesh_info.num_elementblocks /*numElementBlocks*/ * sizeof(int) +
                    nodeset_data.size() * mesh_info.num_nodes/100 /*numberNodes/100*/ * sizeof(int) +
                    nodeset_data.size() * mesh_info.num_nodes/100 /*numberNodes/100*/ * iows +
                    nodeset_data.size() * sizeof(int) +
                    sideset_data.size() * num_elem/100 * 2 * sizeof(int) +
                    sideset_data.size() * num_elem/100 * iows +
                    sideset_data.size() * sizeof(int);


/* leave define mode */

   if (nc__enddef (genesis_file_id, 
                  header_size, NC_ALIGN_CHUNK, 
                  fixed_var_size, NC_ALIGN_CHUNK) == -1)
   {
     exerrval = ncerr;
     PRINT_ERROR(
          "failed to complete variable definitions in file id %d\n",genesis_file_id);
     return (EX_FATAL);
   }

   return (EX_NOERR);

}

VerdeStatus ExodusTool::write_element_order_map(ExodusMeshInfo &mesh_info)
{
  // note: this routine bypasses the standard exodusII interface for efficiency!

  // element order map
  int* map = new int[mesh_info.num_elements];

  // for now, output a dummy map!

   for(unsigned int i=0; i<mesh_info.num_elements; i++)
   {
      map[i] = i+1;
   }

   // output array and cleanup

   int error = write_exodus_integer_variable(genesisFileId,
                                             VAR_MAP,
                                             map,
                                             0,
                                             mesh_info.num_elements);
   

  if(map)
    delete [] map;
  
  if( error < 0 )
  {
    PRINT_ERROR("Failed writing element map\n"  );
    return VERDE_FAILURE;
  }
  
  return VERDE_SUCCESS;
}

VerdeStatus ExodusTool::write_global_node_order_map(ExodusMeshInfo &mesh_info)
{
  // note: this routine bypasses the standard exodusII interface for efficiency!

  // node order map
  int* map = new int[mesh_info.num_nodes];

  // for now, output a dummy map!

   for(unsigned int i=0; i<mesh_info.num_nodes; i++)
   {
      map[i] = i+1;
   }

   // output array and cleanup

   int error = write_exodus_integer_variable(genesisFileId,
                                             VAR_NODE_NUM_MAP,
                                             map,
                                             0,
                                             mesh_info.num_nodes);
   

  if(map)
    delete [] map;
  
  if( error < 0 )
  {
    PRINT_ERROR("Failed writing global node order map\n"  );
    return VERDE_FAILURE;
  }
  
  return VERDE_SUCCESS;
}

// Writes the element order map to the file
VerdeStatus ExodusTool::write_global_element_order_map(ExodusMeshInfo &mesh_info)
{

  // allocate map array
  int* map = new int[mesh_info.num_elements];


  // Many Sandia codes assume this map is unique, and CUBIT does not currently
  // have unique ids for all elements.  Therefore, to make sure nothing crashes,
  // insert a dummy map...

   for(unsigned int i=0; i<mesh_info.num_elements; i++)
   {
      map[i] = i+1;
   }


   // output array and cleanup

   int error = write_exodus_integer_variable(genesisFileId,
                                             VAR_ELEM_NUM_MAP,
                                             map,
                                             0,
                                             mesh_info.num_elements);

  
  if(map)
    delete [] map;
  
  if( error < 0 )
  {
    PRINT_ERROR("Failed writing global element order map\n"  );
    return VERDE_FAILURE;
  }
  
  return VERDE_SUCCESS;
}

VerdeStatus ExodusTool::write_exodus_integer_variable(int genesis_file_id,
                                                      const char* variable_name,
                                                      int *variable_array,
                                                      int start_position,
                                                      int number_values)
{
  
  // note: this routine bypasses the standard exodusII interface for efficiency!

   // write directly to netcdf interface for efficiency

   // get the variable id of the element map

   int mapid;
     
   if ((mapid = ncvarid (genesis_file_id, variable_name)) == -1)
   {
     PRINT_ERROR(
       "failed to locate variable %d in file %d\n", variable_name,
       genesis_file_id);
     return (VERDE_FAILURE);
   }
   // this contortion is necessary because netCDF is expecting nclongs; 
   // fortunately it's necessary only when ints and nclongs aren't the same size

   long start[1], count[1];
   int iresult;
   start[0] = (nclong) start_position;
   count[0] = number_values;

   if (sizeof(int) == sizeof(nclong)) {
     iresult = ncvarput (genesis_file_id, mapid, start, count, variable_array);
   } else {
     nclong *lptr = itol (variable_array, (int)number_values);
     iresult = ncvarput (genesis_file_id, mapid, start, count, lptr);
     free(lptr);
   }

   //int error = 0;
   
   if (iresult == -1)
   {
     PRINT_ERROR(
       "failed to store variable %s in file id %d\n", variable_name,
             genesisFileId);
     return VERDE_FAILURE;
   }

   return VERDE_SUCCESS;
}


VerdeStatus ExodusTool::write_qa_records(ExodusMeshInfo &mesh_info)
{
  // get the qa_record list

  std::deque<char*> *qa_record_list = mesh_info.mesh->get_qa_record_list();

  // write out the strings in the list


  int record = 0;
  for(unsigned int i = 0; i < mesh_info.num_qa_records-1; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      char *data = (*qa_record_list)[record++];
      write_qa_string(genesisFileId, data, i, j);
    }
  }

  // now, write out the time stamp for Verde



  VerdeStatus status;
  char string1[80];
  char string2[80];

  strcpy(string1,"VERDE");

  status = write_qa_string(genesisFileId, string1, mesh_info.num_qa_records-1, 0);

  strcpy(string1, "2.6");

  status = write_qa_string(genesisFileId, string1, mesh_info.num_qa_records-1, 1);

  time_and_date(string2, string1);
  status = write_qa_string(genesisFileId, string1, mesh_info.num_qa_records-1, 2);
  status = write_qa_string(genesisFileId, string2, mesh_info.num_qa_records-1, 3);


  return VERDE_SUCCESS;
}


VerdeStatus ExodusTool::write_qa_string(int /*genesis_file_id*/,
                                        char *string,
                                        int record_number,
                                        int record_position)
{
      // get the variable id in the exodus file

  int varid;
  if ((varid = ncvarid (genesisFileId, VAR_QA_TITLE)) == -1)
  {
    PRINT_ERROR("failed to find qa record array in file id %d", genesisFileId);
    return VERDE_FAILURE;
  }


//  write out the record

  long start[3], count[3];

  start[0] = record_number;
  start[1] = record_position;
  start[2] = 0;

  count[0] = 1;
  count[1] = 1;
  count[2] = strlen(string) +1;

  if (ncvarput (genesisFileId, varid, start, count,
              VOID_PTR string) == -1)
  {
    PRINT_ERROR("failed to store qa string in file id %d\n", genesisFileId);
    return (VERDE_FAILURE);
  }


  return VERDE_SUCCESS;
}



