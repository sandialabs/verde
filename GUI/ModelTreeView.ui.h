/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


#include <deque>
// The standard is to use std::string when the string is not being
// passed into Qt otherwise use QString.  However, there is enough
// combination here of QString and std::string that I am concerned
// about combining the two.  Therefore, all strings in this module
// are declared as QStrings.
//#include <string> 

void ModelTreeView::init()
{
  // we don't want to indent too much, so lessen it
  ModelListView->setTreeStepSize(15);

  //create the action group   
  blockGroupAction = new QActionGroup( this, "blockActivate" );
      
  //create activate action
  blockActivateAction = new QAction( blockGroupAction, "activate" );
  blockActivateAction->setText("Activate");
  
  //create deactivate action
  blockDeactivateAction = new QAction( blockGroupAction, "deactivate" );
  blockDeactivateAction->setText("Deactivate");
  
  //create the popup menu to place the actions into
  act_deact_menu = new QPopupMenu(this,NULL);
  
  //put action into popup menu
  blockActivateAction->addTo(act_deact_menu);
  blockDeactivateAction->addTo(act_deact_menu);
  
  //form the connection   
  connect( blockActivateAction, SIGNAL(activated() ), this, SLOT(activate_items()));
  connect( blockDeactivateAction, SIGNAL(activated() ), this, SLOT(deactivate_items()));
}

void ModelTreeView::get_model_info()
{
  // clean the ModelView
  ModelListView->clear();    
  
  QString label = verde_app->input_file().c_str();
  
  QFileInfo fi(label);

  if( verde_app->should_add_mesh() )
  {
    model_name += " ";
    model_name += fi.fileName();
  }
  else
    model_name = fi.fileName();
  
  model_item = new QListViewItem(ModelListView, model_name);
  
  model_item->setOpen(true);
  
  // get the list of ElementBlocks
  std::deque<ElementBlock*> block_list;
  verde_app->mesh()->element_blocks(block_list);
  
  // get the lists of Boundary Conditions
  std::deque<NodeBC*> node_bc_list;
  std::deque<ElementBC*> element_bc_list;    
  verde_app->mesh()->get_node_bc_list(node_bc_list);
  verde_app->mesh()->get_element_bc_list(element_bc_list);
  
  // create a parent "blocks" item
  blocks_item = new QListViewItem(model_item, "Blocks");
  active_blocks_item = new QListViewItem(blocks_item, "Active Blocks");
  inactive_blocks_item = new QListViewItem(blocks_item, "InActive Blocks");
   
  // create a parent node set item
  node_set_item = new QListViewItem(model_item, "Node Sets");
  active_ns_item = new QListViewItem(node_set_item, "Active Node Sets");
  inactive_ns_item = new QListViewItem(node_set_item, "InActive Node Sets");
      
  // create a parent side set item
  side_set_item = new QListViewItem(model_item, "Side Sets");
  active_ss_item = new QListViewItem(side_set_item, "Active Side Sets");
  inactive_ss_item = new QListViewItem(side_set_item, "InActive Side Sets");
  
  // add the individual blocks to the parent "blocks" item
  
  std::deque<ElementBlock*>::iterator iter;
  std::deque<ElementBlock*>::iterator iend = block_list.end();
  for (iter=block_list.begin(); iter != iend; iter++)
  {
    QString block_num;
    block_num.setNum((*iter)->block_id());
    QString block_name = "Block ";
    block_name.append(block_num);

    if ( (*iter)->is_active() == VERDE_TRUE)
      new QListViewItem(active_blocks_item, block_name, block_num);
    else
      new QListViewItem(inactive_blocks_item, block_name, block_num);
   }
  
  std::deque<NodeBC*>::iterator jter;
  std::deque<NodeBC*>::iterator jend = node_bc_list.end();
  for (jter=node_bc_list.begin(); jter != jend; jter++)
  {
    QString node_set_num;
    node_set_num.setNum((*jter)->id());
    QString node_set_name = "Node Set ";
    node_set_name.append(node_set_num);

    if ((*jter)->is_active())
      new QListViewItem(active_ns_item, node_set_name, node_set_num);
    else
      new QListViewItem(inactive_ns_item, node_set_name, node_set_num);
  }           
   
  std::deque<ElementBC*>::iterator kter;
  std::deque<ElementBC*>::iterator kend = element_bc_list.end();
  for (kter=element_bc_list.begin(); kter != kend; kter++)
  {
    QString element_set_num;
    element_set_num.setNum((*kter)->id());
    QString element_set_name = "Side Set ";
    element_set_name.append(element_set_num);

    if ((*kter)->is_active())
      new QListViewItem(active_ss_item, element_set_name, element_set_num);
    else
      new QListViewItem(inactive_ss_item, element_set_name, element_set_num);
  }           
}

