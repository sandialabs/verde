//-------------------------------------------------------------------------
//- Class:         GUICommandHandler
//
//- Description:   GUICommandHandler class implementation.
//-                This class is implimented as a {singleton} pattern.  Only
//-                one instance is created and it is accessed through the
//-                {instance()} static member function.
//-                This class acts as an interface between the UI and GUI
//
//- Owner:         Clinton Stimpson
//
//- Date:          5/31/2001
//-------------------------------------------------------------------------

#include <string.h>
#include <ctype.h>
#include "GUICommandHandler.hpp"
#include <qapplication.h>
#include "../UserInterface.hpp"
#include "UserEventHandler.hpp"
#include "../VerdeApp.hpp"

#define DEBUG_GUICOMMANDHANDLER false

extern "C" {
extern int yyparse();
}

GUICommandHandler::GUICommandHandler()
{
  is_busy_parsing = VERDE_FALSE;
  command_to_execute = 0;
  command_read = false;
  current_history = 0;
}


GUICommandHandler* GUICommandHandler::instance_ = 0;

GUICommandHandler *GUICommandHandler::instance()
{
  if (instance_ == 0)
    instance_ = new GUICommandHandler;
  
  return instance_;
}


GUICommandHandler::~GUICommandHandler()
{
  char *tmp = NULL;
  int i;
  
  for(i = command_history.size(); i--; )
  {
    tmp = command_history.front();
    command_history.pop_front();
    delete [] tmp;
  }	
  
  instance_ = NULL;
  delete UserEventHandler::instance();
}



void GUICommandHandler::add_command_string(const char *command_string, VerdeBoolean echo, VerdeBoolean parse_it)
{
  if(!command_string) return;
  // make sure we don't call this recursively, because we 
  // don't handle it, and because it will leave a possibility
  // for infinite loops
  assert(command_to_execute == 0);

  int strlength = strlen(command_string);
  // put a copy of the string on the commandlist
  if(command_string[strlength-1] == '\n')
  {
    command_to_execute = new char[strlength+1];
    strcpy(command_to_execute, command_string);
  }
  else
  {
    command_to_execute = new char[strlength+2 ];
    strcpy(command_to_execute, command_string);
    command_to_execute [strlength] = '\n';
    command_to_execute [strlength+1] = '\0';
  }
  
  // return if parse_it = false
  if( parse_it == VERDE_FALSE )
    return;
  
  // get previous echo state
  int prev_echo_state = UserInterface::instance()->command_is_echoed();
  // if we don't want to echo, change the state
  if(echo == VERDE_FALSE)
  {
    UserInterface::instance()->command_is_echoed(echo);
  }
  
  call_yyparse();
  
  // restore the previous echo state
  UserInterface::instance()->command_is_echoed(prev_echo_state);
  
  if((tolower(command_to_execute[0]) == 'e' &&
        tolower(command_to_execute[1]) == 'x' &&
        tolower(command_to_execute[2]) == 'i' &&
        tolower(command_to_execute[3]) == 't' ) || 
      (tolower(command_to_execute[0]) == 'q' &&
       tolower(command_to_execute[1]) == 'u' &&
       tolower(command_to_execute[2]) == 'i' &&
       tolower(command_to_execute[3]) == 't' ) ) 
  {
    // tell the gui to quit
    QApplication::exit(0);
  }
  
  // add the command to the history list
  command_to_execute[strlen(command_to_execute) -1] = '\0';
  command_history.push_front(command_to_execute);
  
  // if list reaches max size, delete the oldest command
  if(command_history.size() > COMMAND_HISTORY_MAX_SIZE)
  {
    command_to_execute = command_history.back();
    command_history.pop_back();
    delete [] command_to_execute;
  }
  
  // reset history index
  current_history = 0;
	
  command_to_execute = 0;  
  
}


const char *GUICommandHandler::get_command_string(void)
{
  
  // return command at the front of the queue
  if((command_read == false) && (command_to_execute != 0) )
  {
    command_read = true;
    return command_to_execute; //commandlist.front();
  }
  else
    return "exit\n";
}

void GUICommandHandler::call_yyparse()
{
  
  // return if the parser is busy
  // if the parser is busy, it will automatically find the new command string
  if( is_busy_parsing == VERDE_TRUE )
    return;
  
  is_busy_parsing = VERDE_TRUE;
  
  // call this if there are no commands but journal files need processed
  if(command_to_execute == 0)         //commandlist.size() == 0)
    yyparse();
  
  else
  {
    // call this if there are commands on the list 
    // this will also process journal files before commands if they exist

    command_read = false;
    
    yyparse();
  }
  
  // a journal file was added so call yyparse again to process it
  if ( 0 != strcmp(UserInterface::instance()->currentFile->filename.c_str(), "VERDE_GUI") 
      && UserInterface::instance()->inputFileList.size() > 1 )
    yyparse();
  
  
  is_busy_parsing = VERDE_FALSE;
  
  UserEventHandler::instance()->process_messages();	
  
}

char * GUICommandHandler::history_next()
{
  
  // returns the next history command towards the most recent command
	
  // 0 means the original command (if partially entered, or blank)
  // 1 means the first in history  2, 3, 4, etc...
	
  if (current_history == 1)
  {
    current_history--;
    return 0;      // return the previous entered text
  }
  if(current_history == 0)
    return 0;
  
  current_history--;
  
  return command_history [current_history-1];
  
}

char * GUICommandHandler::history_prev()
{
  // returns the next history command towards the oldest command

  if (current_history >= command_history.size())
  {
    return command_history [current_history-1];
  }
  
  current_history++;
  
  return command_history [current_history-1];
  
}
