//-------------------------------------------------------------------------
// Filename      : VerdeApp.cc 
//
// Purpose       : This file represents the Verde application itself.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef NT
  #include <io.h>
#else
  #include <unistd.h>
#endif

#ifdef CANT_USE_STD_IO
#include <strstream.h>
#else
#include <strstream>
#endif

#include <ctype.h>
#include <time.h>
#include <new.h>

#include "VerdeApp.hpp"
#include "VerdeMessage.hpp"
#include "Mesh.hpp"
#include "ExodusTool.hpp"
#include "ErrorTool.hpp"
#include "CpuTimer.hpp"
//#include "MDBVerde.hpp"
#include "MDBInterface.hpp"
#include "MDBClient.hpp"
#include "UserInterface.hpp"
#include "EdgeTool.hpp"
#include "idr.hpp"


// Different platforms follow different conventions
#ifndef NT
#include <sys/resource.h>
#endif
#ifdef SOLARIS
extern "C" int getrusage(int, struct rusage *);
#ifndef RUSAGE_SELF
#include </usr/ucbinclude/sys/rusage.h>
#endif
#endif

#ifdef HP
#include <sys/syscall.h>
//#define getrusage(a, b)  syscall(SYS_GETRUSAGE, a, b)
#endif

extern "C" void signal_handler(int sig);
static void out_of_memory();
extern "C" void gl_cleanup();


// the global VerdeApp
VerdeApp* verde_app = 0;

VerdeApp::VerdeApp()
{
  verde_app = this;

  // get registry information so verde_help will work on windows
#ifdef WIN32
  
  // first see if the environment is set or not
  if(!getenv("VERDE_DIR"))
  {
    DWORD reg_size = 200;
    TCHAR reg_value[201];
    char my_reg_value[201];
    HKEY hKey;
    
    // Open the Verde key
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Verde"),
          0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
    // Get the Value of VERDE_DIR key
    if(RegQueryValueEx(hKey, TEXT("VERDE_DIR"),
          NULL, NULL, (LPBYTE)reg_value, &reg_size)
        == ERROR_SUCCESS)
    {
      // copy the value
      for(int i=0; i<reg_size; i++)
        my_reg_value[i] = (char)reg_value[i];
      my_reg_value[reg_size] = '\0';
      
      // put value of key into environment
      char env[300];
      sprintf(env, "VERDE_DIR=%s", my_reg_value);
      _putenv(env);
    }
    
    // close the Verde key
    RegCloseKey(hKey);
    
    }
  }

#endif
	
  currentMesh = NULL;
}



// Initializes the application
VerdeStatus VerdeApp::initialize()
{
     // Initialize the signal handling
   this->initialize_signal_handling();

   // initialize variables
   inputFile = ""; 
   outputFile ="";

   doMetrics = VERDE_TRUE;
   doTopology = VERDE_TRUE;
   doInterface = VERDE_TRUE;
   doOutput = VERDE_FALSE;
   printFailedElements = VERDE_FALSE;
   metricsSuccess = VERDE_FALSE;
   topologySuccess = VERDE_FALSE;
   interfaceSuccess = VERDE_FALSE;
   doIndividual = VERDE_FALSE;
   add_mesh = VERDE_FALSE;

   // initialize MDB database
   mdb_client = new MDBClient("MDB");
   MDBuuid uuid = IDD_MDBVerde;
   mdb_client->GetInterface( uuid, (MDBUnknownInterface**)&MDBObject );

//   MDBObject = new MDBVerde();
   if(!MDBObject)
      return VERDE_FAILURE;
   
   //MDBObject = new MDBVerde();

   return VERDE_SUCCESS;
}

// Initializes signal handling for the session
VerdeStatus VerdeApp::initialize_signal_handling() const
{
#ifndef NT
     //
     // Send signal to handler routine.  Only do so 
     // if the signal is not already being ignored.
     //
   if (signal(SIGHUP,  SIG_IGN) != SIG_IGN)
      signal(SIGHUP,  signal_handler);
   if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
      signal(SIGQUIT, signal_handler);
   if (signal(SIGBUS,  SIG_IGN) != SIG_IGN)
      signal(SIGBUS,  signal_handler);
   if (signal(SIGSEGV, SIG_IGN) != SIG_IGN)
      signal(SIGSEGV, signal_handler);

     // Set up the new handler, too.
   set_new_handler(out_of_memory);
#endif
   return VERDE_SUCCESS;
}