void ModelTreeView::make_block_active(int id)
{
  // make an iterator that starts at the first inactive block item
  QListViewItemIterator iterator(inactive_blocks_item);
  iterator++;

  // the block name we are looking for
  QString block_name("Block ");
  QString num;
  num.setNum(id);
  block_name.append(num);

  // find the item that represents this block
  while(iterator.current()->parent() == inactive_blocks_item && 
      iterator.current()->text(0) != block_name )
  {
    iterator++;
  }
  
  // we found it
  if(iterator.current()->parent() == inactive_blocks_item)
  {
    // move it to the active blocks section
    QListViewItem* block_item = iterator.current();
    inactive_blocks_item->takeItem(block_item);
    active_blocks_item->insertItem(block_item);
  }
  
}

void ModelTreeView::make_block_inactive(int id)
{
  // make an iterator that starts at the first active block item
  QListViewItemIterator iterator(active_blocks_item);
  iterator++;

  // the block name we are looking for
  QString block_name("Block ");
  QString num;
  num.setNum(id);
  block_name.append(num);

  // find the item that represents this block
  while(iterator.current()->parent() == active_blocks_item &&
      iterator.current()->text(0) != block_name)
  {
    iterator++;
  }

  // if we found it
  if(iterator.current()->parent() == active_blocks_item)
  {
    QListViewItem* block_item = iterator.current();
    active_blocks_item->takeItem(block_item);
    inactive_blocks_item->insertItem(block_item);
  }
}

void ModelTreeView::make_ns_active(int id)
{
  // make an iterator that starts at the first inactive block item
  QListViewItemIterator iterator(inactive_ns_item);
  iterator++;

  // the block name we are looking for
  QString num;
  num.setNum(id);
  
  // find the item that represents this block
  while(iterator.current()->parent() == inactive_ns_item && 
      iterator.current()->text(1) != num )
  {
    iterator++;
  }
  
  // we found it
  if(iterator.current()->parent() == inactive_ns_item)
  {
    // move it to the active blocks section
    QListViewItem* ns_item = iterator.current();
    inactive_ns_item->takeItem(ns_item);
    active_ns_item->insertItem(ns_item);
  }
}


void ModelTreeView::make_ns_inactive(int id)
{
     // make an iterator that starts at the first inactive block item
  QListViewItemIterator iterator(active_ns_item);
  iterator++;

  // the block name we are looking for
  QString num;
  num.setNum(id);
  
  // find the item that represents this block
  while(iterator.current()->parent() == active_ns_item && 
      iterator.current()->text(1) != num )
  {
    iterator++;
  }
  
  // we found it
  if(iterator.current()->parent() == active_ns_item)
  {
    // move it to the active blocks section
    QListViewItem* ns_item = iterator.current();
    active_ns_item->takeItem(ns_item);
    inactive_ns_item->insertItem(ns_item);
  }
}


void ModelTreeView::make_ss_active(int id)
{
  // make an iterator that starts at the first inactive block item
  QListViewItemIterator iterator(inactive_ss_item);
  iterator++;

  // the block name we are looking for
  QString num;
  num.setNum(id);
  
  // find the item that represents this block
  while(iterator.current()->parent() == inactive_ss_item && 
      iterator.current()->text(1) != num )
  {
    iterator++;
  }
  
  // we found it
  if(iterator.current()->parent() == inactive_ss_item)
  {
    // move it to the active blocks section
    QListViewItem* ss_item = iterator.current();
    inactive_ss_item->takeItem(ss_item);
    active_ss_item->insertItem(ss_item);
  }   
}


void ModelTreeView::make_ss_inactive(int id)
{
        // make an iterator that starts at the first inactive block item
  QListViewItemIterator iterator(active_ss_item);
  iterator++;

  // the block name we are looking for
  QString num;
  num.setNum(id);
  
  // find the item that represents this block
  while(iterator.current()->parent() == active_ss_item && 
      iterator.current()->text(1) != num )
  {
    iterator++;
  }
  
  // we found it
  if(iterator.current()->parent() == active_ss_item)
  {
    // move it to the active blocks section
    QListViewItem* ss_item = iterator.current();
    active_ss_item->takeItem(ss_item);
    inactive_ss_item->insertItem(ss_item);
  }
}


