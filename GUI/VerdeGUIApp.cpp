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



#include <qsettings.h>
#include <qpoint.h>
#include <qaction.h>

#include "GUITextBox.hpp"
#include "verde.h"
#include "VerdeGUIApp.hpp"
#include "ModelTreeView.h"
#include "ModelResultsTreeView.h"
#include "GLWindow.hpp"

VerdeGUIApp *verde_gui_app = 0;


VerdeGUIApp::VerdeGUIApp(int argc, char **argv, bool useGUI )
                        :QApplication( argc, argv, useGUI)
{
  verde_gui_app = this;
  workingDirectory = 0;
  set_working_directory("./");
}

VerdeGUIApp::~VerdeGUIApp()
{

 if(workingDirectory != 0 )
    delete [] workingDirectory;

 delete VForm;
}


void VerdeGUIApp::initialize()
{


   save_gui_settings = VERDE_FALSE;

   //mouse buttons
   left_button = 0;   // 0 = rotate
   middle_button = 1; // 1 = zoom
   right_button = 2;  // 2 = translate
   zoom_dir = VERDE_TRUE;  // pan out with up mouse movement

}

int VerdeGUIApp::execute()
{
  int ret = exec();
  if( save_gui_settings )
     write_gui_settings();
  else
  {
    QSettings settings;
    settings.writeEntry( "/verde/edit/settings save_gui_settings", false);
  }
  return ret;
}


void VerdeGUIApp::create_gui()
{
  VForm = new VerdeForm();
  connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));
  textbox = VForm->getTextBox();

  //read gui settings
  QSettings settings;
  int pos_x    = settings.readNumEntry( "/verde/main_window x", 161 );
  int pos_y    = settings.readNumEntry( "/verde/main_window y", 43 );
  int size_x   = settings.readNumEntry( "/verde/main_window width", 831 );
  int size_y   = settings.readNumEntry( "/verde/main_window height", 731 );
  int graphics = settings.readNumEntry( "/verde/display graphics", 0 );

  //mouse buttons
  left_button   = settings.readNumEntry( "/verde/edit/mouse left_button", 0 );
  middle_button = settings.readNumEntry( "/verde/edit/mouse middle_button", 1 );
  right_button  = settings.readNumEntry( "/verde/edit/mouse right_button", 2 );
  zoom_dir      = settings.readBoolEntry("/verde/edit/mouse zoom_direction", true) ? VERDE_TRUE : VERDE_FALSE;
  save_gui_settings = settings.readBoolEntry("/verde/edit/settings save_gui_settings", true) ? VERDE_TRUE : VERDE_FALSE;
 
  // point size 
  float point_size = settings.readNumEntry( "verde/display pointsize", 0.0);

  // normals, skin and edges
  int normals  = settings.readNumEntry( "/verde/display normals", 0 );
  int skin     = settings.readNumEntry( "/verde/display skin", 1 );
  int edges    = settings.readNumEntry( "/verde/display edges", 1 );

  int orthographic = settings.readBoolEntry( "/verde/view orthographic", 0);
  int autofit      = settings.readBoolEntry( "/verde/view autofit", 0);

  int quick_transform = settings.readBoolEntry( "/verde/view quicktransform", 1);

  if( save_gui_settings )
    VForm->saveSettings->setOn(TRUE);

  if( normals == 1)
    VForm->showNormals->setOn( TRUE );

  if( skin == 0)
    VForm->showSkin->setOn( FALSE );

  if( edges == 0)
    VForm->showEdges->setOn( FALSE );

  if( autofit )
    VForm->autoFit->setOn(TRUE);

  VForm->quickTransform->setOn( (quick_transform != 0) ? true : false );

  if( orthographic )
    VForm->perspectiveView->setOn(FALSE);

  if( graphics == 1)
    VForm->hiddenLine->setOn(true);
  else if( graphics == 2)
    VForm->smoothShade->setOn(true);
  else 
    VForm->wireFrame->setOn(true);

  QPoint position(pos_x,pos_y);
  QSize size(size_x,size_y);
  VForm->resize(size);
  VForm->move(position);

  // sets all the docking windows
  QString fn = QString(getenv("VERDE_DIR")) + "/.docksettings";
  QFile f( fn );
  if ( f.open( IO_ReadOnly ) )
  {
    QTextStream ts( &f );
    ts >> *(VForm );
    f.close();
  }

  VForm->show();

  // update "Windows" portion of pull-down menu
  // ModelTreeView
  if( VForm->getModelView()->isVisible() )
    VForm->Menu_Window_ModelView->setOn(true);
  else
    VForm->Menu_Window_ModelView->setOn(false);
    
  // ModelResultsTreeView
  if( VForm->getModelResultsView()->isVisible() )
    VForm->Menu_Window_ModelResultsView->setOn(true);
  else
    VForm->Menu_Window_ModelResultsView->setOn(false);
  
  // TextBox
  if( textbox->isVisible() )
    VForm->Menu_Window_CommandLine->setOn(true);
  else
    VForm->Menu_Window_CommandLine->setOn(false);


  // set point size
  if( point_size <= 0.0 )
  {
    point_size = (float)(GLWindow::first_instance()->width() + GLWindow::first_instance()->height()) / 300.;
  }
 
  GLWindow::first_instance()->set_point_size( point_size );

}

