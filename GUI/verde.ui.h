#include <qmessagebox.h>
#include <qprocess.h>
#include "GLTextTool.hpp"

#ifdef WIN32
#include <stdlib.h>
#endif

void VerdeForm::init()
{

    ModelView = new QDockWindow(QDockWindow::InDock , this);
    ModelTreeView* modeltreeview = new ModelTreeView(ModelView);
    modeltreeview->showMaximized();
    ModelView->setWidget(modeltreeview);
    ModelView->setCloseMode(QDockWindow::Always);
    ModelView->setResizeEnabled(true);
    ModelView->setCaption( trUtf8( "Model" ) );
    moveDockWindow( ModelView, Left );
 
    ModelResultView = new QDockWindow(QDockWindow::InDock , this);
    ModelResultsTreeView* modelresultstreeview = new ModelResultsTreeView(ModelResultView);
    modelresultstreeview->showMaximized();
    ModelResultView->setWidget(modelresultstreeview);
    ModelResultView->setCloseMode(QDockWindow::Always);
    ModelResultView->setResizeEnabled(true);
    ModelResultView->setCaption( trUtf8( "Model Results" ) );
    moveDockWindow( ModelResultView, Left );
 
    GUITextBoxView = new QDockWindow(QDockWindow::InDock , this);
    GUITextBoxWindow = new GUITextBox(GUITextBoxView);
    GUITextBoxWindow->showMaximized();
    GUITextBoxView->setWidget(GUITextBoxWindow);
    GUITextBoxView->setCloseMode(QDockWindow::Always);
    GUITextBoxView->setResizeEnabled(true);
    GUITextBoxView->setCaption( trUtf8( "Command Line" ) );
    moveDockWindow( GUITextBoxView, Bottom );         

    // connect the signals so that our menu gets updated when a user clicks the "x" button.
    connect(ModelView, SIGNAL( visibilityChanged(bool) ), Menu_Window_ModelView, SLOT( setOn(bool) ));
    connect(GUITextBoxView, SIGNAL( visibilityChanged(bool) ), Menu_Window_CommandLine, SLOT( setOn(bool) ));
    connect(ModelResultView, SIGNAL( visibilityChanged(bool) ), Menu_Window_ModelResultsView, SLOT( setOn(bool) ));
    
}

void VerdeForm::destroy( bool, bool)
{

}

void VerdeForm::ModelView_toggle(bool toggle)
{
    if(toggle == false)
	ModelView->hide();
    if(toggle == true)
	ModelView->show();   
}

void VerdeForm::ModelResultView_toggle(bool toggle)
{
    if(toggle == false)
	ModelResultView->hide();
    if(toggle == true)
	ModelResultView->show();
    
}

void VerdeForm::CommandLineView_toggle(bool toggle)
{
    if(toggle == false)
	GUITextBoxView->hide();
    if(toggle == true)
	GUITextBoxView->show();    
}

void VerdeForm::Graphics_Toolbar_toggle(bool )
{
    
}

void VerdeForm::fileExit()
{
  GUICommandHandler::instance()->add_command_string("exit");
  //QApplication::exit(0);
}

void VerdeForm::fileNew()
{

}
/*
void VerdeForm::set_WorkingDirectory()
{
    QString working_directory = QFileDialog::getExistingDirectory(verde_gui_app->get_working_directory(),
					      this, 
					      "get existing directory",
					      "Set Working Directory", TRUE );
    if( working_directory != QString::null )
	verde_gui_app->set_working_directory( working_directory );
}
*/

