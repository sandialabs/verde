/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


#include <deque>
#include "VerdeGUIApp.hpp"
#include "../VerdeApp.hpp"
#include "GLWindow.hpp"


void ModelResultsTreeView::init()
{
  last_selected = 0;
}



void ModelResultsTreeView::calculate()
{
  last_selected = 0;
  GUICommandHandler::instance()->add_command_string( "analyize" );
  verde_gui_app->textBox()->insert_prompt();
  
  
  //See if there were no failed elements and if so, have a message box pop up
  if ( verde_app->get_metricsSuccess() &&
      verde_app->get_topologySuccess() &&
      verde_app->get_interfaceSuccess() )
  {
    QMessageBox::warning( this, "VERDE Information",
        "There were no failed elements in this analysis." );
  }
  
}


void ModelResultsTreeView::show_failed(const char* element_type, const char* metric, int failed)
{
  //set the failed element type
  QString label1( element_type);
  
  //set failed metric
  QString label2( metric );
  
  //set number of failed elements
  QString label3;
  label3.setNum(failed);
  
  //create the listviewitem (with the 3columns, label1, label2, label3) 
  new QListViewItem(FailedListView, label1, label2, label3);
}

void ModelResultsTreeView::highlight_failed_elements(QListViewItem* selected_item)
{
  
  if( !QString::compare(selected_item->text(0), "Hex" ))
  {
    if( !QString::compare(selected_item->text(1), "Aspect Ratio"))
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric aspect" );
    } 
    else if( !QString::compare(selected_item->text(1), "Skew" ))
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric skew" );
    }
    else if( !QString::compare(selected_item->text(1), "Taper" ))
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric taper" );
    }
    else if( !QString::compare(selected_item->text(1), "Element Volume" ))
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric volume" );
    }
    else if( !QString::compare(selected_item->text(1), "Stretch" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric stretch" );
    }
    else if( !QString::compare(selected_item->text(1), "Diagonal Ratio" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric diagonal" );
    }
    else if( !QString::compare(selected_item->text(1), "Dimension" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric dimension" );
    }
    else if( !QString::compare(selected_item->text(1), "Oddy" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric oddy" );
    }
    else if( !QString::compare(selected_item->text(1), "Condition No." ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric condition" );
    }
    else if( !QString::compare(selected_item->text(1), "Jacobian" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric jacobian" );
    }
    else if( !QString::compare(selected_item->text(1), "Scaled Jacobian" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric scaled jacobian" );
    }
    else if( !QString::compare(selected_item->text(1), "Shear" ))
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric shear" );
    }
    else if( !QString::compare(selected_item->text(1), "Shape" ))
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric shape" );
    }
    else if( !QString::compare(selected_item->text(1), "Relative Size" ))
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric relative size" );
    }
    else if( !QString::compare(selected_item->text(1), "Shape and Size" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed hexes metric shape and size" );
    }
  }
  
  else if( !QString::compare(selected_item->text(0), "Tet" ))
  {
    
    if( !QString::compare(selected_item->text(1), "Aspect Ratio" ) )
    {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric aspect" );
    }
  else if( !QString::compare(selected_item->text(1), "Aspect Ratio Gamma" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric aspect gamma" );
  }
  else if( !QString::compare(selected_item->text(1), "Element Volume" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric volume" );
  }
  else if( !QString::compare(selected_item->text(1), "Condition No." ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric condition" );
  }
  else if( !QString::compare(selected_item->text(1), "Jacobian" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric jacobian" );
  }
  else if( !QString::compare(selected_item->text(1), "Scaled Jacobian" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric scaled jacobian" );
  }
     else if( !QString::compare(selected_item->text(1), "Shear" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric shear" );
  }
   else if( !QString::compare(selected_item->text(1), "Shape" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric shape" );
  }
  else if( !QString::compare(selected_item->text(1), "Relative Size" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric relative size" );
  }
  else if( !QString::compare(selected_item->text(1), "Shape and Size" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tets metric shape and size" );
  }
    }
    else if( !QString::compare(selected_item->text(0), "Quad" ))
    {
  if( !QString::compare(selected_item->text(1),  "Aspect Ratio" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric aspect" );
  }
  else if( !QString::compare(selected_item->text(1), "Skew" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric skew" );
  }
  else if( !QString::compare(selected_item->text(1), "Taper" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric taper" );
  }
  else if( !QString::compare(selected_item->text(1), "Warpage" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric warpage" );
  }
  else if( !QString::compare(selected_item->text(1), "Element Area" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric area" );
  }
  else if( !QString::compare(selected_item->text(1), "Stretch" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric stretch" );
  }
  else if( !QString::compare(selected_item->text(1), "Maximum Angle" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric largest angle" );
  }
  else if( !QString::compare(selected_item->text(1), "Minimum Angle" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric smallest angle" );
  }
  else if( !QString::compare(selected_item->text(1), "Oddy" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric oddy" );
  }
  else if( !QString::compare(selected_item->text(1), "Condition No." ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric condition" );
  }
  else if( !QString::compare(selected_item->text(1), "Jacobian" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric jacobian" );
  }
  else if( !QString::compare(selected_item->text(1), "Scaled Jacobian" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric scaled jacobian" );
  }
  else if( !QString::compare(selected_item->text(1), "Shear" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric shear" );
  }
  else if( !QString::compare(selected_item->text(1), "Shape" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric shape" );
  }
  else if( !QString::compare(selected_item->text(1), "Relative Size" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric relative size" );
  }
  else if( !QString::compare(selected_item->text(1), "Shape and Size" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric shape and size" );
  }
  else if( !QString::compare(selected_item->text(1), "Coincident Quads" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric coincident" );
  }
  else if( !QString::compare(selected_item->text(1), "Share 3 Nodes" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed quads metric share three nodes" );
  }
    }      
    else if( !QString::compare(selected_item->text(0), "Tri" ))
    {
  if( !QString::compare(selected_item->text(1),  "Aspect Ratio" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric aspect" );
  }
  else if( !QString::compare(selected_item->text(1),  "Element Area" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric area" );
  }
  else if( !QString::compare(selected_item->text(1),  "Angle" ))
  {
     GUICommandHandler::instance()->add_command_string( "highlight failed tris metric largest angle" );
  }
  else if( !QString::compare(selected_item->text(1),  "Minimum Angle" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric smallest angle" );
  }
  else if( !QString::compare(selected_item->text(1),  "Condition No." ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric condition" );
  }
  else if( !QString::compare(selected_item->text(1),  "Scaled Jacobian" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric scaled jacobian" );
  }
  else if( !QString::compare(selected_item->text(1),  "Shear" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric shear" );
  }
  else if( !QString::compare(selected_item->text(1),  "Shape" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric shape" );
  }
  else if( !QString::compare(selected_item->text(1),  "Relative Size" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric relative size" );
  }
  else if( !QString::compare(selected_item->text(1),  "Shape and Size" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric shape and size" );
  }
  else if( !QString::compare(selected_item->text(1), "Coincident Tris" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric coincident" );
  }
  else if( !QString::compare(selected_item->text(1), "Nonconformal Tris" ) )
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed tris metric nonconformal" );
  }
    }

    else if( !QString::compare(selected_item->text(0), "Nodes" ))
    {
  if( !QString::compare(selected_item->text(1),  "Coincident Nodes" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed nodes metric coincident" );
  }
    }

    else if( !QString::compare(selected_item->text(0), "Edge" ))
    {
  if( !QString::compare(selected_item->text(1),  "Non-manifold Edges" ))
  {
     GUICommandHandler::instance()->add_command_string( "highlight failed edges metric nonmanifold" );
  }
  if( !QString::compare(selected_item->text(1),  "Length" ))
  {
      GUICommandHandler::instance()->add_command_string( "highlight failed edges metric length" );
  }
    }
}    


void ModelResultsTreeView::show_failed_elements()
{
  //! clear out the list
  FailedListView->clear();

  //Hex Elements
  if(HexMetric::instance()->number_failed_hexes())
  {
    for( int i=0; i<NUM_HEX_METRICS; i++)
    {
      std::deque<FailedHex*> failed_hexes;
      HexMetric::instance()->failed_hex_list(i, failed_hexes);

      int num_failed_hexes = failed_hexes.size();
      if( num_failed_hexes )
        show_failed("Hex", HexMetric::instance()->metric_name(i), num_failed_hexes );
    }
  }

  //Tet Elements
  if(TetMetric::instance()->number_failed_tets())
  {
    for( int i=0; i<NUM_TET_METRICS; i++)
    {
      std::deque<FailedTet*> failed_tets;
      TetMetric::instance()->failed_tet_list(i, failed_tets);

      int num_failed_tets = failed_tets.size();
      if( num_failed_tets )
        show_failed("Tet", TetMetric::instance()->metric_name(i),num_failed_tets );
     }
  }

  //Quad Elements
  if(QuadMetric::instance()->number_failed_quads())
  {
    for( int i=0; i<NUM_QUAD_METRICS; i++)
    {
      std::deque<FailedQuad*> failed_quads;
      QuadMetric::instance()->failed_quad_list(i, failed_quads);

      int num_failed_quads = failed_quads.size();
      if( num_failed_quads )
        show_failed("Quad",QuadMetric::instance()->metric_name(i), num_failed_quads );
    }
  }


  //Tri Elements
  if(TriMetric::instance()->number_failed_tris())
  {
    for( int i=0; i<NUM_TRI_METRICS; i++)
    {
      std::deque<FailedTri*> failed_tris;
      TriMetric::instance()->failed_tri_list(i, failed_tris);

      int num_failed_tris = failed_tris.size();
      if( num_failed_tris )
        show_failed("Tri",TriMetric::instance()->metric_name(i),num_failed_tris );
    }
  }
  
  //Edge Elements
   if(EdgeMetric::instance()->number_failed_edges())
   {
    for( int i=0; i<NUM_EDGE_METRICS; i++)
    {
      std::deque<FailedEdge*> failed_edges;
      EdgeMetric::instance()->failed_edge_list(i, failed_edges);

      int num_failed_edges= failed_edges.size();
      if( num_failed_edges)
        show_failed("Edge", EdgeMetric::instance()->metric_name(i),num_failed_edges);
    }
  }

  //Check for coincident quads
  if( SkinTool::number_coincident_quads() )
    show_failed("Quad","Coincident Quads",
          SkinTool::number_coincident_quads() );

  //Check for coincident tris
  if( SkinTool::number_coincident_tris() )
    show_failed("Tri","Coincident Tris",
          SkinTool::number_coincident_tris() );

  //Check for non-conformal tris
  if( SkinTool::number_non_conformal_tris() )
    show_failed("Tri","Nonconformal Tris",
          SkinTool::number_non_conformal_tris() );

  //Check for coincident nodes
  if( SkinTool::number_coincident_nodes() )
    show_failed("Nodes","Coincident Nodes",
          SkinTool::number_coincident_nodes() );

  //Check for non manifold edges
  if( EdgeTool::instance()->number_non_manifold_edges())
    show_failed("Edge","Non-manifold Edges",
          EdgeTool::instance()->number_non_manifold_edges() );

  //!Check for number of quads that share 3 nodes
  if( SkinTool::number_quads_share_three_nodes() )
    show_failed("Quad","Share 3 Nodes",
          SkinTool::number_quads_share_three_nodes() );


}

void ModelResultsTreeView::unselect_item( QListViewItem *item)
{
     //!if it's selected, unhighlight it
    if( !item || last_selected == item )
    {
       FailedListView->clearSelection();
       
       DrawingTool::instance()->clear_highlighted();
       DrawingTool::instance()->clear_selected();
       GLWindow::first_instance()->updateGL();
       last_selected = 0;
    }
     
   //!if it's not selected, highlight it  
    else
    {
       highlight_failed_elements(item);
       last_selected = item;
    }
      
   return;
}
 


void ModelResultsTreeView::unselect_all_items()
{
    FailedListView->clearSelection();
}


