//-------------------------------------------------------------------------
//- Class:         UserEventHandler
//
//- Description:   UserEventHandler class implementation.
//-                This class is implimented as a {singleton} pattern.  Only
//-                one instance is created and it is accessed through the
//-                {instance()} static member function.
//-                This class acts as an interface between the UI and GUI
//-                This class is only accessable by GUICommandHandler and
//-                CommandHandler.
//
//- Owner:         Clinton Stimpson
//
//- Date:          10/1/2001
//-------------------------------------------------------------------------


#include "UserEventHandler.hpp"
#include "VerdeGUIApp.hpp"
#include "verde.h"
#include "ModelTreeView.h"
#include "ModelResultsTreeView.h"
#include <qlistview.h>
#include "DrawingTool.hpp"
#include "GLWindow.hpp"
#include "PickTool.hpp"
#include "../UserInterface.hpp"
#include "../ElementBlock.hpp"
#include "../ElementBC.hpp"
#include "../NodeBC.hpp"
#include <qapplication.h>
#include <qcursor.h>
#include <qobject.h>


UserMessage::UserMessage(const UserMessage& copy)
{
  if(copy.data)
  {
    data = new char[strlen(copy.data)+1];
    strcpy(data, copy.data);
  }  
  else
    data=NULL;
  event = copy.event;
}

UserMessage::UserMessage(GUIUserEvents _event, const char* _data)
{
  if(_data)
  {
    data = new char[strlen(_data)+1];
    strcpy(data, _data);
  }
  else
    data = NULL;

  event = _event;

}

UserMessage::~UserMessage()
{
  if(data)
    delete [] data;
}

UserEventHandler::UserEventHandler()
{}


UserEventHandler* UserEventHandler::instance_ = 0;

UserEventHandler *UserEventHandler::instance()
{
	if (instance_ == 0)
		instance_ = new UserEventHandler;

	return instance_;
}


UserEventHandler::~UserEventHandler()
{
	instance_ = NULL;
}


// puts an event to process on the deque
void UserEventHandler::post_message(UserMessage event, bool process_now)
{
  if(process_now == FALSE)
    messages.push_back(event);
  else
  {
    messages.push_front(event);
    process_messages(true);
  }
}


