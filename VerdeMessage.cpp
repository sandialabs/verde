

#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif

#ifdef CANT_USE_STD_IO
#include <fstream.h>
#else
#include <fstream>
#endif

#ifdef CANT_USE_STD_IO
#include <iomanip.h>
#else
#include <iomanip>
#endif


  #include "GUI/GUITextBox.hpp"
  #include "UserInterface.hpp"


#include "VerdeMessage.hpp"
#include "VerdeDefines.hpp"
#include "VerdeString.hpp"
#include "VerdeUtil.hpp"
#include "GUI/VerdeGUIApp.hpp"

#include <assert.h>

//
// Message type usage:
// PRINT_ERROR:    Message to tell user why the task did not complete.
// PRINT_WARNING_HIGH:  same as PRINT_WARNING
// PRINT_WARNING:  Message to tell user why completed task may not be what
//                 was requested.
// PRINT_WARNING_LOW:  Low-priority message to tell user why completed task may not be what
//                 was requested.
// PRINT_INFO:     Message to tell user about status and progress.
// PRINT_DEBUG:    Message to developer tied to a global debug flag.
// DIAGNOSTIC:     Message to developer.
//
// VERDE_DEBUG_FLAG "Ownership":
// initialize static class members
MessageFlag VerdeMessage::debugFlag[] = {
  MessageFlag( 0, "UNUSED"),
  MessageFlag( 1, "Debug Graphics toggle for some debug options."),
  MessageFlag( 2, "Whisker weaving information"),
  MessageFlag( 3, "Timing information for 3D Meshing routines."),
  MessageFlag( 4, "Testing of video generation - if on then\n"
		    "\t\tvideo specific drawing is enabled and related debug\n"
		    "\t\tstatements will be printed."),
  MessageFlag( 5, "FastQ debugging"),
  MessageFlag( 6, "Submaping graphics debuging"),
  MessageFlag( 7, "Knife progress whisker weaving information"),
  MessageFlag( 8, "Mapping Face debug / Linear Programing debug "),
  MessageFlag( 9, "Paver Debugging"),
  MessageFlag(10, "WW: removed hex seam flag"),
  MessageFlag(11, "Nodeset Associativity debugging"),
  MessageFlag(12, "Fastq activity"),
  MessageFlag(13, "Mesh entities"),
  MessageFlag(14, "Model activity"),
  MessageFlag(15, "2.5D Debugging (Project, Translate, Rotate)"),
  MessageFlag(16, "RefFace activity"),
  MessageFlag(17, "Use Count debugging"),
  MessageFlag(18, "Webcut debugging"),
  MessageFlag(19, "Feature Merge / Unmerge debugging"),
  MessageFlag(20, "Parallel meshing activity"),
  MessageFlag(21, "Boundary Layer Tool Debugging"),
  MessageFlag(22, "ExodusMesh sizing function debugging"),
  MessageFlag(23, "Draw after joining chords in WW"),
  MessageFlag(24, "SelfCrossingLoop (and derivatives) debug info"),
  MessageFlag(25, "Extra invalidity checking in WW"),
  MessageFlag(26, "Surface Smoothing"),
  MessageFlag(27, "Primal Construction debugging, see also flag 70"),
  MessageFlag(28, "Plastering debugging"),
  MessageFlag(29, "Volume SubMapping"),
  MessageFlag(30, "Volume Mapping"),
  MessageFlag(31, "CleanUp debugging"),
  MessageFlag(32, "Pyramid debugging"),
  MessageFlag(33, "Whisker Weaving inside chord list face drawing"),
  MessageFlag(34, "If on Whisker Weaving doesn't merge sheets"),
  MessageFlag(35, "If on WW query displays sheets before joining chords"),
  MessageFlag(36, "Enable/Disable idr_keyword_debugger function"),
  MessageFlag(37, "Superdrive debugging"),
  MessageFlag(38, "WW hex formation messages"),
  MessageFlag(39, "Doublet Pillower graphics output"),
  MessageFlag(40, "Element Quality debugging output"),
  MessageFlag(41, "Check_join adjacency check"),
  MessageFlag(42, "Debuging for auto vertex type setting"),
  MessageFlag(43, "Programmer Errors for SubMapping"),
  MessageFlag(44, "Submapping Graphics Debuging"),
  MessageFlag(45, "Pillow Sheet debugging"),
  MessageFlag(46, "Paver breakout detection (expensive)"),
  MessageFlag(47, "Extra LP debugging (see flag 8 also)"),
  MessageFlag(48, "Geometry sizing function."),
  MessageFlag(49, "Draws Face by Face Creation in Paving"),
  MessageFlag(50, "Debugging for AutoSchemeSelect"),
  MessageFlag(51, "Modified Paver (row by row, more intersection checking)"),
  MessageFlag(52, "User Interface: If flag is enabled, filenames being\n"
		    "\t\t\tused for input will be echoed and each input line\n"
		    "\t\t\twill be echoed prior to being parsed."),
  MessageFlag(53, "Surface Morpher debugging"),
  MessageFlag(54, "Parser debugging"),
  MessageFlag(55, "Stairtool general debugging"),
  MessageFlag(56, "Stairtool face intersection debugging"),
  MessageFlag(57, "Relative Interval/Length setting"),
  MessageFlag(58, "StcVertex debugging of Whisker Weaving" ),
  MessageFlag(59, "VGI developer error"),
  MessageFlag(60, "StcVertex debugging of Looping" ),
  MessageFlag(61, "List number of points used in curve faceting" ),
  MessageFlag(62, "Print verbose information on group operations"),
  MessageFlag(63, "Label Whisker Weaving diagrams tersely"),
  MessageFlag(64, "No label on Whisker Weaving diagrams"),
  MessageFlag(65, "Volume Morpher debugging"),
  MessageFlag(66, "Print debug information on importing Pro/E geometry"),
  MessageFlag(67, "List number of triangles used in surface faceting" ),
  MessageFlag(68, "Debug information on tetrizing volumes" ),
  MessageFlag(69, "Debug information on tet mesher geometry access" ),
  MessageFlag(70, "STC Pillowing, see also flag 27" ),
  MessageFlag(71, "Hex-Tet Debug Flag"),
  MessageFlag(72, "DoubletPillower text messages"),
  MessageFlag(73, "Auto Surface debugging (use new auto surf select)"),
  MessageFlag(74, "Feature-based decomposition info"),
  MessageFlag(75, "Many-to-many sweep imprint debugging"),
  MessageFlag(76, "Virtual point and partition curve"),
  MessageFlag(77, "Volume interval matching"),
  MessageFlag(78, "Tipton Smoother jacobian modification enabler"),
  MessageFlag(79, "VoidCleanupTool debugging"),
  MessageFlag(80, "Hex-Tet informational messages"),
  MessageFlag(81, "Curve Morpher Debugging"),
  MessageFlag(82, "Diamond debugging"),
  MessageFlag(83, "AutoSizer debugging"),
  MessageFlag(84, "Surface auto decomposition"),
  MessageFlag(85, "U-SubMapping debugging"),
  MessageFlag(86, "Virtual curve and partition surface"),
  MessageFlag(87, "Composite curve and composite surface"),
  MessageFlag(88, "General virtual geometry stuff"),
  MessageFlag(89, "Tet meshing warning and debug messages")
};

