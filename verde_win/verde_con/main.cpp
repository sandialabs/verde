// Verde_con is a helper application for verde.exe.
// It provides a console to verde.exe for access to the stdout and stderr

// Creator: Clinton Stimpson
// Date:    11/15/2001


#include <iostream>

#include <qprocess.h>
#include <qapplication.h>
#include "StdioHandler.h"

int main(int argc, char** argv)
{
	// create the app
	QApplication verde_con_app(argc, argv);

	// create a process
	QProcess verde_app;
	
	
	// be a little smart on invoking verde
	// verde_con.exe must be in the same directory as verde.exe
	// or verde.exe must be accessable through the PATH

	QString this_apps_name = argv[0];

	// try finding verde_con.exe from the end of the string
	int found_at = this_apps_name.findRev("verde_con");
	if(found_at == -1)
	{
		// let's try this, if something weird happened with argv[0]
		this_apps_name = "verde.exe";
	}
	else
	{
		// replace verde_con with verde including relavite/absolute path
		this_apps_name.replace(found_at, 9, "verde");
	}

	const char* verde_app_name = (const char*)this_apps_name;
	
	// set the executable
	verde_app.addArgument(verde_app_name);
	// add the other arguements
	for(int i=1; i<argc; i++)
		verde_app.addArgument(argv[i]);

	// create a StdioHandler that redirects the stdout of the process
	// into the stdout of this app
	StdioHandler iohandler(&verde_con_app, &verde_app);

	// when our process quits, then this app quits too
	QObject::connect(&verde_app, SIGNAL(processExited()), &verde_con_app, SLOT(quit()));

	// start the process
	bool p_started = verde_app.start();

	// start this app's main loop or exit with errors
	if(p_started)
		return verde_con_app.exec();
	else
	{
		std::cout << "Couldn't start verde.exe" << std::endl;
		std::cout << "Be sure verde_con.exe is in the same directory as verde.exe" << std::endl;
		return 1;
	}

}