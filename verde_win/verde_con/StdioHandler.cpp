
// Verde_con is a helper application for verde.exe.
// It provides a console to verde.exe for access to the stdout and stderr

// Creator: Clinton Stimpson
// Date:    11/15/2001


//////////////////////////////////////////////////////////////////////
// StdioHandler.cpp: implementation of the StdioHandler class.
//
//////////////////////////////////////////////////////////////////////




#include <iostream>
#include "StdioHandler.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StdioHandler::StdioHandler(QObject* parent, QProcess* p)
:QObject(parent), _process(p)
{
	// check for NULL process
	if(!p)
	{
		std::cout << "Invalid Process" << std::endl;
		return;
	}

	// connect the stdout of the process to our read_stdout function
	QObject::connect( p, SIGNAL(readyReadStdout()), this, SLOT(read_stdout()) );
	QObject::connect( p, SIGNAL(readyReadStderr()), this, SLOT(read_stderr()) );
}

StdioHandler::~StdioHandler()
{
}


void StdioHandler::read_stdout()
{
	// double check NULL process
	if(!_process) return;

	// get the data and print it
	QString data;
	while( (data = _process->readLineStdout()) != QString::null)
	{
		std::cout << (const char*)data << std::endl;
	}
}

void StdioHandler::read_stderr()
{
	// double check NULL process
	if(!_process) return;

	// get the data and print it
	QString data;
	while( (data = _process->readLineStdout()) != QString::null)
	{
		const char* stdout_data = data;
		std::cerr << (const char*)data << std::endl;
	}
}