void ModelTreeView::items_right_clicked(QListViewItem* item, const QPoint&, int )
{
  if(item == NULL)
    return;
  
  //when the right mouse button is pressed, bring up act_deact_menu
  if( item->parent()==active_blocks_item || 
      item->parent()==inactive_blocks_item ||
      item->parent()==active_ns_item ||               
      item->parent()==inactive_ns_item ||
      item->parent()==active_ss_item ||               
      item->parent()==inactive_ss_item )
  {
      act_deact_menu->exec(QCursor::pos());
  }
}


void ModelTreeView::activate_items()
{
  //go through all inactive items, getting only selected ones
  //std::string block_command("block ");
  QString block_command("block ");
  QListViewItemIterator iterator(inactive_blocks_item);
  for( ; iterator.current(); ++iterator)
  {
    if(iterator.current()->isSelected() && 
       iterator.current()->parent() == inactive_blocks_item)
    {
      QListViewItem * current_item;
      current_item = iterator.current();
      QString text = current_item->text(1);   
      block_command = block_command + text.latin1() + " ";
    }
  }

  if (block_command.length() > 6)
  {
    block_command += "on";
    GUICommandHandler::instance()->add_command_string( block_command.latin1(), 
                                                       VERDE_FALSE);
  }

  QListViewItemIterator iterator2(inactive_ns_item);
  for( ; iterator2.current(); ++iterator2)
  {
    if(iterator2.current()->isSelected() && 
       iterator2.current()->parent() == inactive_ns_item)
    {
      QListViewItem * current_item;
      current_item = iterator2.current();
      QString text = current_item->text(1);   
      
      //std::string command;
      QString command;
      command = "nodeset " + text + " on";
      GUICommandHandler::instance()->add_command_string( command.latin1(), VERDE_FALSE);
  
    }
  }

  QListViewItemIterator iterator3(inactive_ss_item);
  for( ; iterator3.current(); ++iterator3)
  {
    if(iterator3.current()->isSelected() && 
       iterator3.current()->parent() == inactive_ss_item)
    {
      QListViewItem * current_item;
      current_item = iterator3.current();
      QString text = current_item->text(1);   
      
      //std::string command;
      QString command;
      command = "sideset " + text + " on";
      GUICommandHandler::instance()->add_command_string( command.latin1(), VERDE_FALSE);

    }
  }
}
  
  
void ModelTreeView::deactivate_items()
{
  //go through all active items, getting only selected ones
  //std::string block_command("block ");
  QString block_command("block ");
  QListViewItemIterator iterator(active_blocks_item);
  for( ; iterator.current(); ++iterator)
  {
    if(iterator.current()->isSelected() && 
       iterator.current()->parent() == active_blocks_item)
    {
      QListViewItem * current_item;
      current_item = iterator.current();
      QString text = current_item->text(1);   

      block_command = block_command + text.latin1() + " ";
    }
  }

  if(block_command.length() > 6)
  {
      block_command += "off";
      GUICommandHandler::instance()->add_command_string( block_command.latin1(), 
                                                         VERDE_FALSE);
  }

  QListViewItemIterator iterator2(active_ns_item);
  for( ; iterator2.current(); ++iterator2)
  {
    if(iterator2.current()->isSelected() && 
       iterator2.current()->parent() == active_ns_item)
    {

      QListViewItem * current_item;
      current_item = iterator2.current();
      QString text = current_item->text(1);   
    
      //std::string command;
      QString command;
      command = "nodeset " + text + " off";
      GUICommandHandler::instance()->add_command_string( command.latin1(), VERDE_FALSE);
  
    }
  }
    
  QListViewItemIterator iterator3(active_ss_item);
  for( ; iterator3.current(); ++iterator3)
  {
    if(iterator3.current()->isSelected() && 
       iterator3.current()->parent() == active_ss_item)
    {

      QListViewItem * current_item;
      current_item = iterator3.current();
      QString text = current_item->text(1);   
    
      //std::string command;
      QString command;
      command = "sideset " + text + " off";
      GUICommandHandler::instance()->add_command_string( command.latin1(), VERDE_FALSE);

    }
  }
}  


