//----------------------------------------------------------------
// Filename         : GUImain.cpp
//
// Purpose          : serves as the main control for the 
//                    VERDE_GUI program.
//
// Creator          : Clinton Stimpson
//
// Date             : 7 Aug 2001
//
// Owner            : Clinton Stimpson
//----------------------------------------------------------------


#include "../UserInterface.hpp"
#include "verde.h"
#include "../VerdeApp.hpp"
#include "VerdeGUIApp.hpp"
#include "../Mesh.hpp"
#include <qgl.h>

int main(int argc, char **argv)
{

  bool useGUI = true;
  
  //checks DISPLAY env. variable 
  #ifndef WIN32
    if( !(getenv("DISPLAY")) )
      useGUI = false;
  #endif   

  // set to global variable verde_app in VerdeApp constructor
  new VerdeApp(); 
  

  if( verde_app->initialize() == VERDE_FAILURE ||  
    UserInterface::instance()->init_interface(argc, argv) == VERDE_FAILURE )
  {
    delete verde_app;
    return 1;
  }


  int ret = 0;

  // if not if batch mode, init the GUI

  if (UserInterface::instance()->GetBatchMode() == VERDE_FALSE)
  {
    // create the verde GUI
    new VerdeGUIApp( argc, argv, useGUI );
    verde_gui_app->initialize();

    // create the main gui
    verde_gui_app->create_gui();

    if ( !QGLFormat::hasOpenGL() )
    {
      qWarning( "This system has no OpenGL support." );
    }
    
  }

  // print the splash screen

  UserInterface::instance()->start_up_screen();

   //Print out command line options
   if( argc > 1 )
   {
     PRINT_INFO("\n");
     PRINT_INFO("Executing: ");
     for(int i=0; i<argc; i++ )
       PRINT_INFO("%s ", argv[i]);
     PRINT_INFO("\n");
   }

   if(UserInterface::instance()->hasDefaultsFile() == VERDE_TRUE)
     GUICommandHandler::instance()->call_yyparse();

  // Try loading the mesh in case it was specified on the command line
  if(!verde_app->load_initial_mesh())
    return 1;

  // parse any journal files if any were entered
  if(UserInterface::instance()->isJournalling == VERDE_TRUE)
    GUICommandHandler::instance()->call_yyparse();
	  


  // if we are in batch mode
  if( UserInterface::instance()->GetBatchMode() == VERDE_TRUE)
  {
    // if there is no journal file, do a default calculation
    if (UserInterface::instance()->isJournalling == VERDE_FALSE)
    {
      verde_app->verify_mesh();
    }
  }
  else
  {
    // init some more stuff for interactive use
    verde_gui_app->vForm()->secondary_init();
	
    // add the VERDE> to begin with
    verde_gui_app->textBox()->append("VERDE> ");
		
    // the main program execution
    ret =  verde_gui_app->execute();
  }

  delete verde_app;
  delete verde_gui_app;

  return ret;

}






