//-------------------------------------------------------------------------
//- Class:         GUITextBox    ( derived from QTextEdit )
//
//- Description:   GUITextBox class implementation.
//                 This is the implimentation of the textbox in the GUI
//
//- Owner:         Clinton Stimpson
//
//- Date:          6/21/2001
//------------------------------------------------------------------------- 






#include "GUITextBox.hpp"
#include "GUICommandHandler.hpp"

GUITextBox::GUITextBox (QWidget * parent, const char *name)
	:QTextEdit(parent, name)
{
	getCursorPosition(&Cursor_Paragraph, &Cursor_Position);
	setWordWrap(NoWrap);
}

GUITextBox::~GUITextBox()
{
}


void GUITextBox::append(const char *text)
{
    setFamily("Courier");
    //QTextEdit::append(text);
    moveCursor(MoveEnd, false);
    QTextEdit::insert(text, false, true, false);
    moveCursor(MoveEnd, false);
    scrollToBottom();
}

void GUITextBox::insert_prompt()
{
	append("VERDE> ");
}


void GUITextBox::keyPressEvent(QKeyEvent *e)
{


	int result = do_special_keyevents(e);
	if (result == 1)   // 1 means that it was a unique case and was processed
		return;

	// put the cursor in the correct place if necessary
	result = move_cursor(e);
	if(result == 1)
		return;

	// enter was pressed
	if ( terminating_character(e->ascii()))
	//if ( (e->ascii()) == 13)
	{
		QTextEdit::keyPressEvent(e);
		// get the command string
		command_to_execute = text ( Cursor_Paragraph );
		// remove the "verde> "
		command_to_execute.remove (0, 7);
		// remove the " " at the end of the string
		command_to_execute.truncate( command_to_execute.length() - 1);

		if (command_to_execute.length() > 1)
			GUICommandHandler::instance()->add_command_string(command_to_execute);
		
		append ("VERDE> ");
	}
	else if(e->ascii() == 8)
	{
		if(Cursor_Position > 7)
		    QTextEdit::keyPressEvent(e);
	}
	else
	{
		QTextEdit::keyPressEvent(e);
	}
	getCursorPosition(&Cursor_Paragraph, &Cursor_Position);

}


int GUITextBox::move_cursor(QKeyEvent *e)
{
	// find out if the cursor is currently in the "VERDE>" prompt
	
	int curr_paragraph;
	int curr_position;
	Cursor_Paragraph = paragraphs();

	// filter for keys that will cause cursor readjustment
	
	int ascii_keypressed = e->ascii();	
		
	// move the cursor to the end of the command line if return was entered
	if (ascii_keypressed == 13)
		moveCursor (MoveEnd, false);
	
	if( ( ascii_keypressed <  97 ) && ( ascii_keypressed >  122 ) )   // not a-z
	if( ( ascii_keypressed <  65 ) && ( ascii_keypressed >   90 ) )    // not A-Z
	if( ( ascii_keypressed <  48 ) && ( ascii_keypressed >   57 ) )  // not 0-9
	if( ( ascii_keypressed != 13 ) || ( ascii_keypressed !=   8 ) ) // not enter or backspace
	if( ( ascii_keypressed !=  9 ) || ( ascii_keypressed !=  32 ) )  // not tab or space
		return 0;

	// if cursor is not in correct place and key was not filtered out, 
	// move to the end of the command line.

	getCursorPosition(&curr_paragraph, &curr_position);

	// don't let the cursor go into the "VERDE> " area
	if( (e->key() == Key_Left) && (curr_position <= 7) )
		return 1;
	

	// cursor is not on the line
	if ( (curr_paragraph+1) != paragraphs() )
	{
		moveCursor( MoveEnd, false );
	}

	else if (curr_position <=6 )
	{
		// cursor is inside the "VERDE> " part -- move it to the end of the line
		moveCursor (MoveEnd, false); 
	}
	
	getCursorPosition(&Cursor_Paragraph, &Cursor_Position);

	return 0;
}


int GUITextBox::do_special_keyevents( QKeyEvent *e )
{

	if( e->key() == Key_Up )
	{
		// put the previous in the command_history in the text box
		
		// select the text to replace
		setSelection ( paragraphs()-1, 7 , paragraphs()-1, text(paragraphs()-1).length()-1 );
		// replace the selection with the new command
		insert( GUICommandHandler::instance()->history_prev() );

		return 1;
	}

	if( e->key() == Key_Down )
        {
		// put the next in the command_history in the text box
		
		// select the text to replace
		setSelection ( paragraphs()-1, 7 , paragraphs()-1, text(paragraphs()-1).length()-1 );
		// replace the selection with the new command
		insert( GUICommandHandler::instance()->history_next() );

		return 1;
	}

	if( e->ascii() == (int)'!' )
	{


		return 1;
	}	
	return 0;
}

int GUITextBox::terminating_character(int ch)
{
	if( ch == 13 )   // "return"
		return true;
	else
		return false;

}


