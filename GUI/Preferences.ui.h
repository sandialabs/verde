/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
#include "../VerdeApp.hpp"
#include "VerdeGUIApp.hpp"
#include "GUITextBox.hpp"
#include "../UserInterface.hpp"
#include "../SkinTool.hpp"
#include "../Metric.hpp"
#include "GLWindow.hpp"

//! Set the GUI to match any command line flags at startup
void PreferencesDialog::init()
{
  VerdeApp *app = verde_app;

  topology->setChecked(app->get_topology());
  interfaceChk->setChecked(app->get_interface());
  metrics->setChecked(app->get_metrics());        
  verbose->setChecked(app->get_print_failed_elements());
  individual->setChecked(app->get_individual());
  algebraic->setChecked(Metric::algebraic());
  diagnostic->setChecked(Metric::diagnostic());
  traditional->setChecked(Metric::traditional());
  echo->setChecked(UserInterface::instance()->command_is_echoed());
  quads3nodes->setCurrentItem(SkinTool::get_quads_sharing_three_nodes_completeness());
  pointSizeBox->setValue( GLWindow::first_instance()->get_point_size() );
  
    //warning_level->setValue(app->get_warning_level());
}


int PreferencesDialog::exec()
{
    init();
    return QDialog::exec();
}



//! Slot connected to the OK button.
void PreferencesDialog::ok()
{
   int command_issued = 0;
    //! Get all the values from the dialog box and set them.
   QString topology_command = "set topology_calculations ";
   if( topology->isChecked() && !verde_app->get_topology() )
   {
       topology_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)topology_command);
       command_issued++;
   }
   else if( !topology->isChecked() && verde_app->get_topology() )
   {
       topology_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)topology_command);
       command_issued++;
   }

   QString interface_command = "set interface_calculations ";
   if( interfaceChk->isChecked() && !verde_app->get_interface() )
   {
       interface_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)interface_command);
       command_issued++;
   }
   else if( !interfaceChk->isChecked() && verde_app->get_interface() )
   {
       interface_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)interface_command);
       command_issued++;
   }
    
   QString metrics_command = "set metric_calculations ";
   if( metrics->isChecked() && !verde_app->get_metrics() )
   {
       metrics_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)metrics_command);
       command_issued++;
   }
   else if( !metrics->isChecked() && verde_app->get_metrics() )
   {
       metrics_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)metrics_command);
       command_issued++;
   }
  
   QString algebraic_command = "set algebraic_calculations ";
   if( algebraic->isChecked() && !Metric::algebraic() )
   {
       algebraic_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)algebraic_command);
       command_issued++;
   }
   else if( !algebraic->isChecked() && Metric::algebraic() )
   {
       algebraic_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)algebraic_command);
       command_issued++;
   }
     
   QString diagnostic_command = "set diagnostic_calculations ";
   if( diagnostic->isChecked() && !Metric::diagnostic() )
   {
       diagnostic_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)diagnostic_command);   
       command_issued++;
   }
   else if( !diagnostic->isChecked() && Metric::diagnostic() )
   {
       diagnostic_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)diagnostic_command);
       command_issued++;
   }
   QString traditional_command = "set traditional_calculations ";
   if( traditional->isChecked() && !Metric::traditional() )
   {
       traditional_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)traditional_command);  
       command_issued++;
   }
   else if( !traditional->isChecked() && Metric::traditional() )
   {
       traditional_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)traditional_command);
       command_issued++;
   }

    
   QString verbose_command = "set verbose_model ";
   if( verbose->isChecked() && !verde_app->get_print_failed_elements() )
   {
       verbose_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)verbose_command);   
       command_issued++;
   }
   else if( !verbose->isChecked() && verde_app->get_print_failed_elements() )
   {
       verbose_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)verbose_command);
       command_issued++;
   }

   QString individual_command = "set individual_output ";
   if( individual->isChecked() && !verde_app->get_individual() )
   {
       individual_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)individual_command);   
       command_issued++;
   }
   else if( !individual->isChecked() && verde_app->get_individual() )
   {
       individual_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)individual_command);
       command_issued++;
   }

   QString echo_command = "echo ";
   if( echo->isChecked() && !UserInterface::instance()->command_is_echoed() )
   {
       echo_command.append("on");
       GUICommandHandler::instance()->add_command_string( (const char*)echo_command);   
       command_issued++;
   }
   else if( !echo->isChecked() && UserInterface::instance()->command_is_echoed() )
   {
       echo_command.append("off");
       GUICommandHandler::instance()->add_command_string( (const char*)echo_command);
       command_issued++;
   }

   int point_size = pointSizeBox->value();
   if( point_size != GLWindow::first_instance()->get_point_size())
   {
       char command[100];
       sprintf(command, "set point size %d", point_size);
       GUICommandHandler::instance()->add_command_string( command );
       command_issued++;
   }
    

   int level_item = quads3nodes->currentItem();
   if( level_item != SkinTool::get_quads_sharing_three_nodes_completeness() )
   {
	QString quads3nodes_command = "set quads_share_three_nodes "; 
	if(level_item == 0)
	    quads3nodes_command.append(QString("none"));
	else if(level_item == 1)
	    quads3nodes_command.append(QString("exterior"));
	else if(level_item == 2)
	    quads3nodes_command.append(QString("all"));
	GUICommandHandler::instance()->add_command_string( (const char*)quads3nodes_command);
        command_issued++;
   }

   QString default_file_command = "load defaults '";
   QString ini_file = metrics_range_file->text();

   if (ini_file.length() != 0)
   {
       default_file_command.append(ini_file);
       
       // TODO: what do we do about echoing the command lines?
       // do the command
       GUICommandHandler::instance()->add_command_string( (const char*)default_file_command);
       command_issued++;
   }
   if( command_issued )
     verde_gui_app->textBox()->insert_prompt(); 

   // Close the dialog box
   accept();
   return;
}

 
//! Slot connected to the browse button.
void PreferencesDialog::browse()
{
    // Open the file dialog box
    QString str = QFileDialog::getOpenFileName("./",
    				"Default Metrics Range Files (*.ini)",
    				this,
    				"open file dialog",
    				"Select Default Metrics Range File");
    
    if(str.length() == 0)
	return;

    // Set the string in the line edit widget.
    metrics_range_file->setText(str);

    return;
}

void PreferencesDialog::on_off_metrics( bool value )
{
 metrics_group->setEnabled(value);   
 //  algebraic->setEnabled(value);
  // diagnostic->setEnabled(value);
 //  traditional->setEnabled(value);
}
    