VerdeMessage* VerdeMessage::instance_ = NULL;

VerdeMessage* VerdeMessage::instance()
{
  if (!instance_) {
    instance_ = new VerdeMessage;
    if (!instance_) {
	  std::cerr << " *** Unable to instantiate message object ***" << std::endl;
      exit(1);
    }
  }
  return instance_;
}

VerdeMessage::VerdeMessage()
{
  errorCount    = 0;
  infoFlag      = VERDE_TRUE;
  warningFlag   = VERDE_TRUE;
  diagnosticFlag= VERDE_FALSE;
  loggingStream = NULL;
  loggingFile   = NULL;
  warningLevelFlag = 2;
    
  // Check initialization of debugFlag array.
  for (int i=number_of_debug_flags(); i > 0; i--) {
    debugFlag[i].setting = VERDE_FALSE;
    assert(i == debugFlag[i].flagNumber);
    assert(debugFlag[i].description != NULL);
  }
  debugFlag[ 62 ].setting = VERDE_TRUE;
}

VerdeMessage::~VerdeMessage()
{
  // Close all streams associated with debug flags.
  // If the same stream is being used for debug and logging, it
  // will get closed below.
  for (int i=number_of_debug_flags(); i > 0; i--)
    remove_debug_stream(i);

  // At this time, the only open stream possible is loggingStream.
  if (loggingStream != NULL) {
    loggingStream->close();
    delete loggingStream;
    delete loggingFile;
  }
  
  // Set static instance_ to zero to indicated that we are dead.
  instance_ = 0;
}