void VerdeForm::fileOpenMesh()
{   
    QString command = "open '";

    OpenMeshFileDialog file_dialog( verde_gui_app->get_working_directory() );          
    QString str = file_dialog.getOpenFileName();
    
    // set working directory
    QString directory = file_dialog.dirPath();
    verde_gui_app->set_working_directory(directory);
    
    if(str.length() == 0)
	return;
    
    if(file_dialog.SpecifyBlocks())
    {
	    command.append(str);
            command.append("'");

            if(file_dialog.AddMesh())
              command.append(" add");

	    command.append(" block_info");
	    GUICommandHandler::instance()->add_command_string(command);
	    GUITextBoxWindow->insert_prompt();
	    BlockImport block_import;
	    if(block_import.exec() == QDialog::Accepted )
	    {
		std::list<int> blocks;
		block_import.get_blocks(blocks);
		char command[100];
		while(blocks.size())
		{
		    
		    //turn off the normals button if it is on
		    showNormals->setOn(FALSE); 
		    int id = blocks.front();
		    blocks.pop_front();
		    sprintf(command, "block %i on", id);
		    GUICommandHandler::instance()->add_command_string(command, VERDE_FALSE);
		}
		GLWindow::first_instance()->reset();
                GLWindow::first_instance()->center_model();
	    }
     }
    else
    {
	    command.append(str);
	    command.append("'");
	    // echo the command to the text box    
	    //GUITextBoxWindow->append( (const char*) command);
            	   
            if(file_dialog.AddMesh())
              command.append(" add");
    
	    //turn off the normals button if it is on
	    //showNormals->setOn(FALSE); 
	    // do the command
	    GUICommandHandler::instance()->add_command_string( (const char*)command);
	    // add prompt here
	    GUITextBoxWindow->insert_prompt();
	}

}

void VerdeForm::filePlayJournal()
{
    QString command = "playback '";
   
    QString str = QFileDialog::getOpenFileName( verde_gui_app->get_working_directory(),
				                "Journal Files (*.jou *.j)",  
				                 this,
                                                "zzzzzzz",  
				                "Choose a jounal file to play");

    // set working directory
    QFileInfo file_info( str );
    QString directory = file_info.dirPath();
    verde_gui_app->set_working_directory(directory);
    
    if(str.length() == 0)
	return;
    
    command.append(str);
    command.append("\n");
    // echo the command to the text box
    GUITextBoxWindow->append( (const char*) command );
    
    // do the command
    GUICommandHandler::instance()->add_command_string( (const char*)command );
    
    // gotta add the prompt here
    GUITextBoxWindow->insert_prompt();
    
}

void VerdeForm::save_mesh()
{

    QString file_name = QFileDialog::getSaveFileName( 
        verde_gui_app->get_working_directory(),
        "Mesh Files (*.g; *.gen; *.exoII; *.exo)",
        this,
        "zzzzzzz",  
        "Save Mesh File." );
  
    // set working directory
    QFileInfo file_info( file_name );
    QString directory = file_info.dirPath();
    verde_gui_app->set_working_directory(directory);
 
    if(file_name.length() == 0) 
	return;
    
    if( !(file_info.extension(false).lower() == "gen" ||
        file_info.extension(false).lower() == "g" ||
        file_info.extension(false).lower() == "exo" ||
        file_info.extension(false).lower() == "exoii"))
    {
      file_name = file_name + ".gen";
    }
      
    QString command = "save exodus '";
    file_name += "'";
    command += file_name;
		
    GUICommandHandler::instance()->add_command_string( (const char*)command);
    // add prompt here
    GUITextBoxWindow->insert_prompt();
}
    

