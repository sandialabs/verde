#ifndef GUITEXTBOX
#define GUITEXTBOX 

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


#include <qtextedit.h>

//! Command window 
class GUITextBox : public QTextEdit
{
public:
	GUITextBox(QWidget *parent = 0, const char *name = 0);
	~GUITextBox();

	//! append data to the text box
	void append(const char *);
	void insert_prompt();

protected:
	//! process key events
	virtual void keyPressEvent( QKeyEvent *e);

private:
	//! put cursor in correct place
	int move_cursor( QKeyEvent *e );
	
	//! special events such as history
	int do_special_keyevents( QKeyEvent *e);

	//! terminating characters ( "enter", "!", etc.. )
	int terminating_character(int ch);
	
	//! location of the cursor
	int Cursor_Paragraph;
	int Cursor_Position;

	//! temporary holding for commands
	QString tmp_command;

	//! string to store command that will be executed
	QString command_to_execute;
};

#endif

