/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void BlockImport::init()
{
  std::deque<ElementBlock*> block_list;
    //Place all block ids and corresponding element types in the list
    verde_app->mesh()->element_blocks( block_list );

    //feed block_list into columns of  blockList
    unsigned int i;
    for(i=0; i<block_list.size(); i++)
    {
	//set the block type
	QString block_type( block_list[i]->element_type().c_str() );
	
	//set the block number
	QString block_num;
	block_num.setNum(block_list[i]->block_id());
	
	//set the number of elements
	QString num_elements;
	num_elements.setNum(block_list[i]->number_elements());
	
	//create the listviewitem (with the 2 columns of number and type)
	new QListViewItem( blockList, block_num, block_type, num_elements);
    }
}



void BlockImport::select_all()
{
    blockList->selectAll(TRUE);
}


void BlockImport::clear_all()
{
   blockList->selectAll(FALSE);
}

void BlockImport::get_blocks(std::list<int>& blocks)
{
    QListViewItemIterator iterator(blockList);
    for( ; iterator.current(); ++iterator)
    {
	if(iterator.current()->isSelected())
	{
	    bool ok=true;
	    int id = iterator.current()->text(0).toInt(&ok);
	    if(ok)
	    {
		blocks.push_front(id);
	    }
	}
    }
}
