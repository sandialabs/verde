//-------------------------------------------------------------------------
// Filename      : ElementBlock.cc 
//
// Purpose       : Contains information for an Exodus element block
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "ElementBlock.hpp"
#include "HexRef.hpp"
#include "TetRef.hpp"
#include "TriRef.hpp"
#include "QuadRef.hpp"
#include "PyramidRef.hpp"
#include "WedgeRef.hpp"
#include "KnifeRef.hpp"
#include "EdgeRef.hpp"
#include "HexMetric.hpp"
#include "TetMetric.hpp"
#include "PyramidMetric.hpp"
#include "WedgeMetric.hpp"
#include "KnifeMetric.hpp"
#include "TriMetric.hpp"
#include "QuadMetric.hpp"
#include "EdgeMetric.hpp"
#include "SkinTool.hpp"
#include "EulerTool.hpp"
#include "ErrorTool.hpp"
#include "VerdeMessage.hpp"
#include "VerdeString.hpp"
#include <string.h>
#include "BoundingBox.hpp"
#include "exodusII.h"

#ifdef USE_VERDICT
#include "verdict.h"
#endif

int ElementBlock::numActiveBlocks = 0;
bool ElementBlock::wasJustZero = false;
int ElementBlock::elementNumberMapIndex = 0;
int* ElementBlock::elementNumberMap = NULL;


ElementBlock::ElementBlock(int block_id)
{
   blockActive = VERDE_FALSE;
   elementsLoaded = VERDE_FALSE;
   blockID = block_id;

   // set MeshContainer owner

   elementData.owner(this);

   // get more information about the block
   read_block_header();   
   elementOffset = elementNumberMapIndex;
   elementNumberMapIndex += elementData.number_elements();

}

ElementBlock::~ElementBlock()
{
  if(elementNumberMap)
    delete [] elementNumberMap;
  elementNumberMap = NULL;
  skinTool.reset();
}

int ElementBlock::read_block_header()
{
  return elementData.read_block_header(blockID);
}

VerdeVector ElementBlock::centroid()
{
  if( boundingBox.is_used() )
    return boundingBox.get_centroid();
  else
  {
    // quads
    std::deque<QuadElem*>::iterator quad_list_iterator;
    std::deque<QuadElem*> quad_list;
    skinTool.local_exterior_quads(quad_list);
    for(quad_list_iterator = quad_list.begin();
    quad_list_iterator != quad_list.end();
    ++quad_list_iterator )
      boundingBox.add_quad(*quad_list_iterator); 

    // tris
    std::deque<TriElem*>::iterator tri_list_iterator;
    std::deque<TriElem*> tri_list; 
    skinTool.local_exterior_tris(tri_list);
    for(tri_list_iterator = tri_list.begin(); tri_list_iterator != tri_list.end(); ++tri_list_iterator )
      boundingBox.add_tri(*tri_list_iterator); 

    // edges
    std::deque<EdgeElem*>::iterator edge_list_iterator;
    std::deque<EdgeElem*> edge_list; 
    skinTool.local_edges(edge_list);
    for(edge_list_iterator = edge_list.begin();
    edge_list_iterator != edge_list.end();
    ++edge_list_iterator )
      boundingBox.add_edge(*edge_list_iterator);

    return boundingBox.get_centroid();
  }
}

int ElementBlock::load_elements()
{
	int status = elementData.load_elements(blockID,
			&elementNumberMap[elementOffset],
            elementOffset);  // elementNumberMapIndex ]);
	if(status == VERDE_SUCCESS)
		elementsLoaded = VERDE_TRUE;

	//elementOffset = elementNumberMapIndex;
	//elementNumberMapIndex += elementData.number_elements();
	return status;
	
}

//int *ElementBlock::get_element_connectivity(int element_index)
//{
//   return (connectivity + element_index*numberNodesPerElement);
//}

