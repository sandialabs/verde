// Verde_con is a helper application for verde.exe.
// It provides a console to verde.exe for access to the stdout and stderr

// Creator: Clinton Stimpson
// Date:    11/15/2001



//////////////////////////////////////////////////////////////////////
// StdioHandler.h: interface for the StdioHandler class.
//
//////////////////////////////////////////////////////////////////////

#ifndef STDIO_HANDLER
#define STDIO_HANDLER

#include <qobject.h>
#include <qprocess.h>

class StdioHandler : QObject
{
	Q_OBJECT

public:
	StdioHandler(QObject* parent, QProcess* p);
	virtual ~StdioHandler();

public slots:
	// slots to read the stdout and stderr from the process
	void read_stdout();
	void read_stderr();

private:
	// pointer to the process we are redirecting from
	QProcess* _process;

};

#endif // STDIO_HANDLER