int VerdeMessage::number_of_debug_flags()
{
  return sizeof(debugFlag)/sizeof(debugFlag[0])-1;
}

void VerdeMessage::internal_error ( const int message_type,
				    std::ofstream *output_stream,
				    const char *format, va_list &args )
{
//#ifdef _DEBUG
//	int print_it = VERDE_TRUE;
//#else
	int print_it = VERDE_FALSE;
//#endif

  VerdeString prefix = "";
  
  switch (message_type) {
  case VERDE_ERROR:
    print_it = VERDE_TRUE;
    prefix = "ERROR: ";
    break;
  case VERDE_WARNING:
    if (warningFlag) {
      print_it = VERDE_TRUE;
      prefix = "WARNING: ";
    }
    break;
  case VERDE_INFO:
    if (infoFlag) print_it = VERDE_TRUE;
    break;
  case VERDE_DIAGNOSTIC:
    if (diagnosticFlag) {
      print_it = VERDE_TRUE;
      prefix = "DIAGNOSTIC: ";
    }
    break;
  default:
    if (message_type >= VERDE_DEBUG_1 &&
	message_type <= number_of_debug_flags()+10)
      {
	if (debugFlag[message_type-10].setting) print_it = VERDE_TRUE;
	break;
      }
  }

  if (print_it) {
    char msgbuf[BUFSIZ]; /* we're not getting huge strings */

    vsprintf(msgbuf, format, args);
    
    // loggingStream is used to journal error, warning, and info messages.
    // debug messages can also be journalled there by setting the
    // output stream for the debug flag to the same file.
    if (loggingStream != NULL && (message_type == VERDE_ERROR ||
				  message_type == VERDE_WARNING ||
				  message_type == VERDE_INFO))
      
      *loggingStream << prefix.c_str() << msgbuf << std::flush;
    
    if(UserInterface::instance()->GetBatchMode() == VERDE_TRUE || verde_gui_app == 0 || verde_gui_app->textBox() == 0)
    {
      if (output_stream == NULL)
        std::cout << prefix.c_str() << msgbuf << std::flush;
      else
        *output_stream << prefix.c_str() << msgbuf << std::flush;
    }
    else
    {
        verde_gui_app->textBox()->append((const char*) msgbuf);
    }

  }
}

int VerdeMessage::print_error ( const char *format, ... )
{
  va_list args;
  va_start(args, format);

#ifdef XTERM
  char esc = 0x1B;
  // Turn on reverse video on VT102 (xterm also)
  // (0=normal, 1-bold, 4-underscore, 5-blink, 7-inverse)
  std::cout << esc << '[' << '7' << 'm';
#endif

  internal_error(VERDE_ERROR, NULL, format, args);

#ifdef XTERM
  std::cout << esc << '[' << '0' << 'm' << std::flush;
#endif

  va_end(args);
  add_to_error_count();
  return VERDE_FAILURE;
}