// This function is called whenever a signal is passed to Verde
extern "C" void signal_handler(int)// sig)
{
   PRINT_INFO("...Interrupt Detected. VERDE Exiting...\n");
   //gl_cleanup();
   exit(1);
//    signal(sig, SIG_IGN);
//      /*
//       * Reset the signal to come here again. (Code not reached, left in
//       * for future modifications...
//       */
//    signal(sig, signal_handler);
}

// This function is called whenever Verde runs out of memory
void out_of_memory()
{
   PRINT_ERROR("Too much dynamic memory requested.\n"  );
   //gl_cleanup();
   exit(1);
}


// Prints out information about the session, like
// execution time and memory problems.
void VerdeApp::report_resource_usage() const
{
#ifndef NT
   struct rusage r_usage;
   float utime, stime;
   getrusage(RUSAGE_SELF, &r_usage);
   utime = (float)r_usage.ru_utime.tv_sec +
      ((float)r_usage.ru_utime.tv_usec/1.e6);
   stime = (float)r_usage.ru_stime.tv_sec +
      ((float)r_usage.ru_stime.tv_usec/1.e6);
   PRINT_INFO("\nEnd of Execution\n");
   PRINT_INFO("User time             = %f\n", utime);
   PRINT_INFO("System time           = %f\n", stime);
   PRINT_INFO("Total time            = %f\n", utime+stime);
   PRINT_INFO("Max resident set size = %d bytes\n", r_usage.ru_maxrss*4096);
   PRINT_INFO("Int resident set size = %d\n", r_usage.ru_idrss);
   PRINT_INFO("Minor Page Faults     = %d\n", r_usage.ru_minflt);
   PRINT_INFO("Major Page Faults     = %d\n", r_usage.ru_majflt);
   PRINT_INFO("Swaps                 = %d\n", r_usage.ru_nswap);
#endif
}

VerdeApp::~VerdeApp()
{
  // We may want to do this someplace else, but this works.
  EdgeTool::delete_instance();

  if(currentMesh)
  {
    delete currentMesh;
  }
  
  delete mdb_client;
  IDR::delete_instance();
  delete UserInterface::instance();

}

void VerdeApp::print_summary() const
{
  /*
#ifndef NT
   struct rusage r_usage;
   float utime;
   getrusage(RUSAGE_SELF, &r_usage);
   utime = (float)r_usage.ru_utime.tv_sec +
      ((float)r_usage.ru_utime.tv_usec/1.e6);
#else
   float utime = 1.0;
#endif
*/
   
   int number_elements = currentMesh->number_elements();
   PRINT_INFO("\n-----------------\n"
              "Execution Summary\n"
              "-----------------\n");
   PRINT_INFO("   User time             = %3.2f\n", analyze_time);
   int rate = (int)((double)number_elements/analyze_time);
   PRINT_INFO("   Elements Processed    = %d\n",number_elements);
   PRINT_INFO("   Elements Per Second   = %d\n", rate);
   if (currentMesh->volume() > 0.0)
   {
      PRINT_INFO("   Total Volume          = %3.2f\n",
                 currentMesh->volume());
   }
   if (currentMesh->area() > 0.0)
   {
      PRINT_INFO("   Total Area            = %3.2f\n",
                 currentMesh->area());
   }

   PRINT_INFO("\n");
   
   if (doMetrics)
   {
      if (metricsSuccess)
         PRINT_INFO("   Metrics Tests         = PASS\n");
      else
         PRINT_INFO("   Metrics Tests         = FAIL  "
                    "(see Metrics Summary above)\n");
   }

   if (doTopology)
   {
      if (topologySuccess)
         PRINT_INFO("   Mesh Topology Tests   = PASS\n");
      else
         PRINT_INFO("   Mesh Topology Tests   = FAIL "
                    "(see Mesh Topology Information above)\n");
   }

   if (doInterface)
   {
      if (interfaceSuccess)
         PRINT_INFO("   Mesh Interface Tests  = PASS\n");
      else
         PRINT_INFO("   Mesh Interface Tests  = WARN "
                    "(see Mesh Interface Information above)\n");
   }

   VerdeString temp_string = "verde";
   temp_string.to_upper_case();
   if (topologySuccess && metricsSuccess && interfaceSuccess)
      PRINT_INFO("   %s                 = PASS\n",temp_string.c_str());
   else
      PRINT_INFO("   %s                 = FAIL\n",temp_string.c_str());
   
}

