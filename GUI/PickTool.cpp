/*  File:     PickTool.cpp
 *  
 *  Purpose:  To handle picking interaction between
 *            the DrawingTool and GLInputWidget
 *  
 *  Creator:  Clinton Stimpson
 *
 *  Date:     7 Jan 2002
 */

#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif

#include <algorithm>

#include <qmessagebox.h>

#include "PickTool.hpp"
#include "GLWindow.hpp"
#include "DrawingTool.hpp"


PickTool* PickTool::_instance = NULL;


PickTool::PickTool()
{
   // pick blocks by default at startup
   // picktype = P_BLOCK;
}

PickTool::~PickTool()
{
}

PickTool* PickTool::instance()
{
   if(!_instance)
      _instance = new PickTool;

   return _instance;
}

void PickTool::reset()
{
   picked_entities.clear();
   DrawingTool::instance()->clear_selected();
}

/*
void PickTool::set_picktype(PickType type)
{
   picktype = type;
}

PickTool::PickType PickTool::get_picktype()
{
   return picktype;
}
*/

bool PickTool::process_pick_event(QMouseEvent* event)
{
   // remove any previous entities from the picked_entities list
   picked_entities.clear();

   // get some GLWindow metrics and mouse event coordinates
   GLWindow* win = GLWindow::first_instance();
   int GLWin[2] = { win->width(), win->height() };
   GLdouble clips[2];
   win->get_near_far_clips(clips[0], clips[1]);
   int pick_x = event->x();
   int pick_y = GLWin[1] - event->y();

   // set our pick buffer
   glSelectBuffer(PT_PICK_BUFFER_SIZE, pick_buffer);

   // initialize selection mode
   glRenderMode(GL_SELECT);

   glInitNames();
   
   // modify the projection matrix to limit the pick area
   GLint viewport[4];
   glGetIntegerv(GL_VIEWPORT, viewport);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluPickMatrix(pick_x, pick_y, 10.0, 10.0*GLWin[1]/GLWin[0], viewport);
   gluPerspective(35, (GLfloat)GLWin[0]/(GLfloat)GLWin[1], clips[0], clips[1]);
   glMatrixMode(GL_MODELVIEW);
   
   // draw things
   GLWindow::first_instance()->paintGL();

   // for debugging picking, uncomment this line
   // and comment the glRenderMode(GL_SELECT);
   //GLWindow::first_instance()->swapBuffers();

   // restore the previous state before picking   
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);   

   // get the number of hits and return to render mode
   int pick_hits = glRenderMode(GL_RENDER);

   // if we get a buffer overflow
   if(pick_hits == -1)
   {
      QMessageBox::warning(NULL, "Selection Warning",
            "Too many selectable items.\nPlease use another view", 
            QMessageBox::Ok, 0);

      return false;
   }

   // process the pick buffer
   process_hits(pick_hits);
   current_item = 1;

   draw_selected();
   
   return true;
}


/*   ------ The hierarchical model for picking ------------
 *
 *   this model was designed to allow filtering of which entity types
 *   we want to pick.
 *
 *   a glPushName(PickTool::PickType) is specified in
 *   the DrawingTool when drawing entities.
 *   a glLoadName(entity_id) is done for each entity
 *   finally, a glPopName() is done
 *
 *   following the above procedure, our pick_buffer will contain
 *   2, min_depth, max_depth, pick_type, entity_id, .....
 *    
 */


void PickTool::process_hits(int pick_hits)
{
   // pick_hits is the number of entities that were picked.
   
   // check for any stack errors
   GLenum error;
   while((error = glGetError()) != GL_NO_ERROR)
   {
      if(error == GL_STACK_OVERFLOW)
         std::cout << "gl stack overflow in picking" << endl;
      if(error == GL_STACK_UNDERFLOW)
         std::cout << "gl stack underflow in picking" << endl;
   }

   // keep track of zdepth for each entity so we can make
   // a list in z-order
   std::list<unsigned int> zdepth;
      
   int i;
   unsigned int curr_ptr = 0;
   for(i = 0; i < pick_hits; i++)
   {
      unsigned int num_names_for_this_hit = pick_buffer[curr_ptr++];
      
      // max_depth commented out for now.  
      // We may want to use them later.
      unsigned int min_depth = pick_buffer[curr_ptr++];
      /*unsigned int max_depth = pick_buffer[*/curr_ptr++/*]*/;
      
      // the hierarchical model was not followed, ignore this hit
      if(num_names_for_this_hit != 2)
      {
         curr_ptr += num_names_for_this_hit;
         continue;
      }

      // the type of entity that produced this hit
      int entity_type = pick_buffer[curr_ptr++];
      int entity_id = pick_buffer[curr_ptr++];

      // find out where in the list to insert this item
      // so our list is in z-order
      if(picked_entities.size() > 0)
      {
         bool inserted = false;
         std::list<PickedEntity>::iterator IT_sel_list = picked_entities.begin();
         std::list<unsigned int>::iterator IT_zdepth = zdepth.begin();
         while( IT_sel_list != picked_entities.end() && inserted == false )
         {
            // if this is a duplicate, skip it
            if( (*IT_sel_list).entity_id == entity_id  &&
             (*IT_sel_list).picktype == entity_type )
            {
               inserted = true;
            }
            else if(min_depth < *IT_zdepth)
            {
               inserted = true;
               PickedEntity ent = { entity_type, entity_id };
               picked_entities.insert(IT_sel_list, ent);
               zdepth.insert(IT_zdepth, min_depth);
            }               
            IT_sel_list++;  IT_zdepth++;
         }
         if(inserted == false)
         {
            PickedEntity ent = { entity_type, entity_id };
            picked_entities.push_back(ent);
            zdepth.push_back(min_depth); 
         }
      }
      // this is the first item in the list, just add it.
      else
      {
         PickedEntity ent = { entity_type, entity_id };
         picked_entities.push_front(ent);
         zdepth.push_front(min_depth);
      }
      
   }


   // we need to uniquify our list because hiddenline and smoothshade
   // have duplicate hits
   // picked_entities.unique();

}


void PickTool::step()
{
   if(current_item >= picked_entities.size())
      current_item = 1;
   else
      current_item++;

   draw_selected();   
}


void PickTool::draw_selected()
{
   // send data to DrawingTool
   DrawingTool::instance()->highlight_selected( current_item, picked_entities);
   // redraw the scene
   GLWindow::first_instance()->updateGL();

}

