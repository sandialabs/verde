//-------------------------------------------------------------------------
// Filename      : VerdeGUIApp.hpp
//
// Purpose       : This file represents the GUI Verde application itself.
//
// Special Notes : This class was made to separate GUI Application stuff 
//                 from non-Application stuff
//
// Creator       : Corey Ernst
//
// Date          : 04/26/02
//
// Owner         : Corey Ernst 
//-------------------------------------------------------------------------

#ifndef VERDE_GUI_APP_HPP
#define VERDE_GUI_APP_HPP

#include <qapplication.h>

#include "../VerdeDefines.hpp"

class GUITextBox;
class VerdeForm;

class VerdeGUIApp  :public QApplication
{
  public:
  
   //! constructor
   VerdeGUIApp(int argc, char **argv, bool useGUI );

   //! destructor
   ~VerdeGUIApp();

   //! initialized the application and returns success
   void initialize();

   //! executes the gui
   int execute();

    //! access to GUITextBox
   GUITextBox* textBox();

   //! access to VerdeForm
   VerdeForm* vForm() { return VForm; }

   //! creates verde main gui
   void create_gui();

   //! sets save_gui_settings flag
   void saveGUISettings( VerdeBoolean value) { save_gui_settings = value; }

   //! writes settings of GUI
   void write_gui_settings();

   //! sets left mouse button map
   void set_leftButton(int value) { left_button = value; }

   //! sets middle mouse button map
   void set_middleButton(int value) { middle_button = value; }

   //! sets right mouse button map
   void set_rightButton(int value) { right_button = value; }

   //! sets zoom direction
   void set_zoomDir( VerdeBoolean value ) { zoom_dir = value; }

   //! sets working directory
   VerdeBoolean set_working_directory( const char * directory );

   //! gets working directory
   char * get_working_directory() { return workingDirectory; }

   //! accesses left mouse button map
   int leftButton() { return left_button; }

   //! accesses middle mouse button map
   int middleButton() { return middle_button; }

   //! accesses right mouse button map
   int rightButton() { return right_button; }

   //! accesses zoom direction
   VerdeBoolean zoomDir() { return zoom_dir; }

   //! gets save_gui_settins flag
   VerdeBoolean get_save_settings_flag() { return save_gui_settings; }

private:

    //! text box object for entering commands
   GUITextBox            *textbox;

   //! houses textbox, GlWindow, ModelResultsTreeView
   VerdeForm             *VForm;

   //! flag indicating whether user wants to save gui settings
   VerdeBoolean save_gui_settings;

   //! data indicating to what button is mapped (rot, trans, zoom)
   int left_button;

   //! data indicating to what button is mapped (rot, trans, zoom)
   int middle_button;

   //! data indicating to what button is mapped (rot, trans, zoom)
   int right_button;

   //! data indicating zoom direction
   VerdeBoolean zoom_dir;

   //! working directory
   char *workingDirectory; 

};

//! global variable. All file that include this file have access to it.
extern VerdeGUIApp *verde_gui_app;


#endif