void VerdeForm::filePrint()
{
   QPrinter printer(QPrinter::PrinterResolution);

   QColor bgcolor = GLWindow::first_instance()->get_background_color();
   //QColor fgcolor = GLWindow::first_instance()->get_foreground_color();
    
   // bring up the print dialog
   if ( printer.setup( this ) ) 
   {
      // For greyscale images the user probably??? wants a 
      // white background
      if (QPrinter::GrayScale == printer.colorMode())
      {
         if (black == bgcolor)
         { 
            switch( QMessageBox::information( this, "Verde",
              "This will print the image with a black background.\n"
              "Do you want to print with a white background?",
              "&Yes", "&No", "Cancel",
              0,      // Enter == button 0
              2 ) )   // Escape == button 2
            {
                case 0: // Yes clicked or Alt+Y pressed or Enter pressed.
                  // Set colors and redraw.  This will cause the screen to
                  // flash white.  TODO:  Fix the flash.  Look into using
                  // renderPixmap instead of grabFrameBuffer. 
                  GLWindow::first_instance()->set_background_color( white );
                  //GLWindow::first_instance()->set_foreground_color( black );
                  GLWindow::first_instance()->updateGL();
                  break;
                case 1: // Discard clicked or Alt+N pressed
                  // Print as is
                  break;
                case 2: // Cancel clicked or Alt+C pressed or Escape pressed
                  return; // Just leave.
                  break;
            }
         }
      }

      // render the image with the specified colors.
      const QImage image = GLWindow::first_instance()->grabFrameBuffer(); 

      QPainter painter(&printer);

      // print it.  Use the orientation specified.  The image
      // will fill the page.
      painter.drawPixmap(0,0,image);

      // reset the background colors if they were changed
      GLWindow::first_instance()->set_background_color( bgcolor );
      //GLWindow::first_instance()->set_foreground_color( fgcolor );

      GLWindow::first_instance()->updateGL();

   }
}

void VerdeForm::fileRecordJournal()
{
    QString command = "record '";
    QString str = QFileDialog::getSaveFileName( "./",   
                                              "Journal Files (*.jou *.j)",   
                                              this,   
                                              "open file dialog",  
                                              "Choose or name a journal file to record to" );
    // set working directory
    QFileInfo file_info( str );
    QString directory = file_info.dirPath();
    verde_gui_app->set_working_directory(directory);
 
    if(str.length() == 0) 
	return;
    if(str.find('.') == -1)
	str.append(".jou");    
    
    command.append(str); 
    command.append("\n"); 
    // echo the command to the text box 
    GUITextBoxWindow->append( (const char*) command ); 
     
    // do the command 
    GUICommandHandler::instance()->add_command_string( (const char*)command );
     
    // gotta add the prompt here 
    GUITextBoxWindow->insert_prompt();
 
}

void VerdeForm::fileSave()
{

}

void VerdeForm::fileSaveAs()
{

}

//! Open the preferences dialog.
void VerdeForm::editPreferences()
{
    // Create a new dialog if needed otherwise make sure
    // that the dialog is in sync with the command line 
    // settings.

    if ( !preferencesDialog ) 
	preferencesDialog = new PreferencesDialog( this );
    else
        preferencesDialog->init();
    
   preferencesDialog->show();
}
//! Open the preferences dialog.
void VerdeForm::editMouseButtons()
{
    if ( !mouseDialog ) 
	mouseDialog = new MouseMap( this );
    else
        mouseDialog->init();
    
   mouseDialog->show();
}
void VerdeForm::ediMetricRanges()
{
    if ( !metricRangeDialog ) 
	metricRangeDialog = new MetricRangeDialog( this );
   
   metricRangeDialog->show();
}

void VerdeForm::helpAbout()
{
    aboutBox AboutBox;
    AboutBox.exec();
}

void VerdeForm::helpContents()
{
  QString path = QString( getenv( "VERDE_DIR" ) );
  if (path != NULL)
  {
    QString run_help = path + "/verde_help";
    QProcess proc( run_help );
    if (proc.start())
      return;
  }
  // else fall through to an error state
  QMessageBox::information( this, "Help Error", 
         "Unable start help system.\n  Check configuration");
  return;	
}

void VerdeForm::helpIndex()
{

}

GUITextBox* VerdeForm::getTextBox()
{
    return GUITextBoxWindow;
}