// processes messages on the deque
void UserEventHandler::process_messages(bool one_time)
{
  while(messages.size())
  {
    UserMessage Message = messages.front();
    messages.pop_front();
    
    if(UserInterface::instance()->GetBatchMode() == VERDE_TRUE)
      continue;

    switch(Message.event) 
    {
      case none:
      {
		      break;
      }

      // we read in a new mesh so update the ModelView              
      case UpdateModelView:
      {
		      ModelTreeView* model = verde_gui_app->vForm()->getModelView();
          if(model)
            model->get_model_info();
          //This enables the calculate button in ModelResultsTreeView when 
          //a mesh is read in
          ModelResultsTreeView* model_result = verde_gui_app->vForm()->getModelResultsView();
          if(model_result)
            model_result->calculateButton->setEnabled(1);
          
          break;
      }

      // we read in a new mesh so we need to clean out
      // the ModelResultsView
      case ClearOutModelResultView:
      {
        
        ModelResultsTreeView* model_result = verde_gui_app->vForm()->getModelResultsView();
        if(model_result)
          model_result->FailedListView->clear();
        
        break;
      }
      
      // the skin changed, so tell the DrawingTool that. 
      case UpdateSkin:
      {
        
        DrawingTool::instance()->add_surface_mesh();
        DrawingTool::instance()->add_model_edges();
        DrawingTool::instance()->add_surface_normals();
    
        // if autofit is on, or if there are 1 active blocks, no active nodesets or sidesets
        if(DrawingTool::instance()->get_autofit_toggle() || 
           ( ElementBC::number_active_sidesets() == 0 && 
             ElementBlock::number_active_blocks() == 1 &&
             NodeBC::number_active_nodesets() == 0 &&
             ElementBlock::was_just_zero() ))
          GLWindow::first_instance()->center_model();

        if( !DrawingTool::instance()->get_autofit_toggle() )
          GLWindow::first_instance()->resize_radius();

        // continue to UpdateGraphics for a redraw
      }

      case UpdateGraphics:
      {
        GLWindow::first_instance()->updateGL();
        break;
      }

      case NewMesh:
      {
        DrawingTool::instance()->reset();
 
        DrawingTool::instance()->add_surface_mesh();
        DrawingTool::instance()->add_model_edges();
        DrawingTool::instance()->add_surface_normals();
        
        //draw normals if button is down
        DrawingTool::instance()->toggle_normals_vis(verde_gui_app->vForm()->showNormals->isOn());			
        //draw skin if button is down
        DrawingTool::instance()->toggle_skin_vis(verde_gui_app->vForm()->showSkin->isOn());			
        //draw model edges if button is down
        DrawingTool::instance()->toggle_model_edges_vis(verde_gui_app->vForm()->showEdges->isOn());			
        GLWindow::first_instance()->reset();
        GLWindow::first_instance()->set_draw_temp_only(false);
        GLWindow::first_instance()->center_model();
        GLWindow::first_instance()->updateGL();
        break;
      }
      case BlockActive:
      {
        if(!Message.data)
          break;
        int block_id = atoi(Message.data);
        if(block_id == 0)
          break;

        verde_gui_app->vForm()->getModelView()->make_block_active(block_id);
        break;                  

      }
      case BlockInActive:
      {
        if(!Message.data)
          break;
        int block_id = atoi(Message.data);
        if(block_id == 0)
          break;

        verde_gui_app->vForm()->getModelView()->make_block_inactive(block_id);
        break;
      }
      case ShowFailedElements:
      {
        verde_gui_app->vForm()->getModelResultsView()->show_failed_elements();
      }
      case NodeBCActive:
      {
        if(!Message.data)
          break;
        int id = atoi(Message.data);
        DrawingTool::instance()->add_NodeBC(id);
        verde_gui_app->vForm()->getModelView()->make_ns_active(id);

        // if autofit is on, or if there is one active nodeset, and no active sidesets or blocks 
        if(DrawingTool::instance()->get_autofit_toggle() || 
           ( ElementBC::number_active_sidesets() == 0 && 
             ElementBlock::number_active_blocks() == 0 &&
             NodeBC::number_active_nodesets() == 1 ))
          GLWindow::first_instance()->center_model();

        if( !DrawingTool::instance()->get_autofit_toggle() )
          GLWindow::first_instance()->resize_radius();

        break;
      }
      
      case NodeBCInActive:
      {
        if(!Message.data)
          break;
        int id = atoi(Message.data);
        DrawingTool::instance()->remove_NodeBC(id);
        verde_gui_app->vForm()->getModelView()->make_ns_inactive(id);

        break;
      }
      case ElementBCActive:
      {
        if(!Message.data)
          break;
        int id = atoi(Message.data);
        DrawingTool::instance()->add_ElementBC(id);
        verde_gui_app->vForm()->getModelView()->make_ss_active(id);

        // if autofit is on, or if there is one active sideset, and no active nodesets or blocks 
        if(DrawingTool::instance()->get_autofit_toggle() || 
           ( ElementBC::number_active_sidesets() == 1 && 
             ElementBlock::number_active_blocks() == 0 &&
             NodeBC::number_active_nodesets() == 0 ))
          GLWindow::first_instance()->center_model();

        if( !DrawingTool::instance()->get_autofit_toggle() )
          GLWindow::first_instance()->resize_radius();

        break;
      }
      case ElementBCInActive:
      {
        if(!Message.data)
          break;
        int id = atoi(Message.data);
        DrawingTool::instance()->remove_ElementBC(id);
        verde_gui_app->vForm()->getModelView()->make_ss_inactive(id);

        break;
      }
      case SetMouseToHourglass:
      {
        // setOverrideCursor doesn't work in NT
        //QApplication::setOverrideCursor(Qt::WaitCursor);
        verde_gui_app->vForm()->setCursor(QCursor(Qt::WaitCursor));
        GLWindow::first_instance()->setCursor(QCursor(Qt::WaitCursor));
        break;
      }
      case RestoreMouse:
      {
        //QApplication::restoreOverrideCursor();
        verde_gui_app->vForm()->setCursor(QCursor(Qt::ArrowCursor));
        GLWindow::first_instance()->setCursor(QCursor(Qt::CrossCursor));
        break;
      }
      
      case Edges:
      {
        // block signal emission
        verde_gui_app->vForm()->showEdges->blockSignals(true);
        if(Message.data)
        {
          DrawingTool::instance()->toggle_model_edges_vis(1);
          verde_gui_app->vForm()->showEdges->setOn(1);
        }
        else
        {
          DrawingTool::instance()->toggle_model_edges_vis(0);
          verde_gui_app->vForm()->showEdges->setOn(0);
        }

        // unblock signal emission
        verde_gui_app->vForm()->showEdges->blockSignals(false);        
        GLWindow::first_instance()->updateGL();
        break; 
      }

      case Skin:
      {
        // block the emission of signals
        verde_gui_app->vForm()->showSkin->blockSignals(true);

        if(Message.data)
        {
          DrawingTool::instance()->toggle_skin_vis(1);
          verde_gui_app->vForm()->showSkin->setOn(1);
        }
        else
        {
          DrawingTool::instance()->toggle_skin_vis(0);
          verde_gui_app->vForm()->showSkin->setOn(0);
        }

        // unblock the emission of signals
        verde_gui_app->vForm()->showSkin->blockSignals(false);        
        // update the graphics
        GLWindow::first_instance()->updateGL();
        break; 
      }

      case Normals:
      {
        verde_gui_app->vForm()->showNormals->blockSignals(true);        
        if(Message.data)
        {
          DrawingTool::instance()->toggle_normals_vis(1);
          verde_gui_app->vForm()->showNormals->setOn(1);
        }
        else
        {
          DrawingTool::instance()->toggle_normals_vis(0);
          verde_gui_app->vForm()->showNormals->setOn(0);
        }

        verde_gui_app->vForm()->showNormals->blockSignals(false);
        GLWindow::first_instance()->updateGL();
        break; 
      }

      case Label:
      {
        verde_gui_app->vForm()->sidesetLabels->blockSignals(true);
        verde_gui_app->vForm()->blockLabels->blockSignals(true);
        verde_gui_app->vForm()->nodesetLabels->blockSignals(true);
        verde_gui_app->vForm()->nodeLabels->blockSignals(true);
        verde_gui_app->vForm()->elementLabels->blockSignals(true);


        int on_off_flag = atoi(&(Message.data[0]));

        if( !strcmp( &Message.data[2], "nodes" ))
        { 
          DrawingTool::instance()->toggle_node_labels(on_off_flag);
          verde_gui_app->vForm()->nodeLabels->setOn(on_off_flag);
        }
        else if( !strcmp( &Message.data[2], "elements" ))
        { 
          DrawingTool::instance()->toggle_element_labels(on_off_flag);
          verde_gui_app->vForm()->elementLabels->setOn(on_off_flag);
        }
        else if( !strcmp( &Message.data[2], "failed" ))
        { 
          DrawingTool::instance()->toggle_failed_labels(on_off_flag);
          verde_gui_app->vForm()->failedElemLabels->setOn(on_off_flag);
        }
        else if( !strcmp( &Message.data[2], "blocks" ))
        { 
          DrawingTool::instance()->toggle_block_labels(on_off_flag);
          verde_gui_app->vForm()->blockLabels->setOn(on_off_flag);
        }
        else if( !strcmp( &Message.data[2], "nodesets" ))
        { 
          DrawingTool::instance()->toggle_nodeset_labels(on_off_flag);
          verde_gui_app->vForm()->nodesetLabels->setOn(on_off_flag);
        }
        else if( !strcmp( &Message.data[2], "sidesets" ))
        { 
          DrawingTool::instance()->toggle_sideset_labels(on_off_flag);
          verde_gui_app->vForm()->sidesetLabels->setOn(on_off_flag);
        }

        verde_gui_app->vForm()->sidesetLabels->blockSignals(false);
        verde_gui_app->vForm()->blockLabels->blockSignals(false);
        verde_gui_app->vForm()->nodesetLabels->blockSignals(false);
        verde_gui_app->vForm()->nodeLabels->blockSignals(false);
        verde_gui_app->vForm()->elementLabels->blockSignals(false);

        GLWindow::first_instance()->updateGL();
        break; 
      }
      case ClearHighlighted:
      {
        DrawingTool::instance()->clear_highlighted();
	verde_gui_app->vForm()->getModelResultsView()->unselect_all_items();
        PickTool::instance()->reset();
        break;
      }
      case Perspective:
      {
        verde_gui_app->vForm()->perspectiveView->blockSignals(true);        
        if(Message.data)
        {
          GLWindow::first_instance()->setScale(1.0);
          GLWindow::first_instance()->set_perspective(1);
          verde_gui_app->vForm()->perspectiveView->setOn(1);
        }
        else
        {
          GLWindow::first_instance()->set_perspective(0);
          verde_gui_app->vForm()->perspectiveView->setOn(0);
        }

        verde_gui_app->vForm()->perspectiveView->blockSignals(false);        
        GLWindow::first_instance()->switch_viewing_mode();
        GLWindow::first_instance()->updateGL();
        break; 
      }
      case Autofit:
      {
        verde_gui_app->vForm()->autoFit->blockSignals(true);        
        if(Message.data)
        {
          verde_gui_app->vForm()->autoFit->setOn(1);
        }
        else
        {
          verde_gui_app->vForm()->autoFit->setOn(0);
        }

        verde_gui_app->vForm()->autoFit->blockSignals(false);        
        break; 
      }
      case QuickTransform:
      {
        verde_gui_app->vForm()->quickTransform->blockSignals(true);
        if(Message.data)
          verde_gui_app->vForm()->quickTransform->setOn(false);
        else
          verde_gui_app->vForm()->quickTransform->setOn(true);        
       
        verde_gui_app->vForm()->quickTransform->blockSignals(false);
      }


      default:
        break;
    }
    if(one_time == true)
      continue;
  }
}

