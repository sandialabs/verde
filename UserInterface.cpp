//! Class:       UserInterface
//! Description: Implementation code for the UserInterface class
//- Version: $Id: 

#include <time.h>
#include <stdio.h>

#ifdef CANT_USE_STD_IO
#include <strstream.h>
#else
#include <strstream>
#endif


#ifdef NT
  #include <io.h>
#else
  #include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "VerdeString.hpp"

const int STRING_LENGTH = 132;
const int MAX_NUMBER_JOURNALS = 99;

#include "VerdeVersion.h"
#include "UserInterface.hpp"
#include "idr.hpp"
#include "CommandHandler.hpp"
#include "VerdeMessage.hpp"
#include "VerdeDefines.hpp"
#include "VerdeApp.hpp"
#include "HexMetric.hpp"
#include "TetMetric.hpp"
#include "PyramidMetric.hpp"
#include "WedgeMetric.hpp"
#include "KnifeMetric.hpp"
#include "QuadMetric.hpp"
#include "TriMetric.hpp"
#include "EdgeMetric.hpp"
#include "Mesh.hpp"
#include "ErrorTool.hpp"
#include "VerdeUtil.hpp"
#include "SkinTool.hpp"


#include "GUI/GUICommandHandler.hpp"


extern "C" {
extern int yyparse(); 
}


extern void define_idr_variable(char *s);
void start_up_screen();

#define streq(a,b)  (0 == strcmp(a,b))

#ifndef NT
  extern "C" {
      void  hist_adds(const char *);

      void  gl_init(int);
      void  gl_cleanup();
  }
#endif

#define DEBUG_PRINT(var,level) PRINT_DEBUG(VERDE_DEBUG_##level, \
                          "DEBUG %d: %s = %s\n", level, #var, var)

int exit_yyparse = VERDE_FALSE;

      
UserInterface* UserInterface::instance_ = 0;

UserInterface* UserInterface::instance()
{
  if (instance_ == 0) {
    instance_ = new UserInterface;
  }
  return instance_;
}