void VerdeForm::secondary_init()
{
    
    GLWindowView->updateGL();

    DrawingTool::instance()->reset();
  
   // we shouldn't need to test for batch mode
    // because we should never get here anyways.
//    if( !(UserInterface::instance()->GetBatchMode()) )
   GLWindow::first_instance()->reset();
 
    if(verde_app->mesh())
    {
	if(getModelResultsView())
	    getModelResultsView()->calculateButton->setEnabled(1);
	DrawingTool::instance()->add_surface_mesh();
	DrawingTool::instance()->add_model_edges();
	DrawingTool::instance()->add_surface_normals();
	GLWindow::first_instance()->center_model();


        // these checks are here because the settings read in 
	// in the verderc file set the buttons before these drawing 
	// are actually made.  This sets the DrawingList visibility
	// flags according to the buttons. 
         
        if(!showSkin->isOn())
	  DrawingTool::instance()->toggle_skin_vis(0); 

        if(!showEdges->isOn())
	  DrawingTool::instance()->toggle_model_edges_vis(0); 

        if(!showNormals->isOn())
	  DrawingTool::instance()->toggle_normals_vis(0);

	getModelView()->get_model_info();
    }
	
}



 
void VerdeForm::Wireframe_clicked(bool value)
{
    if(value) 
	GUICommandHandler::instance()->add_command_string("graphics mode wireframe", VERDE_FALSE);
}
 
void VerdeForm::Hiddenline_clicked(bool value)
{
    if(value)
 	GUICommandHandler::instance()->add_command_string("graphics mode hiddenline", VERDE_FALSE);
}
 
void VerdeForm::Smoothshade_clicked(bool value)
{
    if(value)
	GUICommandHandler::instance()->add_command_string("graphics mode smoothshade", VERDE_FALSE);
}          


ModelTreeView* VerdeForm::getModelView()
{
    return static_cast<ModelTreeView*>(ModelView->widget());
}

ModelResultsTreeView* VerdeForm::getModelResultsView()
{
    return static_cast<ModelResultsTreeView*>(ModelResultView->widget());
}

GUITextBox* VerdeForm::getGUITextBoxWindow()
{
   return GUITextBoxWindow;
} 
 
void VerdeForm::Reset_clicked()
{
   GUICommandHandler::instance()->add_command_string("graphics reset", VERDE_FALSE);
}

void VerdeForm::BackgroundColor_clicked()
{
    QColor color = QColorDialog::getColor(black, this);
    if (color.isValid())
    {
	GLWindow::first_instance()->set_background_color( color );
	GLWindow::first_instance()->updateGL();
    }
}

void VerdeForm::LabelFont_clicked()
{

  GLTextTool& gltext = DrawingTool::instance()->textTool;
 
   bool ok;
    QFont font = QFontDialog::getFont( &ok, gltext.current_font(), this );
    if ( ok ) 
    {
      // font is set to the font the user selected
      gltext.build_bitmap_font(GLWindow::first_instance(), font);
      GLWindow::first_instance()->updateGL();
    } 
    else 
    {
        // the user cancelled the dialog; 
        // the font is not changed
    }
    
}

void VerdeForm::fileImageSave()
{
    QString selected_filter;

    QString file_name = QFileDialog::getSaveFileName( verde_gui_app->get_working_directory(),   
                                              "PNG file *.png;;JPEG File *.jpg, *.jpeg",   
                                              this,   
                                              "save file dialog",  
                                              "Choose an image file.",
                                              &selected_filter );
    
    // set working directory
    QFileInfo file_info( file_name );
    QString directory = file_info.dirPath();
    verde_gui_app->set_working_directory(directory);

    if(file_name.length() == 0) 
	return;

    const QImage image = GLWindow::first_instance()->grabFrameBuffer(); 

    // save the file in either png or jpeg format
    if (selected_filter.contains( "PNG", TRUE ))
    {
      if (file_name.contains(".png", FALSE) == 0)
          file_name = file_name + ".png";

      image.save(file_name.latin1(), "PNG");
    }
    else
    {
      if (file_name.contains(".jpg", FALSE) == 0 && 
          file_name.contains(".jpeg", FALSE) == 0 )
      {
          file_name = file_name + ".jpg";
      }

      image.save(file_name.latin1(), "JPEG");

    }
}