void VerdeGUIApp::write_gui_settings()
{

  QSettings settings;

  settings.writeEntry( "/verde/main_window x", VForm->x() );
  settings.writeEntry( "/verde/main_window y", VForm->y() );
  settings.writeEntry( "/verde/main_window width", VForm->width() );
  settings.writeEntry( "/verde/main_window height", VForm->height() );

  settings.writeEntry( "/verde/edit/mouse left_button", left_button );
  settings.writeEntry( "/verde/edit/mouse middle_button", middle_button );
  settings.writeEntry( "/verde/edit/mouse right_button", right_button );

  // save settings on exit 
  if( VForm->saveSettings->isOn() )
    settings.writeEntry( "/verde/edit/settings save_gui_settings", true );
  else
    settings.writeEntry( "/verde/edit/settings save_gui_settings", false );

  // zoom direction 
  if( zoom_dir)
    settings.writeEntry( "/verde/edit/mouse zoom_direction", true);
  else
    settings.writeEntry( "/verde/edit/mouse zoom_direction", false);

  // normals
  if( VForm->showNormals->isOn())
    settings.writeEntry( "/verde/display normals", 1 );
  else
    settings.writeEntry( "/verde/display normals", 0 );

  // skin
  if( VForm->showSkin->isOn())
    settings.writeEntry( "/verde/display skin", 1 );
  else
    settings.writeEntry( "/verde/display skin", 0 );

  // edges
  if( VForm->showEdges->isOn())
    settings.writeEntry( "/verde/display edges", 1 );
  else
    settings.writeEntry( "/verde/display edges", 0 );

  // graphics mode
  if( VForm->wireFrame->isOn() )
    settings.writeEntry( "/verde/display graphics", 0 );
  else if( VForm->hiddenLine->isOn() )
    settings.writeEntry( "/verde/display graphics", 1 );
  else
    settings.writeEntry( "/verde/display graphics", 2 );

  // perspective view
  if( !VForm->perspectiveView->isOn() )
    settings.writeEntry( "/verde/view orthographic", true);
  else
    settings.writeEntry( "/verde/view orthographic", false);

  // autofit 
  if( VForm->autoFit->isOn() )
    settings.writeEntry( "/verde/view autofit", true);
  else
    settings.writeEntry( "/verde/view autofit", false);


  settings.writeEntry("/verde/view quicktransform", VForm->quickTransform->isOn());

  // point size
  settings.writeEntry( "/verde/display pointsize", GLWindow::first_instance()->get_point_size()); 

  // sets all the docking windows
  QString fn = QString(getenv("VERDE_DIR")) + "/.docksettings";
  QFile f( fn );
  f.open( IO_WriteOnly );
  QTextStream ts( &f );
  ts << *(VForm);
  f.close();

}

GUITextBox* VerdeGUIApp::textBox()
{
  return textbox;
}

VerdeBoolean VerdeGUIApp::set_working_directory( const char * directory )
{
  //get size of 'directory'
  int length = strlen( directory );

  if(length == 0)
    return VERDE_FALSE;

  //delete old working dir
  if(workingDirectory != 0)
    delete [] workingDirectory;

  //new enough space for 'directory'
  workingDirectory = new char[length+1];

  //copy over 'directory'
  strcpy( workingDirectory, directory );

  return VERDE_TRUE;
}