int VerdeMessage::print_warning ( const char *format, ... )
{
  if(warningLevelFlag > 0)
  {
    va_list args;
    va_start(args, format);
    
    internal_error(VERDE_WARNING, NULL, format, args);
    
    va_end(args);
  }
  return VERDE_FAILURE;
}

int VerdeMessage::print_warning_low ( const char *format, ... )
{
  if(warningLevelFlag > 1)
  {
    va_list args;
    va_start(args, format);
    
    internal_error(VERDE_WARNING, NULL, format, args);
    
    va_end(args);
  }
  return VERDE_FAILURE;
}

int VerdeMessage::print_info ( const char *format, ... )
{
  va_list args;
  va_start(args, format);

  internal_error(VERDE_INFO, NULL, format, args);

  va_end(args);
  return VERDE_FAILURE;
}

int VerdeMessage::print_debug ( const int flag, const char *format, ... )
{
  va_list args;
  va_start(args, format);

  internal_error(flag, debugFlag[flag-10].outputStream, format, args);

  va_end(args);
  return VERDE_FAILURE;
}
 
void VerdeMessage::print_diagnostic ( const char *format, ... )
{
  va_list args;
  va_start(args, format);
 
  internal_error(VERDE_DIAGNOSTIC, NULL, format, args);
 
  va_end(args);
}

int VerdeMessage::reset_error_count(int value)
{
  int current_value = errorCount;
  if (errorCount != value) {
    errorCount = value;
    PRINT_WARNING("Error count manually changed from %d to %d\n\n",
		  current_value, value);
  }
  return current_value;
}

int VerdeMessage::error_count()
{
  return errorCount;
}

void VerdeMessage::add_to_error_count()
{
  errorCount++;
}

void VerdeMessage::output_debug_information(int from, int to, int step) 
{
  if (to == -1)
    to = number_of_debug_flags();
  
  PRINT_INFO("Debug Flag Settings "
	     "(flag number, setting, output to, description):\n");
   for (int i=from; i <= to; i+=step) {
      debugFlag[i].output();
   }
  PRINT_INFO("\n");
}
   
void VerdeMessage::output_debug_information(VerdeString &match) 
{
  int count = 0;
  for (int i=1; i <= number_of_debug_flags(); i++) {
    char *tmp = strdup(debugFlag[i].description);
    if (tmp) {
      VerdeUtil::convert_string_to_lowercase(tmp);
      VerdeString debug_description = tmp;
      free(tmp);
      if (debug_description.find(match, 0) < debug_description.length()) {
	if (count == 0) {
	  PRINT_INFO("Debug Flag Settings "
		     "(flag number, setting, output to, description):\n");
	}
	debugFlag[i].output();
	count++;
      }
    }
  }
  if (count == 0) {
    PRINT_WARNING("No debug descriptions contain the "
		  "substring '%s'\n", match.c_str());
  }
  PRINT_INFO("\n");
}
   
void VerdeMessage::output_logging_information()
{
  if (loggingStream != NULL)
    PRINT_INFO("logging           = On, log file = '%s'\n", loggingFile->c_str());
  else
    PRINT_INFO("logging           = Off\n");
}
   
void MessageFlag::output()
{
  VerdeMessage::instance()->
    print_info("%2d  %3s  %-16s   %s\n",
               flagNumber, (setting == 1 ? "ON " : "OFF"),
               (filename == NULL ? "terminal" : filename->c_str()),
               description);
}
    
int VerdeMessage::find_file_use(const VerdeString &filename)
{
  if (filename == "terminal") {
    // remove_debug_stream has set the outputStream and filename to NULL.
    return -1;
  }

  // See if any of the other debug flags have this file open
  for (int i=number_of_debug_flags(); i > 0; i--) {
    if (debugFlag[i].filename && *(debugFlag[i].filename) == filename) {
      return i;
    }
  }
  if (loggingFile && *(loggingFile) == filename)
    return -2;
  
  return 0;
}
  