UserInterface::UserInterface ()
{
   inputFile              = NULL;
   includePath            = NULL;
  inputLine              = "";
  currentPosition        = 0;
  journalString          = NULL;
  currentFile            = NULL;

  BatchMode              = VERDE_FALSE;
  isJournalling	         = VERDE_FALSE;
  commandIsEchoed        = VERDE_TRUE;
  commandIsJournaled     = VERDE_FALSE;
  commandIsRecorded      = VERDE_FALSE;
  readingFastq           = VERDE_FALSE;
  hasdefaultsfile        = VERDE_FALSE;
  journalFilename        = "";
  recordingFilename      = "";

  CommandHandler::initialize_echo_stream();
#if defined(__STDC__) || defined(__cplusplus)

#ifdef NT
  static char buf[10024];
  if (setvbuf(stdout, buf, _IOLBF, sizeof(buf)))  {
#else
  if (setvbuf(stdout, NULL, _IOLBF, 0))  {
#endif

	  PRINT_ERROR(" line buffering initialization failed\n" );
  }
#else
  setlinebuf(stdout);
#endif

  // Initialize time and date string
  int stringLength = 24;
  //  executionTimeAndDate = new char [ stringLength ];
  struct tm *localTime;
  time_t calendarTime;
  calendarTime  = time( NULL );
  localTime = localtime( &calendarTime );
  #ifndef NT
    const char *format = "%m/%d/%y at %r";
  #else
    const char *format = "%m/%d/%y at %H:%M:%S";
	if(localTime->tm_isdst == 0)  // account for daylight savings time
	{localTime->tm_hour += 1;}
  #endif
  strftime ( executionTimeAndDate, stringLength, format, localTime );
  
  define_command_options();
}

UserInterface::~UserInterface ()
{
  instance_ = NULL;

  for(unsigned int i=0; i<inputFileList.size(); i++)
    delete inputFileList[i];
  inputFileList.clear();
  
  delete GUICommandHandler::instance();

}

VerdeString UserInterface::version()
{
  VerdeString version = VERDE_VERSION;
  return version;
}

VerdeString UserInterface::revision_date()
{
  VerdeString revision = VERDE_DATE;
  return revision;
}

const char *UserInterface::execution_time_and_date()
{
  return &executionTimeAndDate[0];
}


int UserInterface::unget_input(int c)
{
  if (--currentPosition < 0) {
    // We are pushing back more than we had originally,
    // probably the result of a variable substitution in aprepro
    // Create a new inputLine with blanks at the beginning to hold
    // the extra data
    VerdeString blanks_32 = "                                ";
    int len_32 = blanks_32.length();
    blanks_32 += inputLine;
    inputLine = blanks_32;
    currentPosition = len_32-1;
  }
  inputLine.put_at(currentPosition, c);
  return c;
}

int UserInterface::init_interface (int argc, char **argv)
{
      
   int rc = parse_command_options(argc, argv);
   
   if(!rc) //for bad command flags
   {
     //exit on bad command flag
     std::cout<<"Bad Command Flag Option....VERDE Exiting"<<std::endl;
     exit(1);
   }
  
   // read error commands file

   if (ErrorTool::instance()->do_error())
		rc = ErrorTool::instance()->read_control_file();

   return VERDE_SUCCESS;   
}

void UserInterface::perform_interface()
{
  GUICommandHandler::instance()->call_yyparse();
  if (IDR::instance()->parse_error_occurred())
    PRINT_ERROR("Errors parsing input file.\n" );
  
  if(commandIsJournaled)
    PRINT_INFO("Closing journal file '%s'\n", journalFilename.c_str());
  
  journalStream.close();
}


void UserInterface::define_command_options()
{
   options.usage("[options] [input_file.gen | input_journalfile.jou]");
   options.enroll("help",  GetLongOpt::Valueless,
                  "Print this summary", 0);
   options.enroll("batch", GetLongOpt::Valueless,
					"Runs verde in batch mode", 0);
   options.enroll("blocks", GetLongOpt::MandatoryValue,
                  "Specifies block(s) to be processed (e.g. 1,5-7,9)",
                  NULL);
   //options.enroll("error", GetLongOpt::MandatoryValue,
   //               "Enable error estimation using commands in file $val:",
   //                0);
   //options.enroll("exodus_file", GetLongOpt::MandatoryValue,
   //					"exodus file to process", 0);
   options.enroll("individual",  GetLongOpt::Valueless,
                  "prints metrics for each block individually", 0);
   //options.enroll("journal_file", GetLongOpt::MandatoryValue,
   //					"journal file to write to", 0); 
/*   
#ifdef NAME
   options.enroll("list_defaults",  GetLongOpt::Valueless,
                  "Lists defaults settable in ."NAME" defaults file", 0);
#endif
*/ 
   options.enroll("defaults", GetLongOpt::MandatoryValue, 
		   "Specifies path and file of defaults file (e.g ../defaults1)", 0);
   //options.enroll("list_error_commands",  GetLongOpt::Valueless,
   //               "Lists commands available in error command file", 0);
   options.enroll("nodefaults", GetLongOpt::Valueless,
					"do not process .verde default file", 0);
   options.enroll("nointerface",  GetLongOpt::Valueless,
                  "Suppresses mesh interface verification calculations", 0);
   options.enroll("nometrics",  GetLongOpt::Valueless,
                  "Suppresses element metric calculations", 0);
   options.enroll("output",  GetLongOpt::MandatoryValue,
                  "Generates an Exodus II output file specified by $val", 0);
   options.enroll("notopology",  GetLongOpt::Valueless,
                  "Suppresses mesh topology verification calculations", 0);
   //options.enroll("file_name", GetLongOpt::OptionalValue,
   //               "Output to file $val: default = input_file_out.gen",
   //               NULL);
   options.enroll("print_failed_elements",  GetLongOpt::Valueless,
                  "Prints individual failed elements and values", 0);
   options.enroll("version",  GetLongOpt::Valueless,
                  "Prints verde version number", 0);
   options.enroll("quads_share_3_nodes", GetLongOpt::MandatoryValue,
		  "Check entire mesh, exterior quads, or nothing for quads sharing 3 nodes (all, exterior, none)", 0);
//   options.enroll("warning_level", GetLongOpt::MandatoryValue,
//       "Values less than 2 will suppress the warning for quadrilateral elements that share three nodes",
//                  NULL);
   
}

int UserInterface::parse_command_options(int argc, char **argv)
{

  // GetLongOpt options
  char temp_char[10] = "VERDE_OPT";
 
  if (getenv("VERDE_OPT") != NULL)
    options.parse(getenv("VERDE_OPT"), temp_char);
  int option_index = options.parse(argc, argv);
  if ( option_index < 0 )
    return VERDE_FAILURE;

   
  // The following commands invoke batch mode automatically, provide the requested
  // information, and terminate
   
  if (options.retrieve("help"))
  {
    UserInterface::instance()->SetBatchMode(VERDE_TRUE);
    start_up_screen();
    options.usage();
    exit(0);
    return VERDE_FAILURE;
  }

/*  if (options.retrieve("list_defaults"))
  {
    UserInterface::instance()->SetBatchMode(VERDE_TRUE);
    start_up_screen();
    //list_default_file_commands();
    exit(0);
    return VERDE_FAILURE;
  }*/

  if (options.retrieve("version"))
  {
    UserInterface::instance()->SetBatchMode(VERDE_TRUE);
    start_up_screen();
    exit(0);
    return VERDE_FAILURE;
  }
 
  /* Disabled command
  if (options.retrieve("list_error_commands"))
  {
    UserInterface::instance()->SetBatchMode(VERDE_TRUE);
    ErrorTool::instance()->list_error_commands();
    exit(0);
    return VERDE_FAILURE;
  }
  */
 
  // add the proper input file to the stack 

  if(options.retrieve("batch"))
  {
    UserInterface::instance()->SetBatchMode();
  }
  else
  {
    // put a fake empty file on the inputFileList
    VerdeInputFile *new_file = new VerdeInputFile((FILE*)NULL);
    new_file->filename = "VERDE_GUI";
    inputFileList.push_back(new_file);
    inputFile = new_file->filePointer;
    currentFile = new_file;
  }

   
 
  // add any journal files or an exodus file
  for (int i = argc-1; i >= option_index; i--)
  {
    DEBUG_PRINT(argv[i],52);
    if (strchr(argv[i], '='))
      ;
    //define_idr_variable(argv[i]);
    else
    {
      int charptr = 0;
      char *extension = NULL;
      // find the file extension
      for(charptr = 0; argv[i][charptr] != '\0'; charptr++);
      for( ; argv[i][charptr] != '.' && charptr != 0; --charptr);
      extension = &argv[i][charptr+1];
         
      // if extension starts with "j", it is a journal file
      // otherwise, it is an exodus file
      if(extension[0] == 'j')
      {
        add_input_file(argv[i]);
        UserInterface::instance()->isJournalling = VERDE_TRUE;
        PRINT_INFO("Input File entered: %s\n", argv[i]);
      }
      else
      {
        verde_app->input_file(argv[i]);
      }
    }
  }

  if(options.retrieve("batch"))
  {
    UserInterface::instance()->SetBatchMode();
  }
 

  // checking this options must be done after all 
  // other options that add files to the Input File Stack
  // so the defaults file is processed first

  // specifying defaults takes precedence over specifying
  // nodefaults

  // if they didn't say nodefaults, process the defaults 
  if(!options.retrieve("nodefaults"))
  {
    const char * defaults_file;
    // if they did specify a defaults file, do it anyways
    defaults_file = options.retrieve("defaults");
    process_defaults_file(defaults_file);
  }
  // if they said nodefaults, process the defaults file only
  // if it was specified
  else
  {
    const char* defaults_file = options.retrieve("defaults");
    if(defaults_file)
      process_defaults_file(defaults_file);
  }

/* Deprecated flags
  const char *journalfile = options.retrieve("journal_file");
  if(journalfile)
  {
    open_journal_stream(journalfile);
    UserInterface::instance()->isJournalling = VERDE_TRUE;
  }

  const char *exodusfile = options.retrieve("exodus_file");
  if(exodusfile)
  {
    verde_app->input_file(exodusfile);
  }
*/
   
  // set verbose flag if necessary
   
  if (options.retrieve("print_failed_elements"))
  {
    verde_app->print_failed_elements(VERDE_TRUE);
  }

  // set options as requested by user
   
  //! output must specify file name for output
  const char *out_file = options.retrieve("output");
  if (out_file)
  {
    verde_app->output_file(out_file);
    verde_app->do_output(VERDE_TRUE);
  }

//   if (options.retrieve("noskin"))
//   {
//      verde_app->do_skin(VERDE_FALSE);
//   }
   
  if (options.retrieve("nometrics"))
  {
    verde_app->do_metrics(VERDE_FALSE);
  }
   
  if (options.retrieve("nointerface"))
  {
    verde_app->do_interface(VERDE_FALSE);
  }

  if (options.retrieve("notopology"))
  {
    verde_app->do_topology(VERDE_FALSE);
  }

//   if (options.retrieve("noedges"))
//   {
//      verde_app->do_edges(VERDE_FALSE);
//   }


  if (options.retrieve("individual"))
  {
    verde_app->do_individual(VERDE_TRUE);
  }
   
/* Deprecated flags
  const char *out_file = options.retrieve("file_name");
  if (out_file)
  {
    verde_app->do_output(VERDE_TRUE);
    verde_app->output_file(out_file);
  }
      
  const char *error_file = options.retrieve("error");
  if (error_file)
  {
    ErrorTool::instance()->set_control_file_name(error_file);
  }
*/
      
  // check for specified blocks
   
  const char *block_list = options.retrieve("blocks");
  if (block_list)
  {
    std::deque<int> int_list;
    VerdeStatus success = parse_int_list((char*)block_list,int_list);
    if (success)
    {
      for(unsigned int i = 0; i < int_list.size(); i++)
      {
        verde_app->add_block_to_process(int_list[i]);
      }
    }
    else
    {
      PRINT_INFO("Error: bad blocks specification..\n");
      exit(1);
    }
  }

  /*
  const char *warning_list = options.retrieve("warning_level");
  if (warning_list)
  {
    std::deque<int> int_list;
    VerdeStatus success = parse_int_list((char*)warning_list,int_list);
    if (success)
    {
      VerdeMessage::instance()->warning_level_flag(int_list[0]);
    }
    else
    {
      PRINT_INFO("Error: bad warning_level specification..\n");
      exit(1);
    }
  }
  */


  const char* quad3node = options.retrieve("quads_share_3_nodes");
  if(quad3node)
  {
    if(VerdeUtil::compare(quad3node, "none"))
      SkinTool::set_quads_sharing_three_nodes_completeness(0);
    
    else if(VerdeUtil::compare(quad3node, "exterior"))
      SkinTool::set_quads_sharing_three_nodes_completeness(1);
    
    else if(VerdeUtil::compare(quad3node, "all"))
      SkinTool::set_quads_sharing_three_nodes_completeness(2);
    
    else
      PRINT_INFO("Error: bad quads_share_3_nodes specification.\n");
      
  }

  return VERDE_SUCCESS;
 
}


void UserInterface::start_up_screen()
{
     VerdeString temp_string;
     temp_string = "verde";
     temp_string.to_upper_case();
  
    PRINT_INFO("\n\n"	       
	       "\n\t\t         *** %s Version %s ***\n"
	       "\n\t\t        Revised %s\n"
               "\n\t\tVERDE: VERIFICATION OF DISCRETE ELEMENTS\n" 
	       "\n\t\tA MESH VERIFICATION TOOL FOR EXODUS MESHES\n"
                 "\n\t\t      Executing on %s\n",
         temp_string.c_str(),      
	       version().c_str(),
	       revision_date().c_str(),
	       execution_time_and_date() );

}

void UserInterface::cpu_time(double *elapsed, double *delta)
{
  *elapsed = globalTimer.elapsed();
  *delta   = globalTimer.cpu_secs();
}


VerdeStatus UserInterface::parse_int_list(char *string,
                                          std::deque<int> &int_list)
{
   // first, check to make sure the string has only numbers and proper
   // delimiters

   int len_str = strlen(string);
   int i;
   for ( i = 0; i < len_str;i++)
   {
      if (!isdigit(string[i]) && string[i] != ' ' &&
          string[i] != '-' && string[i] != ',')
      {
         return VERDE_FAILURE;
      }
   }
   
   // parse the string looking for single integers and integer ranges
   // with format such as 1,2,4-7,9-12,13
   
   const char *ct = ", ";
   const char *numbers = "0123456789";
   char num_string[80];
   
   memset(num_string,0,80);

   // use strtok to get tokens, then parse

   char *token = strtok(string,ct);


   while(token)
   {
      
      // get first (of a possible two) integers in the token

      int len_num = strspn(token,numbers);
      if (len_num > 0)
      {
         // we have a number in this token
         strncpy(num_string,token,len_num);
         int num_1 = atoi(num_string);
         int num_2 = num_1;

         // now, search for the second number if present

         char *ptr = strchr(token,'-');
         if (ptr)
         {
            // we have a range
            ptr++;
            len_num = strspn(ptr,numbers);
            if(len_num)
            {
               strncpy(num_string,ptr,len_num);
               num_2 = atoi(num_string);
            }
         }

         // transfer number or range to intlist

         for (i = num_1; i <= num_2; i++)
         {
            int_list.push_back(i);
         }
         
      }
      token = strtok(NULL,ct);
   }
   return VERDE_SUCCESS;
}

int UserInterface::process_defaults_file(const char *defaults_file)
{
   
   // if a file is specified
   if(defaults_file)
   {
     // try to open the file for input
     std::ifstream def_file(defaults_file);

     // if the file succeeded in being opened
     if(def_file)
     {
       def_file.close();
       // add the file to the Input File Stack
       add_input_file(defaults_file);
       hasdefaultsfile = VERDE_TRUE;
       return VERDE_SUCCESS;
     }

   }
   // if there is no file specified
   // try to find a file
   else
   {
     const char* homedir = getenv("HOME");
     const char* verdedir = getenv("VERDE_DIR");

     // see if we have a file in our current directory
     std::ifstream currdir_file(".verde");
     if(currdir_file)
     {
       currdir_file.close();
       add_input_file(".verde");
       hasdefaultsfile = VERDE_TRUE;
       return VERDE_SUCCESS;
     }

     // see if we have one in the home dir
     if( homedir && (strlen(homedir)>0) )
     {
       char* filename = new char[strlen(homedir)+20];
       strcpy(filename, homedir);
#ifdef WIN32
       strcat(filename, "\\.verde");
#else
       strcat(filename, "/.verde");
#endif
       std::ifstream homedir_file(filename);
       if(homedir_file)
       {
         homedir_file.close();
         add_input_file(filename);
         hasdefaultsfile = VERDE_TRUE;
         delete [] filename;
         return VERDE_SUCCESS;
       }

       delete [] filename;
       
     }

     // see if we have one in the VERDE_DIR
     if( verdedir && (strlen(verdedir)>0) )
     {
       char* filename = new char[strlen(verdedir)+20];
       strcpy(filename, verdedir);
#ifdef WIN32
       strcat(filename, "\\.verde");
#else
       strcat(filename, "/.verde");
#endif
       std::ifstream verdedir_file(filename);
       if(verdedir_file)
       {
         verdedir_file.close();
         add_input_file(filename);
         hasdefaultsfile = VERDE_TRUE;
         delete [] filename;
         return VERDE_SUCCESS;
       }

       delete [] filename;
     }

   }

   return VERDE_FAILURE;   
   
}

// Returns the journal string in its current state
VerdeString UserInterface::journal_string()
{
    // Add a NULL to the end of the string.
	*journalString << std::ends;
  
  char *str = journalString->str();
  VerdeString return_val(str);
  delete [] str;

  delete journalString;
  journalString = NULL;
  return return_val;
}

int UserInterface::get_input()
{
    // File being read is UserInterface::inputFile
  if (inputLine.length() == 0 || (int)inputLine.length() <= currentPosition)
  {
    while (internal_get_input() == VERDE_FALSE &&
           get_next_file()); // empty while body.

    if (inputLine.length() <= 0)
    {
      currentFile = new VerdeInputFile((FILE*)NULL);
      inputLine = "Exit\n";
    }
  }
  return (int)inputLine.get_at(currentPosition++);
}

// Sets the journal string to "", resets the ignore flags
void UserInterface::clear_journal_string()
{
  if (journalString)
  {
    char *temp_s = journalString->str();
    delete journalString;
    delete [] temp_s;
  }
  journalString = new std::ostrstream();
}


int UserInterface::internal_get_input()
{

  currentPosition = 0;

  if( (inputFile == NULL)  && (strcmp(currentFile->filename.c_str(), "VERDE_GUI")== 0) )
  {

   // set inputLine = to the string in the GUI 
   inputLine = GUICommandHandler::instance()->get_command_string(); 

   exit_yyparse = VERDE_TRUE;

   if(inputLine.length() == 0)
	   return VERDE_FALSE;

    return VERDE_TRUE;
  }

  // read input from a file
  else if(inputFile!=NULL )
  {
    exit_yyparse = VERDE_FALSE;
    char command[STRING_LENGTH];

    // Set ret_val to NULL if we are at the end of the file,
      // or get the next line of input.
    char *ret_val = 
      0 != feof( inputFile ) ? NULL :  // end of file ? NULL 
      fgets(command, STRING_LENGTH, inputFile); // : get

    assert( ret_val == NULL || ret_val == command );      
    if (ret_val == NULL)
    {
      if (feof( inputFile )){ // not end of file
        clearerr( inputFile );
      }
      inputLine = "";
      //delete command;
      return VERDE_FALSE;
    }
    else
    {
      inputLine = command;
        // Check for long line (> STRING_LENGTH). To detect that,
        // Assume the following:
        // 1. strlen(command) == STRING_LENGTH-1
        // 2. command[STRING_LENGTH-1] != \n.
        // If both conditions hold, do another fgets and concatenate
        // onto end of inputLine.
      while (strlen(command) == STRING_LENGTH-1 &&
             command[STRING_LENGTH-1] != '\n')
      {
        char *result = 
          0 != feof( inputFile ) ? NULL :  // end of file ? NULL 
          fgets(command, STRING_LENGTH, inputFile); // : get	  
        assert( result == NULL || result == command );      
        if (result != NULL)
          inputLine += command;
        else
          command[0] = '\0';
          // force end of while loop: if eof reached, 
          // "command" is undefined and may be system dependent.
      }
      
        // Ensure that all input lines end with carriage return.
        // It is possible to create a file with no final newline
      if (inputLine.get_at(inputLine.length()-1) != '\n')
        inputLine += "\n";
      
      DEBUG_PRINT(inputLine.c_str(), 52);
        // Add non-comment and non-aprepro lines to the getline history buffer
      
      if (feof( inputFile ))
        clearerr( inputFile );
      //delete command;
      return VERDE_TRUE;
    }
  }
  return VERDE_TRUE;

}


int UserInterface::get_next_file()
{
  if (currentFile &&
      currentFile->fileType == FILE_TEMPORARY &&
      currentFile->loopCount-- > 0) {
    rewind(inputFile);
    currentPosition = 0;
    inputLine = "";
    return VERDE_TRUE;
  }

  if (inputFileList.size() <= 0)
    return VERDE_FALSE;
  else
  {
    if (inputFile != NULL)
    {
      if(inputFile != stdin)
        fclose(inputFile);
      
      currentFile = inputFileList.back();
      inputFileList.pop_back();
      inputFile = currentFile->filePointer;
      delete currentFile;
      if (inputFileList.size() <= 0)
        return VERDE_FALSE;
    }
    currentFile = inputFileList.back();
    inputFile = inputFileList.back()->filePointer;
    currentPosition = 0;
    inputLine = "";
    return VERDE_TRUE;
  }
}


void UserInterface::add_input_file(const char *fileName, int type, int loop)
{
  VerdeInputFile *new_file = new VerdeInputFile(fileName, type, loop,
						includePath);
  if (new_file->filePointer) {
    inputFileList.push_back(new_file);
    inputFile = new_file->filePointer;
    currentFile = new_file;
  }
  else
    delete new_file;
}


void UserInterface::add_input_file(FILE *file, int type, int loop)
{
  if (file)
  {
    VerdeInputFile *new_file = new VerdeInputFile(file, type, loop);
    inputFileList.push_back(new_file);
    inputFile = new_file->filePointer;
    currentFile = new_file;
  }
}


void UserInterface::open_journal_stream(const char *file_name, int max_num)
{

#ifdef WIN32
	const int buf_len = _MAX_PATH +1;
#else
  const int buf_len = PATH_MAX + 1;
#endif
  char buffer[buf_len];
  char file_fmt[buf_len] = "verde%n.jou";

  


  // Open a journal file using the specified file_name.
  // If file_name is not specified, use the file_name verde#.jou
  // where # is a number from 1 to MAX_NUMBER_JOURNALS.
  // If those files already exist, Then overwrite the last file.
 
  if (max_num <= 0)
    max_num = MAX_NUMBER_JOURNALS;
 
  if (max_num >= 100) {
    PRINT_WARNING("A maximum of 99 journal files is supported\n");
    max_num = 99;
  }
 
  if (file_name || (file_name = getenv("VERDE_JOURNAL")) ) {
    strncpy(file_fmt, file_name, buf_len - 1);
  }
 
 
  // Scan for format specifiers
  VerdeBoolean have_number = VERDE_FALSE;
  VerdeBoolean other_format = VERDE_FALSE;
  char* ptr;
  for( ptr = file_fmt; *ptr; ptr++ )
  {
    if( *ptr == '%' )
    {
      if( *(ptr+1)=='n' )
      {
        // If %n was specified, note it (have_number) and
        // change it to two bells so strftime will ignore it.
        have_number = VERDE_TRUE;
        *ptr = '\007';
        ptr++;
        *ptr = '\007';
      }
      else  
      {
        // If we found any other '%'s in the string, note it
        // so we know to call strftime().
        other_format = VERDE_TRUE;
      }
    }
  }
 
  // If we need to, call strftime() to expand date specifiers.
  // Otherwise just copy the string.
  if( other_format )
  {
    time_t current_time = time(0);
    strftime(buffer, buf_len-1, file_fmt, localtime(&current_time) );
  }
  else
  {
    strncpy(buffer, file_fmt, buf_len-1 );
  }
 
  // If %n was encountered in the format string, we need to
  // expand it to the next available number.
  char* ten_ptr = 0;  //Pointer to the tens-digit in the string
  char* one_ptr = 0;  //Pointer to the ones-digit in the string
  if( have_number )
  {
    // Find the tens- and ones-digit in the string.
    // We marked them with bell characters before, so
    // that strftime() would ignore them.
    for( ptr = buffer; *ptr; ptr++ )
    {
      if( (*ptr == '\007') && (*(ptr+1) == '\007') )
      {
        ten_ptr = ptr;
        one_ptr = ptr + 1;
        *ten_ptr = '0';
        *one_ptr = '0';
        break;
      }
    }
    assert(ten_ptr != NULL);
 
    //Search for next available number.
    int counter = 0; 
    for(int i=0; i<10 && counter<=max_num; ++i)
    {
      *ten_ptr = (char)(i + '0');
      for(int j=0; j<10 && counter<=max_num; ++j, counter++)
      {
              if (i==0 && j==0)
                continue; // Don't want verde00.jou
 
        *one_ptr = (char)(j + '0');
 
		std::ifstream test_stream(buffer);
 
              if(!test_stream) {    // File does not exist, use it
                counter = max_num+1; // Break out of both loops.
              }
              test_stream.close();  // File exists, close it and try next.
      }
    }
  }
 
  // At this point, we either have a new file, or we have reached
  // max_num and the last filename will be reused.
  journalStream.open(buffer);
  journalFilename = buffer;
 
  if (journalStream)
  {
    PRINT_INFO("\nCommands will be journalled to '%s'\n",
               journalFilename.c_str());
    commandIsJournaled = VERDE_TRUE;
  } else {
    PRINT_ERROR("Could not open journal file '%s'\n",
                journalFilename.c_str());
    commandIsJournaled = VERDE_FALSE;
  }      

}


void UserInterface::open_recording_stream(const char* filename)
{
	  if(commandIsRecorded) 
	  {
		  PRINT_WARNING("Can't open new recording file\n"
		  "Commands are being recorded to \"%s\"\n", recordingFilename.c_str());
		  return; 
	  }		  
		  // allow only a single recording file at a time
	          // if already TRUE, the stream is in use
	  

	  recordingFilename = filename;
	  recordingStream.open(filename);
	  commandIsRecorded = VERDE_TRUE;
	       
	  if(!recordingStream) {
		  PRINT_WARNING("Could not open recording file\n" );
		  commandIsRecorded = VERDE_FALSE;
		  recordingFilename = "";
	  }
}
 
void UserInterface::close_recording_stream()
{
	  recordingStream.close();
	  commandIsRecorded = VERDE_FALSE;
	  recordingFilename = "";
}      