VerdeStatus ElementBlock::process(VerdeBoolean do_skin,
                                  VerdeBoolean do_metrics,
                                  VerdeBoolean do_individual,
                                  VerdeBoolean do_euler)
{
   // set the proper element type

   set_block_element_type();

   // get each element and process
   
   //VerdeString element_type = element_type();
   //int num_elements =  number_elements();

   // print metric info for block


   if (do_metrics && do_individual)
   {
   PRINT_INFO("\nMetrics for block %d, %d elements "
              "of type %s:\n\n", blockID,
              elementData.number_elements(),
              element_type().c_str());
   }
   else
   {
   PRINT_INFO("Processing block %d, %d elements "
              "of type %s:\n\n", blockID,
              elementData.number_elements(),
              element_type().c_str());
   }
   

   
   // process hex elements
   
   if (strncmp(element_type().c_str(),"HEX",3) == 0)
   {
      process_hex_block(do_skin,do_metrics,do_individual,do_euler);
   }
   
   // process Tet elements
   
   else if (strncmp(element_type().c_str(),"TET",3) == 0)
   {
      process_tet_block(do_skin,do_metrics,do_individual,do_euler);
   }

   // process Pyramid element
   
   else if (strncmp(element_type().c_str(),"PYR",3) == 0)
   {
      process_pyramid_block(do_skin,do_metrics,do_individual,do_euler);
   }

   // process Wedge element
   
   else if (strncmp(element_type().c_str(),"WED",3) == 0)
   {
      process_wedge_block(do_skin,do_metrics,do_individual,do_euler);
   }

   // process Knife element
   
   else if (strncmp(element_type().c_str(),"KNI",3) == 0)
   {
      process_knife_block(do_skin,do_metrics,do_individual,do_euler);
   }

   // process Quad/Shell elements
   
   else if (strncmp(element_type().c_str(),"SHELL",5) == 0
            || strncmp(element_type().c_str(),"QUAD",4) == 0)
   {
      process_quad_block(do_skin,do_metrics,do_individual,do_euler);
   }
   
   // process Tri elements
   
   else if (strncmp(element_type().c_str(),"TRI",3) == 0)
   {
      process_tri_block(do_skin,do_metrics,do_individual,do_euler);
   }

   // process Beam elements
   
   else if (strncmp(element_type().c_str(),"BEA",3) == 0 ||
            strncmp(element_type().c_str(),"BAR",3) == 0 ||
            strncmp(element_type().c_str(),"TRU",3) == 0)
   {
      process_edge_block(do_skin,do_metrics,do_individual,do_euler);
   }
   
   return VERDE_SUCCESS;
}

void ElementBlock::set_block_element_type()
{
   // in this routine, we attempt to find out what kind of
   // element was written in Exodus and process this type of
   // element.  For example, we try to identify degenerate
   // hexes, where pyramids, wedges, knives, etc are written
   // as type HEX

//   VerdeString elementType = elementData.element_type();


   // check hex block for degenerate types
   
   if (strncmp(element_type().c_str(),"HEX",3) == 0)
   {
         HexRef elem(elementData, (elementOffset+1) );
         int node_ids[8];
         for (int i = 0; i < 8; i++)
         {
            node_ids[i] = elem.node_id(i);
         }

         // check for degenerate pyramids

         if ( node_ids[4] == node_ids[5] &&
              node_ids[4] == node_ids[6] &&
              node_ids[4] == node_ids[7])
         {
            elementData.set_element_type( "PYRAMID  (DEGENERATE HEX)");
         }

         // check for degenerate knives

         else if ( node_ids[5] == node_ids[7] )
         {
            elementData.set_element_type ("KNIFE  (DEGENERATE HEX)");
         }

         // check for degenerate wedges

         else if (node_ids[2] == node_ids[3] &&
                  node_ids[6] == node_ids[7])
         {
            elementData.process_degenerate_wedges();
            elementData.set_element_type ( "WEDGE  (DEGENERATE HEX)");
         }
   }
   
   else if (strncmp(element_type().c_str(),"SHELL",5) == 0
            || strncmp(element_type().c_str(),"QUAD",4) == 0)
   {
      // check only one element from this block
      QuadRef elem(elementData, (elementOffset+1));
      // check 1 of 4 possible cases for a degenerate representation
      if( elem.node_id(2) == elem.node_id(3) )
      {
         elementData.set_element_type( "TRI  (DEGENERATE QUAD)");
      }

   }
}

