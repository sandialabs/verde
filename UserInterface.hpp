//- Class:       UserInterface
//- Description: UserInterface class declaration. 
//-              This class is implemented as a {singleton} pattern. Only
//-              one instance is created and it is accessed through the 
//-              {instance()} static member function. 
//- Owner:       Jim Hipp
//- Checked by:  Bill Bohnhoff. 6/27/94
//- Version: $Id: 

#ifndef USERINTERFACE_HPP
#define USERINTERFACE_HPP


#ifdef CANT_USE_STD_IO
#include <fstream.h>
#include <strstream.h>
#else
#include <fstream>
#include <strstream>
#endif
#include <deque>

#include "VerdeDefines.hpp"
#include "VerdeString.hpp"
#include "GetLongOpt.hpp"
#include "CpuTimer.hpp"
#include "DLFilePtrList.hpp"

class CommandHandler;

enum parseSuccess {SUCCESS, BAD_VALUE, BAD_TOKEN};

//! the interface between input commands and command execution 
class UserInterface
{

friend class GUICommandHandler;
  
public:
   int isJournalling;

   //! gets singleton instance of class
   static UserInterface* instance(); 

   //! controlled access and creation of the sole instance of this class.
   ~UserInterface();
  
   //! initialization of UserInterface. 
   //! processes options, prints banner screen, initializes idr
   int init_interface (int argc, char **argv);

   //! Main interface to UserInterface. 
   //! initializes {getline()} (non-GUI version)
   //! parses command line options
   //! calls yyparse() -- In GUI version, this is called to parse
   //! initialization file and any files on command line.
   //! calls XtAppMainLoop() in GUI version.
   void perform_interface();

   
   //! returns verde version number 
   static VerdeString version();

   //! returns revision date
   static VerdeString revision_date();

   //! returns time and date of execution
   const char *execution_time_and_date();
   
   //! outputs verde start-up/splash screen 
   void  start_up_screen();
  
   //! file verde has loaded/read in 
   VerdeInputFile *currentFile;

   //! Set elapsed and delta variables to the cpu time used since beginning of
   //! execution (elapsed) and since last call to cpu_time() function (delta).
   void cpu_time(double *elapsed, double *delta);

   //int get_next_file();
   friend class CommandHandler;
   
   void aprepro_is_journaled(VerdeBoolean flag);
   VerdeBoolean aprepro_is_journaled()    const;
   
   void command_is_journaled(int flag);
   int  command_is_journaled()   const;
   
   void command_is_echoed(int flag);
   int command_is_echoed() const;
   VerdeString journal_filename() const;
  
   // Functions that handle the generation of journal strings.  
   //! Sets the journal string to "", resets the ignore flags
   void clear_journal_string();

   //! Adds 's' to the end of the journal string
   void append_to_journal_string(const char* s)
   { *journalString << s; }

   //! Adds 'c' to the end of the journal string
   void append_to_journal_string(char c)
   { *journalString << c; }

   //! Adds 'num' to the end of the journal string
   void append_to_journal_string(int num)
   { *journalString << num; }

   //! Adds 'num' to the end of the journal string
   void append_to_journal_string(double num)
   { *journalString << num; }

   //! Returns the journal string in its current state.
   VerdeString journal_string();

   //! Heading: Miscellaneous file operations
   void add_input_file(const char *fileName, int type=FILE_NORMAL, int loop=1);

   //! Add the specified filename or file pointer to the filename stack.
   void add_input_file(FILE *file, int type=FILE_NORMAL, int loop=1);

   //! opens file pointer to journal file
   void open_journal_stream(const char *file_name = NULL, int max_num = 0);

   //! closes file pointer to journal file
   void close_journal_stream();

   //! opens stream to record verde commands for journaling
	void open_recording_stream(const char* filename);
 
   //! closes recording stream 
   void close_recording_stream();

   //! Put the last retrieved character back on the input stream.
   int unget_input(int c);

   //! returns TRUE/FALSE if command is recorded
   VerdeBoolean  command_is_recorded()    const;

   //! returns BatchMode flag 	
   int GetBatchMode(){ return BatchMode; }

