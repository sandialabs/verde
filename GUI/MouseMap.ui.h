/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
#include <qmessagebox.h>
#include "VerdeGUIApp.hpp"
void MouseMap::init()
{
   int left_button = verde_gui_app->leftButton();
   if(left_button == 0)
       rot_lmb->setChecked(true);
   else if(left_button == 1)
       zoom_lmb->setChecked(true);
   else
       trans_lmb->setChecked(true);
   
   int middle_button = verde_gui_app->middleButton();
   if(middle_button == 0)
       rot_mmb->setChecked(true);
   else if(middle_button == 1)
       zoom_mmb->setChecked(true);
   else
       trans_mmb->setChecked(true);
   
   int right_button = verde_gui_app->rightButton();
   if(right_button == 0)
       rot_rmb->setChecked(true);
   else if(right_button == 1)
       zoom_rmb->setChecked(true);
   else
       trans_rmb->setChecked(true);

   if( verde_gui_app->zoomDir() )
      zoom_neg_up->setChecked(true);
   else
      zoom_pos_up->setChecked(true);	
   
   return;   
}

void MouseMap::changing_lmb( int changed_id )	
{
    //gets the selected button in middle_mouse button group 
    QButton *m_button = middle_mouse->selected();
    int mmb_id = middle_mouse->id(m_button);
    
    //see if it is matches middle mouse button
    if(mmb_id ==  changed_id)
    {
	// not going to change right mouse button
	QButton *r_button = right_mouse->selected();
	int id_to_keep = right_mouse->id(r_button);
	int id_to_change;
	
	if( changed_id == 0 && id_to_keep == 1)	    
	    id_to_change = 2;
	else if( changed_id == 1 && id_to_keep == 0)
	    id_to_change = 2;
	else if( changed_id == 0 && id_to_keep == 2)
	    id_to_change = 1;
	else if (changed_id ==2 && id_to_keep == 0)
	    id_to_change = 1;	    
	else
	    id_to_change = 0;
	
	// turn on button
	QButton *button_to_change = middle_mouse->find(id_to_change);
	if( !button_to_change->isDown() )
	    button_to_change->toggle();
	
	return;
    }
    
    //gets the selected button in right_mouse button group
    QButton *r_button = right_mouse->selected();
    int rmb_id = right_mouse->id(r_button);
    
    if(rmb_id ==  changed_id)
    {
	QButton *m_button = middle_mouse->selected();
	int id_to_keep = middle_mouse->id(m_button);
	int id_to_change;
		
	if( changed_id == 0 && id_to_keep == 1)	    
	    id_to_change = 2;
	else if( changed_id == 1 && id_to_keep == 0)
	    id_to_change = 2;
	else if( changed_id == 0 && id_to_keep == 2)
	    id_to_change = 1;
	else if (changed_id ==2 && id_to_keep == 0)
	    id_to_change = 1;	    
	else
	    id_to_change = 0;
	
	// turn on button
	QButton *button_to_change = right_mouse->find(id_to_change);
	if( !button_to_change->isDown() )
	    button_to_change->toggle();
     }
}


void MouseMap::change_mmb(int changed_id)
{
    //gets the selected button   
    QButton *l_button = left_mouse->selected();
    int lmb_id = left_mouse->id(l_button);
    
    //see if it is matches left mouse button
    if(lmb_id ==  changed_id)
    {
	// not going to change right mouse button
	QButton *l_button = right_mouse->selected();
	int id_to_keep = right_mouse->id(l_button);
	int id_to_change;
	
	if( changed_id == 0 && id_to_keep == 1)	    
	    id_to_change = 2;
	else if( changed_id == 1 && id_to_keep == 0)
	    id_to_change = 2;
	else if( changed_id == 0 && id_to_keep == 2)
	    id_to_change = 1;
	else if (changed_id ==2 && id_to_keep == 0)
	    id_to_change = 1;	    
	else
	    id_to_change = 0;
	
	// turn on button
	QButton *button_to_change = left_mouse->find(id_to_change);
	if( !button_to_change->isDown() )
	    button_to_change->toggle();
	
	return;
    }
    
     //gets the selected button in right_mouse button group
    QButton *r_button = right_mouse->selected();
    int rmb_id = right_mouse->id(r_button);
    
    if(rmb_id ==  changed_id)
    {
	
    	QButton *m_button = left_mouse->selected();
	int id_to_keep = left_mouse->id(m_button);
	int id_to_change;
	
	if( changed_id == 0 && id_to_keep == 1)	    
	    id_to_change = 2;
	else if( changed_id == 1 && id_to_keep == 0)
	    id_to_change = 2;
	else if( changed_id == 0 && id_to_keep == 2)
	    id_to_change = 1;
	else if (changed_id ==2 && id_to_keep == 0)
	    id_to_change = 1;	    
	else
	    id_to_change = 0;
	
	// turn on button
	QButton *button_to_change = right_mouse->find(id_to_change);
	if( !button_to_change->isDown() )
	    button_to_change->toggle();
     }
}