void ElementBlock::process_hex_block(VerdeBoolean /*do_skin*/,
                                     VerdeBoolean do_metrics,
                                     VerdeBoolean do_individual,
                                     VerdeBoolean do_euler)
{

//   int num_elements =  elementData.number_elements();
   HexMetric *hex_metric = HexMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   
   if (do_individual)
      hex_metric->reset();

   // for metrics, we have to make an initial pass to calculate average
   // size for calculations

   int i;
   if (do_metrics)
   {
     double ave_volume = 0;
     for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
     {
       HexRef elem(elementData,i);
       ave_volume += hex_metric->volume(elem);
     }
     ave_volume /= number_elements();
#ifdef USE_VERDICT
     v_set_hex_size(ave_volume);
#else
     hex_metric->set_average_size(ave_volume);
#endif
     HexMetric::instance()->reset_volume();
   }
   

   for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      HexRef elem(elementData,i);
      if (do_metrics)
         hex_metric->calculate(elem);
//      if (do_skin  || do_euler)
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      hex_metric->print_summary();
      PRINT_INFO("\nBlock %d Volume: %12.3f\n", block_id(),
                 hex_metric->volume());
   }
}

void ElementBlock::process_tet_block(VerdeBoolean /*do_skin*/,
                                     VerdeBoolean do_metrics,
                                     VerdeBoolean do_individual,
                                     VerdeBoolean do_euler)
{

   //int num_elements =  elementData.number_elements();
   TetMetric *tet_metric = TetMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   //ErrorTool *error_tool = ErrorTool::instance();
   

   if (do_individual)
      tet_metric->reset();

   // for metrics, we have to make an initial pass to calculate average
   // size for calculations

   int i;
   if (do_metrics)
   {
     double ave_volume = 0;
     for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
     {
       TetRef elem(elementData,i);
       ave_volume += tet_metric->volume(elem);
     }
     ave_volume /= number_elements();
#ifdef USE_VERDICT
     v_set_tet_size(ave_volume);
#else
     tet_metric->set_average_size(ave_volume);
#endif

     TetMetric::instance()->reset_volume();
   }
   
   for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      TetRef elem(elementData,i);
      if (do_metrics)
         tet_metric->calculate(elem,0);
//      if (do_skin  || do_euler)
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      tet_metric->print_summary();
      PRINT_INFO("\nBlock %d Volume: %12.3f\n", block_id(),
                 tet_metric->volume());
   }
}

void ElementBlock::process_pyramid_block(VerdeBoolean /*do_skin*/,
                                     VerdeBoolean do_metrics,
                                     VerdeBoolean do_individual,
                                         VerdeBoolean do_euler)
{

   //int num_elements =  elementData.number_elements();
   PyramidMetric *pyramid_metric = PyramidMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   
   if (do_individual)
      pyramid_metric->reset();
   for (int i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      PyramidRef elem(elementData,i);
      if (do_metrics)
         pyramid_metric->calculate(elem);
//      if (do_skin  || do_euler )
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      pyramid_metric->print_summary();
      PRINT_INFO("\nBlock %d Volume: %12.3f\n", block_id(),
                 pyramid_metric->volume());
   }
}

void ElementBlock::process_wedge_block(VerdeBoolean /*do_skin*/,
                                       VerdeBoolean do_metrics,
                                       VerdeBoolean do_individual,
                                       VerdeBoolean do_euler)
{

//   int num_elements =  elementData.number_elements();
   WedgeMetric *wedge_metric = WedgeMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   
   if (do_individual)
      wedge_metric->reset();
   for (int i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      WedgeRef elem(elementData,i);
      
      if (do_metrics)
         wedge_metric->calculate(elem);
//      if (do_skin  || do_euler)
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      wedge_metric->print_summary();
      PRINT_INFO("\nBlock %d Volume: %12.3f\n", block_id(),
                 wedge_metric->volume());
   }
}

void ElementBlock::process_knife_block(VerdeBoolean /*do_skin*/,
                                       VerdeBoolean do_metrics,
                                       VerdeBoolean do_individual,
                                       VerdeBoolean do_euler)
{

//   int num_elements =  elementData.number_elements();
   KnifeMetric *knife_metric = KnifeMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   
   if (do_individual)
      knife_metric->reset();
   for (int i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      KnifeRef elem(elementData,i);
      
      if (do_metrics)
         knife_metric->calculate(elem);
//      if (do_skin  || do_euler)
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      knife_metric->print_summary();
      PRINT_INFO("\nBlock %d Volume: %12.3f\n", block_id(),
                 knife_metric->volume());
   }
}