VerdeString VerdeApp::input_file()
{
   return inputFile;
}

void VerdeApp::input_file(const char *s)
{
   // set input_file name
   
   inputFile = s;

   // check if outputfile name has been set. If not, derive one
   // from inputFile

   if (outputFile == "")
   {
      size_t pos = inputFile.find(".");
      outputFile = inputFile.substr(0,pos) + "_out.gen";
   }
   
}

void VerdeApp::set_add_mesh(int flag)
{
   if(flag==1)
     add_mesh = VERDE_TRUE;
   else
     add_mesh = VERDE_FALSE;
}


VerdeString VerdeApp::output_file()
{
   return outputFile;
}

void VerdeApp::output_file(const char *s)
{
   // If there is a value passed in set it else use the value 
   // defined in VerdeApp::input_file() above.
   if (s)
      outputFile = s;
}

void VerdeApp::set_warning_level(int value)
{
  VerdeMessage::instance()->warning_level_flag(value);
}

int VerdeApp::get_warning_level()
{
  return VerdeMessage::instance()->warning_level_flag();
}

VerdeStatus VerdeApp::verify_mesh()
{

  // if there is no mesh object, return

  if (!currentMesh)
  {
    PRINT_ERROR("No mesh loaded...\n");
    return VERDE_FAILURE;
  }

   // create a Mesh Object and open the exodus file

   VerdeStatus success;
   //         Mesh::instance()->open_exodus_file(inputFile.c_str());

   //if (!success)
   //   return success;
   
   // Process the mesh

   CpuTimer time_to_analyze;

   success = currentMesh->verify_mesh(doMetrics,
                                           doTopology,
                                           doInterface,
                                           printFailedElements,
                                           doIndividual,
                                           metricsSuccess,
                                           topologySuccess,
                                           interfaceSuccess);
   if( success == VERDE_FAILURE )
     return success;

   if (doOutput && ( doMetrics || doTopology || doInterface) )
   {
      ExodusTool tool;
      VerdeBoolean do_skin = VERDE_TRUE;
      VerdeBoolean do_edges = VERDE_TRUE;      
      tool.write_results_output_file(outputFile,do_skin, do_edges);
   }

   // for now, call error stuff here

   ErrorTool::instance()->calculate();
   

   analyze_time = time_to_analyze.cpu_secs();
   if(!(analyze_time > 0.))
      analyze_time = 0.0001f;


   print_summary();
   
   return success;
}


VerdeStatus VerdeApp::load_mesh(const char* filename, VerdeBoolean readfile)
{
	// create a mesh object and open the exodus file
    inputFile = filename;

    if(!add_mesh)
    {
      if (currentMesh)
      {
        delete currentMesh;
        currentMesh = NULL;
      }
    }

    if (!currentMesh) { currentMesh = new Mesh(); MDB()->reset(); }
    

    if(readfile == VERDE_TRUE)
    {
        PRINT_INFO("\nProcessing File: %s...\n",inputFile.c_str());
        return currentMesh->open_exodus_file(inputFile.c_str(),NULL);
    }
    else 
        return VERDE_SUCCESS;
}

VerdeStatus VerdeApp::load_initial_mesh()
{
  //VerdeStatus ret_val;

  // first, see if the user has input a filename on the command line
  if (inputFile.length() == 0)
  {
    return VERDE_SUCCESS;
  }

  // create a new mesh object

  currentMesh = new Mesh();
  PRINT_INFO("\nProcessing File: %s...\n",inputFile.c_str());

  // If the user has specified blocks on the command line, input those also

  if (commandLineBlockList.size())
  {
    return currentMesh->open_exodus_file(inputFile.c_str(), &commandLineBlockList);
  }
  else
  {
    return currentMesh->open_exodus_file(inputFile.c_str(),NULL);
  }
}

MDBInterface *VerdeApp::MDB()
{
  return MDBObject;
}