void MouseMap::change_rmb(int changed_id)
{
    //gets the selected button   
    QButton *m_button = middle_mouse->selected();
    int mmb_id = middle_mouse->id(m_button);   
       
    //see if it is matches middle mouse button
    if(mmb_id ==  changed_id)
    {
	// not going to change left mouse button
	QButton *l_button = left_mouse->selected();
	int id_to_keep = left_mouse->id(l_button);
	int id_to_change;
	
	if( changed_id == 0 && id_to_keep == 1)	    
	    id_to_change = 2;
	else if( changed_id == 1 && id_to_keep == 0)
	    id_to_change = 2;
	else if( changed_id == 0 && id_to_keep == 2)
	    id_to_change = 1;
	else if (changed_id ==2 && id_to_keep == 0)
	    id_to_change = 1;	    
	else
	    id_to_change = 0;
	
	// turn on button
	QButton *button_to_change = middle_mouse->find(id_to_change);
	if( !button_to_change->isDown() )
	    button_to_change->toggle();
	
	return;
    }
    
     //gets the selected button in right_mouse button group
    QButton *l_button = left_mouse->selected();
    int lmb_id = left_mouse->id(l_button);
    
    if(lmb_id ==  changed_id)
    {
	QButton *m_button = middle_mouse->selected();
	int id_to_keep = middle_mouse->id(m_button);
	int id_to_change;
	
	if( changed_id == 0 && id_to_keep == 1)	    
	    id_to_change = 2;
	else if( changed_id == 1 && id_to_keep == 0)
	    id_to_change = 2;
	else if( changed_id == 0 && id_to_keep == 2)
	    id_to_change = 1;
	else if (changed_id ==2 && id_to_keep == 0)
	    id_to_change = 1;	    
	else
	    id_to_change = 0;
	
	// turn on button
	QButton *button_to_change = left_mouse->find(id_to_change);
	if( !button_to_change->isDown() )
	    button_to_change->toggle();
     }
}


void MouseMap::ok()
{
 //check to see if there are conflicts
    if(  (rot_lmb->isChecked() && rot_mmb->isChecked() ) ||
         (rot_lmb->isChecked() && rot_rmb->isChecked() ) ||
         (rot_mmb->isChecked() && rot_rmb->isChecked() ) )
    {
	error_message();
	return;
    }
    else if(  (zoom_lmb->isChecked() && zoom_mmb->isChecked() ) ||
                (zoom_lmb->isChecked() && zoom_rmb->isChecked() ) ||
                (zoom_mmb->isChecked() && zoom_rmb->isChecked() ) )
    {
	error_message();
	return;
    }
    else if(  (trans_lmb->isChecked() && trans_mmb->isChecked() ) ||
               (trans_lmb->isChecked() && trans_rmb->isChecked() ) ||
	   (trans_mmb->isChecked() && trans_rmb->isChecked() ) )
     {
	error_message();
	return;
    }
    else
    {
	// Rotate = 0
	if( rot_lmb->isChecked() )
	    verde_gui_app->set_leftButton(0);
	else if( rot_mmb->isChecked() )
	    verde_gui_app->set_middleButton(0);	
	else 
    	    verde_gui_app->set_rightButton(0);
	
	//Zoom = 1
	if( zoom_lmb->isChecked() )
	    verde_gui_app->set_leftButton(1);
	else if( zoom_mmb->isChecked() )
	    verde_gui_app->set_middleButton(1);	
	else 
    	    verde_gui_app->set_rightButton(1);
	
	//Translate = 2
	if( trans_lmb->isChecked() )
	    verde_gui_app->set_leftButton(2);
	else if( trans_mmb->isChecked() )
	    verde_gui_app->set_middleButton(2);	
	else 
    	    verde_gui_app->set_rightButton(2);		
	accept();
    }

  //check zoom direction
  if( zoom_neg_up->isChecked() )
    verde_gui_app->set_zoomDir(VERDE_TRUE);
  else
    verde_gui_app->set_zoomDir(VERDE_FALSE);
  
  return; 

}
void MouseMap::error_message()
{
    QMessageBox::warning( this, "Error", "Two or more buttons identically set!", QMessageBox::Ok, 
			  QMessageBox::NoButton,
			  QMessageBox::NoButton );
}