   //! sets batchMode flag
   void SetBatchMode(int Flag = VERDE_TRUE) { BatchMode = Flag; };

   //! gets hasDefaultsFile flag
   int hasDefaultsFile() { return hasdefaultsfile; }
 
   void names_are_journaled(VerdeBoolean flag);
   VerdeBoolean  names_are_journaled()    const;

	void reading_fastq(int flag);
	int  reading_fastq()          const;

	std::ofstream&   journal_stream();
	std::ofstream&   recording_stream();

	int get_input();
	char *includePath;

protected:
   //! Class Constructor. (Not callable by user code. Class is constructed
   //! by the {instance()} member function.
   UserInterface();

private:

   
   //! static pointer to unique instance of this class
   static UserInterface* instance_;

   void define_command_options();

   //! Parse the command line options and set the appropriate variables
   int parse_command_options(int argc, char **argv);

   int process_defaults_file(const char * defaults_file = 0);
   
   int internal_get_input();
   
   int get_next_file();
   
   std::deque<VerdeInputFile *> inputFileList;

   //! Parse the .verde default file if it exists
  
   FILE *inputFile;

   VerdeString inputLine;
   
   int BatchMode;
   int hasdefaultsfile;

   int currentPosition;
   
   GetLongOpt options;
   int debugIndex;

   int commandIsEchoed;
   int commandIsJournaled;
   int commandIsRecorded;
   int readingFastq;


	cBit graphicsAreJournaled : 1;
	cBit namesAreJournaled    : 1;
	cBit apreproIsJournaled   : 1;
	cBit ignoreNextPrepend    : 1;
	cBit ignoreNextAppend     : 1;

	VerdeString replacementText;

   //! Heading: Data and methods for the default verde journal file
   VerdeString journalFilename;
  
   std::ofstream    journalStream;
   std::ofstream    recordingStream;  
   std::ostrstream  *journalString;

   //! Heading: Data and methods for a user-defined journal file
   VerdeString recordingFilename;
   
   char executionTimeAndDate[24];

   CpuTimer globalTimer;

   //! parses a string containing integers and returns the list
   VerdeStatus parse_int_list(char *string, std::deque<int> &int_list);
};


inline VerdeBoolean
UserInterface::aprepro_is_journaled() const
{ return (VerdeBoolean)apreproIsJournaled; }

inline void
UserInterface::aprepro_is_journaled(VerdeBoolean flag)
{ apreproIsJournaled = flag; }

inline VerdeBoolean
UserInterface::command_is_recorded() const
{ return (VerdeBoolean)commandIsRecorded; }

inline std::ofstream&
UserInterface::recording_stream()
{ return recordingStream; }

inline void 
UserInterface::command_is_journaled(int flag)
{
  if(flag != commandIsJournaled)
  {
    commandIsJournaled = flag;
    if(commandIsJournaled)
      open_journal_stream();
    else
      close_journal_stream();
  }
}




inline void
UserInterface::command_is_echoed(int flag)
{ commandIsEchoed = flag; }



inline int
UserInterface::command_is_echoed() const
{ return commandIsEchoed; }


inline void
UserInterface::reading_fastq(int flag)
{ readingFastq = flag; }


inline int
UserInterface::reading_fastq() const
{ return readingFastq; }


inline std::ofstream&
UserInterface::journal_stream()
{ return journalStream; }

inline int
UserInterface::command_is_journaled() const
{ return commandIsJournaled; }


/*
inline ofstream&
UserInterface::recording_stream()
{ return recordingStream; }


inline int
UserInterface::command_is_recorded() const
{ return commandIsRecorded; }

inline void 
UserInterface::command_is_journaled(int flag)
{
  if(flag != commandIsJournaled) {
    commandIsJournaled = flag;
    if(commandIsJournaled) {
      open_journal_stream();
    }
    else {
      close_journal_stream();
    }
  }
}
*/

/*
inline VerdeBoolean
UserInterface::names_are_journaled() const
{ return (VerdeBoolean)namesAreJournaled; }
*/

#endif // USERINTERFACE_HPP

