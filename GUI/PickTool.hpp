/*  File:     PickTool.hpp
 *  
 *  Purpose:  To handle picking interaction between
 *            the DrawingTool and GLInputWidget
 *  
 *  Creator:  Clinton Stimpson
 *
 *  Date:     7 Jan 2002
 */


#ifndef PICK_TOOL_HPP
#define PICK_TOOL_HPP

#include <list>

#include <qevent.h>
#include <qgl.h>

#define PT_PICK_BUFFER_SIZE 2048


struct PickedEntity
{
   int picktype;
   int entity_id;
};


//! Handles picking of entities
class PickTool
{
public:
   //! singleton access to the PickTool
   static PickTool* instance();
   //! destructor for the PickTool
   ~PickTool();

   //! available types of entities to pick from
   enum PickType { P_BLOCK=1, P_HEX, P_TET, P_WEDGE, 
      P_PYRAMID, P_KNIFE, P_QUAD, P_TRI, P_EDGE, P_NODE };

   //! set which type of entity to pick
   // void set_picktype(PickType thispicktype);
   //! get which type of entity is currently pickable
   // PickType get_picktype();

   //! processes a pick event, returns success
   bool process_pick_event(QMouseEvent* event);   
   
   //! resets any information stored in the PickTool
   void reset();

   //! steps to the next selected item and draws it
   void step();

private:
   //! private constructor
   PickTool();
   //! the only instance of PickTool
   static PickTool* _instance;

   //! processes the pick buffer
   void process_hits(int pick_hits);

   //! draws the selected items
   void draw_selected();

   //! stores which type of entity is currently pickable
   // PickType picktype;
   //! pick buffer for storing the selection results
   GLuint pick_buffer[PT_PICK_BUFFER_SIZE];
   //! list of integers of picked entities
   std::list<PickedEntity> picked_entities;

   //! the current selectable item
   unsigned int current_item;
   
};


#endif