void ElementBlock::process_quad_block(VerdeBoolean /*do_skin*/,
                                      VerdeBoolean do_metrics,
                                      VerdeBoolean do_individual,
                                      VerdeBoolean do_euler)
{

//   int num_elements =  elementData.number_elements();
   QuadMetric *quad_metric = QuadMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   
   if (do_individual)
      quad_metric->reset();

   // for metrics, we have to make an initial pass to calculate average
   // area for calculations

   int i;
   if (do_metrics)
   {
     double ave_area = 0;
     for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
     {
       QuadRef elem(elementData,i);
       ave_area += quad_metric->area(elem);
     }
     ave_area /= number_elements();
#ifdef USE_VERDICT
     v_set_quad_size(ave_area);
#else
     quad_metric->set_average_size(ave_area);
#endif

     QuadMetric::instance()->reset_area();
   }
   
   // now do main loop
   for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      QuadRef elem(elementData,i);
      if (do_metrics)
         quad_metric->calculate(elem);
//      if (do_skin  || do_euler)
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      quad_metric->print_summary();
      PRINT_INFO("\nBlock %d Area: %12.3f\n", block_id(),
                 quad_metric->area());
   }
}

void ElementBlock::process_tri_block(VerdeBoolean /*do_skin*/,
                                     VerdeBoolean do_metrics,
                                     VerdeBoolean do_individual,
                                     VerdeBoolean do_euler)
{

//   int num_elements =  elementData.number_elements();
   TriMetric *tri_metric = TriMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   
   if (do_individual)
      tri_metric->reset();

   // for metrics, we have to make an initial pass to calculate average
   // area for calculations

   int i;
   if (do_metrics)
   {
     double ave_area = 0;
     for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
     {
       TriRef elem(elementData,i);
       ave_area += tri_metric->area(elem);
     }
     ave_area /= number_elements();
#ifdef USE_VERDICT
     v_set_tri_size(ave_area);
#else
     tri_metric->set_average_size(ave_area);
#endif

     TriMetric::instance()->reset_area();
   }
   
   // now do main loop

   for (i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      TriRef elem(elementData,i);
      if (do_metrics)
         tri_metric->calculate(elem);
//      if (do_skin  || do_euler)
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      tri_metric->print_summary();
      PRINT_INFO("\nBlock %d Area: %12.3f\n", block_id(),
                 tri_metric->area());
   }
}

void ElementBlock::process_edge_block(VerdeBoolean /*do_skin*/,
                                      VerdeBoolean do_metrics,
                                      VerdeBoolean do_individual,
                                      VerdeBoolean do_euler)
{

//   int num_elements =  elementData.number_elements();
   EdgeMetric *edge_metric = EdgeMetric::instance();
   //SkinTool *skin_tool = SkinTool::instance();
   EulerTool *euler_tool = EulerTool::instance();
   
   if (do_individual)
      edge_metric->reset();
   for (int i = elementOffset+1; i <= number_elements()+elementOffset; i++)
   {
      EdgeRef elem(elementData,i);
      if (do_metrics)
         edge_metric->calculate(elem);
//      if (do_skin  || do_euler)
//         skin_tool->add_ref(elem);
      if (do_euler)
      {
         euler_tool->add_ref(elem);
         // mark nodes for euler counting
         elem.mark_nodes();
      }

   }
   
   // print metric info for block
   
   if (do_metrics && do_individual)
   {
      edge_metric->print_summary();
   }
}

VerdeStatus ElementBlock::read_element_number_map(int number_prev_elements,
                                                  int number_new_elements, 
                                                  int exodus_id)
{
  int * ptr;

  // read in the map from the exodus file
  if( !verde_app->should_add_mesh() )
  {
    // delete the old map if there is one
    if(elementNumberMap)
	  delete elementNumberMap;
    elementNumberMapIndex = 0;
    elementNumberMap = new int [number_new_elements];
    ptr = elementNumberMap;
  } 

  else
  {
    // new array big enough to store element ids  
    int * temp_elementNumberMap = new int [number_prev_elements + number_new_elements];

    // memcopy over old data into new array
    memcpy( temp_elementNumberMap, elementNumberMap, number_prev_elements*sizeof(int) ); 

    // delete old array
    delete [] elementNumberMap;
   
    // set new array to temp array
    elementNumberMap = temp_elementNumberMap;
   
    ptr = &(elementNumberMap[number_prev_elements]);
  } 

  
  int result = ex_get_elem_num_map(exodus_id, ptr);
  if ( result == -1 )
  {
    PRINT_INFO("Verde:: error reading exodus element number map array...\n");
    return VERDE_FAILURE;
  }
  
  return VERDE_SUCCESS;
  
}

