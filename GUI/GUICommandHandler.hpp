//-------------------------------------------------------------------------
//- Class:         GUICommandHandler
//
//- Description:   GUICommandHandler class declaration.
//-                This class is implimented as a {singleton} pattern.  Only
//-                one instance is created and it is accessed through the
//-                {instance()} static member function.
//
//- Owner:         Clinton Stimpson
//
//- Date:          5/31/2001
//-------------------------------------------------------------------------


#ifndef GUICOMMANDHANDLER_HPP
#define GUICOMMANDHANDLER_HPP

#include "../VerdeDefines.hpp"
#include <deque>

#define COMMAND_HISTORY_MAX_SIZE 30

//! GUI command handler
class GUICommandHandler
{

private:
	//! static pointer to only instance of class
	static GUICommandHandler* instance_;

	// queue that holds the commands to process
	// commented out to support single threaded application only
//	std::queue<char *> commandlist;

	std::deque<char *> command_history;
	unsigned int current_history;

//  use command_to_execute for single-threaded use
	char * command_to_execute;
	bool command_read;

	// flag for if parser is busy or not
	VerdeBoolean is_busy_parsing;

	// constructor
	GUICommandHandler();

public:
	//! called to add command string to the queue
	void add_command_string(const char * command_string, VerdeBoolean echo = VERDE_TRUE, VerdeBoolean parse_it = VERDE_TRUE);

	//! get the string at the front of the queue
	const char *get_command_string(void);

	//! call the parsing function
	void call_yyparse();
		 
	//! returns the instance of this class
	static GUICommandHandler *instance();

	//! returns the next command in history
	char * history_next();

	//! returns the prev command in history
	char * history_prev();

	//! destructor
        ~GUICommandHandler(); 
};


#endif  // GUICOMMANDHANDLER_HPP