void VerdeForm::endRecordJournalFile()
{

    // echo the command to the text box 
    GUITextBoxWindow->append( "record stop\n" ); 

    // do the command 
   GUICommandHandler::instance()->
             add_command_string("record stop", VERDE_FALSE);
   
    // gotta add the prompt here 
    GUITextBoxWindow->insert_prompt();
 
}

void VerdeForm::showAxis(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("axis on", VERDE_FALSE);   
    }	

    else
    {
	GUICommandHandler::instance()->add_command_string("axis off", VERDE_FALSE);
    }   
}

void VerdeForm::showNormals_clicked(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("normals on", VERDE_FALSE);   
    }	

    else
    {
	GUICommandHandler::instance()->add_command_string("normals off", VERDE_FALSE);
    }
}
void VerdeForm::labelNodes_clicked(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("label nodes on", VERDE_FALSE);   
    }	
    else
    {
	GUICommandHandler::instance()->add_command_string("label nodes off", VERDE_FALSE);
    }
}

void VerdeForm::labelElements_clicked(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("label elements on", VERDE_FALSE);       }	
    else
    {
	GUICommandHandler::instance()->add_command_string("label elements off", VERDE_FALSE);
    }
}

void VerdeForm::labelFailed_clicked(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("label failed on", VERDE_FALSE);   
    }	
    else
    {
	GUICommandHandler::instance()->add_command_string("label failed off", VERDE_FALSE);
    }
}

void VerdeForm::labelNodesets_clicked(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("label nodesets on", VERDE_FALSE);   
    }	

    else
    {
	GUICommandHandler::instance()->add_command_string("label nodesets off", VERDE_FALSE);
    }
}

void VerdeForm::labelSidesets_clicked(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("label sidesets on", VERDE_FALSE);   
    }	

    else
    {
	GUICommandHandler::instance()->add_command_string("label sidesets off", VERDE_FALSE);
    }
}

void VerdeForm::labelBlocks_clicked(bool on_off_flag)
{
    if(on_off_flag)
    {
	GUICommandHandler::instance()->add_command_string("label blocks on", VERDE_FALSE);   
    }	

    else
    {
	GUICommandHandler::instance()->add_command_string("label blocks off", VERDE_FALSE);
    }
}

void VerdeForm::showSkin_clicked( bool on_off_flag )
{
    if( on_off_flag )
    {
	GUICommandHandler::instance()->add_command_string("skin on", VERDE_FALSE);
    }
    else
    {
	GUICommandHandler::instance()->add_command_string("skin off", VERDE_FALSE);
    }
}


void VerdeForm::showEdges_clicked( bool on_off_flag )
{
    
    if( on_off_flag )
    {
	GUICommandHandler::instance()->add_command_string("model edges on", VERDE_FALSE);
    }
    else
    {
	GUICommandHandler::instance()->add_command_string("model edges off", VERDE_FALSE);
    }
}

void VerdeForm::perspective_view( bool on_off_flag)
{
     if( on_off_flag )
    {
	GUICommandHandler::instance()->add_command_string("graphics perspective on", VERDE_FALSE);
    }
    else
    {
	GUICommandHandler::instance()->add_command_string("graphics perspective off", VERDE_FALSE);
    }
}

void VerdeForm::auto_fit( bool on_off_flag)
{
     if( on_off_flag )
    {
	GUICommandHandler::instance()->add_command_string("set autofit on", VERDE_FALSE);
    }
    else
    {
	GUICommandHandler::instance()->add_command_string("set autofit off", VERDE_FALSE);
    }
}




void VerdeForm::save_settings( bool on_off_flag )
{
    if( on_off_flag )
      verde_gui_app->saveGUISettings( VERDE_TRUE );
    else 
      verde_gui_app->saveGUISettings( VERDE_FALSE );
}



void VerdeForm::quickTransformtoggle( bool val )
{
    if(val)
	GUICommandHandler::instance()->add_command_string("set quick transform on", VERDE_FALSE);
    else
	GUICommandHandler::instance()->add_command_string("set quick transform off", VERDE_FALSE);
}
