//-------------------------------------------------------------------------
//- Class:         UserEventHandler
//
//- Description:   UserEventHandler class declaration.
//-                This class is implimented as a {singleton} pattern.  Only
//-                one instance is created and it is accessed through the
//-                {instance()} static member function.
//-                This class is ONLY accessable by GUICommandHandler and 
//-                CommandHandler
//
//- Owner:         Clinton Stimpson
//
//- Date:          10/1/2001
//-------------------------------------------------------------------------

/* \note {
   guideline for using this event handler:
   only use the post_message function
   from the CommandHandler.cpp file, PLEASE !!!
} 
*/
#ifndef USEREVENTHANDLER_HPP
#define USEREVENTHANDLER_HPP


#include <deque>

enum GUIUserEvents { none=0, UpdateModelView, ClearOutModelResultView, 
  UpdateGraphics, UpdateSkin, NewMesh, BlockActive, BlockInActive, ShowFailedElements,
  NodeBCActive, NodeBCInActive, ElementBCActive, ElementBCInActive, SetMouseToHourglass, 
  RestoreMouse, Edges, Skin, Normals, Label, ClearHighlighted, Perspective, Autofit, QuickTransform };


class UserMessage
{
public:
   GUIUserEvents  event;
   char*          data;
   UserMessage() : event(none), data(NULL){ };
   UserMessage(const UserMessage& copy);
   UserMessage(GUIUserEvents, const char* data = NULL);
   ~UserMessage();
};

//! UserEventHandler class declaration.
class UserEventHandler
{

friend class GUICommandHandler;
friend class CommandHandler;

private:
	// static pointer to only instance of class
	static UserEventHandler* instance_;

	// constructor
	UserEventHandler();

	// the GUICommandHandler will take care of 
	// processing events and deleting the instance

	// destructor
	~UserEventHandler();
	// gets an event from the queue
	void process_messages(bool one_time = false);

	std::deque<UserMessage> messages;

	// returns the instance of this class
	static UserEventHandler *instance();

	// post an event for the GUI to handle
	void post_message(UserMessage message, bool process_now = true);

};


#endif  // USEREVENTHANDLER_HPP