int VerdeMessage::count_stream_users(const std::ofstream *stream)
{
  int match = 0;
  if (stream != NULL) {
    for (int i=number_of_debug_flags(); i > 0; i--) {
      if (debugFlag[i].outputStream == stream) {
	match++;
      }
    }
    
    if (loggingStream == stream)
      match++;
  }
  return match;
}

void VerdeMessage::set_logging_file(const VerdeString &filename)
{
  // If logging is currently outputting to a file, close it if
  // it is the only thing using that file. (and the filenames don't match)
  if (loggingFile && *loggingFile == filename)
    return;

  int users = count_stream_users(loggingStream);
  if (users == 1) { // Just us...
    loggingStream->close();
    delete loggingStream;
    loggingStream = NULL;
    delete loggingFile;
    loggingFile = NULL;
  }
    
  int match = find_file_use(filename);
  
  if (match == -1) // Filename is 'terminal'
    return;
  else if (match != 0) {
    loggingFile   = debugFlag[match].filename;
    loggingStream = debugFlag[match].outputStream;
  }
  else {
    loggingFile   = new VerdeString(filename);
	loggingStream = new std::ofstream(filename.c_str());
  }
}

void VerdeMessage::set_debug_file(const int index, const VerdeString &filename)
{
  // If this flag is currently outputting to a file, close it if
  // this is the only flag using that file.
  remove_debug_stream(index);
  
  int match = find_file_use(filename);
  
  if (match == -1) // Filename is 'terminal'
    return;
  if (match == -2) {// Filename is same as loggingFile;
    debugFlag[index].filename = loggingFile;
    debugFlag[index].outputStream = loggingStream;
  }
  else if (match == index)
    return;
  else if (match != 0) {
    debugFlag[index].filename = debugFlag[match].filename;
    debugFlag[index].outputStream = debugFlag[match].outputStream;
  }
  else {
    debugFlag[index].filename = new VerdeString(filename);
	debugFlag[index].outputStream = new std::ofstream(filename.c_str());
  }
}

void VerdeMessage::remove_debug_stream(const int index)
{
  // NOTE: DO NOT USE PRINT_* CALLS, THIS IS CALLED FROM DESTRUCTOR.

  // Multiple debug flags may be using the same output stream,
  // Go through the list and count who is using this stream,
  // If only one use, close and delete the stream.
  if (debugFlag[index].outputStream == NULL)
    return;

  int match = count_stream_users(debugFlag[index].outputStream);

  if (match == 1) {
    debugFlag[index].outputStream->close();
    delete debugFlag[index].outputStream;
    delete debugFlag[index].filename;
  }
  debugFlag[index].filename = NULL;
  debugFlag[index].outputStream = NULL;
}
	 
MessageFlag::MessageFlag(int flag_number, const char *desc)
{
  flagNumber   = flag_number;
  setting      = VERDE_FALSE;
  description  = desc;
  filename     = NULL;
  outputStream = NULL;
}

MessageFlag::MessageFlag()
{
  flagNumber   = 0;
  setting      = VERDE_FALSE;
  description  = NULL;
  filename     = NULL;
  outputStream = NULL;
}

MessageFlag::~MessageFlag()
{
  // It is not safe to delete either the stream or the filename here
  // since multiple instances (debug flags) may refer to the same memory
}

#ifdef STANDALONE
void main() {
VerdeMessage::instance()->output_debug_information(1, 10, 2);
VerdeMessage::instance()->output_debug_information(12);
VerdeMessage::instance()->set_debug_file(5, "Debug_Test.file");
DEBUG_FLAG(5, VERDE_TRUE);
PRINT_DEBUG(VERDE_DEBUG_5, "This is a test\n");
VerdeMessage::instance()->output_debug_information(5,5);
}
#endif
