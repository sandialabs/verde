//-------------------------------------------------------------------------
// Filename      : VerdeApp.hpp 
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

#ifndef VERDE_APP_HPP
#define VERDE_APP_HPP

#include <deque>
#include <algorithm>

#include "VerdeDefines.hpp"
#include "VerdeString.hpp"

class Mesh;
class MDBInterface;
class MDBClient;


//! This file represents the Verde application itself
class VerdeApp  
{
public: 

   //! constructor
   VerdeApp();

   //! destructor
   ~VerdeApp();

   //! initialized the application and returns success
   VerdeStatus initialize();

   //! Prints out information about the session
   void report_resource_usage() const;

   //! Prints out summary information for the session
   void print_summary() const;

   //! Returns number of days until the app expires
   int days_to_expire(int year, int month, int day) const;

   //! return the File name which holds the mesh to be processed
   VerdeString input_file();

   //! set the File name which holds the mesh to be processed
   void input_file(const char *s) ;

   //! return the File name which holds the mesh to be processed
   VerdeString output_file();

   //! set the File name which holds the mesh to be processed
   void output_file( const char *s) ;

   //! Controls all mesh processing; main call to do the work
   VerdeStatus verify_mesh();

   //! sets output file flag
   void do_output(VerdeBoolean value);
   
   //! sets topology checking flag
   void do_topology(VerdeBoolean value);

   //! sets interface checking flag
   void do_interface(VerdeBoolean value);
   
   //! sets verboseprint_failed_elements output flag
   void print_failed_elements(VerdeBoolean value);

   //! sets metrics generation flag
   void do_metrics(VerdeBoolean value);

   //! sets individual metrics flag
   void do_individual(VerdeBoolean value);

   //! sets individual metrics flag
   void set_warning_level(int value);
   
   //! gets interface checking flag
   //VerdeBoolean get_do_interface() { return do_interface; }
   
   //! gets topology checking flag
   //VerdeBoolean get_do_topology() { return do_topology; }

   //! gets metrics checking flag
   //VerdeBoolean get_do_metrics() { return do_metrics; }

   //! loads a mesh into memory from the file
   VerdeStatus load_mesh(const char* filename, VerdeBoolean readfile = VERDE_TRUE);

   //! first time load of mesh. Happens regardless of batch or interactive mode
   VerdeStatus load_initial_mesh();

   //! Access to the current mesh pointer
   Mesh *mesh();

   //! adds a block to be processed to the commandLineBlockList
   void add_block_to_process(int block_id);

   //! get *Success functions
   VerdeBoolean get_metricsSuccess() { return metricsSuccess; }
   VerdeBoolean get_topologySuccess() { return topologySuccess; }
   VerdeBoolean get_interfaceSuccess() { return interfaceSuccess; }

   //! get add_mesh flag
   VerdeBoolean should_add_mesh() { return add_mesh; }

   //! set add_mesh flag
   void set_add_mesh(int flag);

   //! gets output file flag
   VerdeBoolean get_output(){return doOutput;}
   
   //! gets topology checking flag
   VerdeBoolean get_topology(){return doTopology;}

   //! gets interface checking flag
   VerdeBoolean get_interface(){return doInterface;}
   
   //! gets verboseprint_failed_elements output flag
   VerdeBoolean get_print_failed_elements(){return printFailedElements;}

   //! gets metrics generation flag
   VerdeBoolean get_metrics(){return doMetrics;}

   //! gets individual metrics flag
   VerdeBoolean get_individual(){return doIndividual;}

   //! gets current warning level
   int get_warning_level();

   //! access to Mesh Object Interface
   MDBInterface *MDB();

private:
   
   VerdeStatus initialize_signal_handling() const;

   //! File which holds the mesh to be processed
   VerdeString inputFile;

   //- File name of the desired exodus output file
   VerdeString outputFile;

   //- controls whether output file is generated
   VerdeBoolean doOutput;
   
   //- controls whether metrics are generated
   VerdeBoolean doMetrics;
   
   //- controls whether topology verification (Euler No.) is done
   VerdeBoolean doTopology;
   
   //! controls whether interface checking is done
   VerdeBoolean doInterface;
      
   //! controls whether verbosefailed element output is performed
   VerdeBoolean printFailedElements;
   
   //! controls whether metrics are printed for each block processed
   VerdeBoolean doIndividual;
   
   //! flag indicating all metrics passed
   VerdeBoolean metricsSuccess;
   
   //! flag indicating topology calcs passed
   VerdeBoolean topologySuccess;
   
   //! flag indicating interface calcs passed
   VerdeBoolean interfaceSuccess;

   //! flag indicating next mesh read in will be appended  
   VerdeBoolean add_mesh;

   //! pointer to mesh.  This is only defined when a valid mesh has been read in
   Mesh *currentMesh;

   //! contains the block ids specified in the command line arguments
   std::deque<int> commandLineBlockList;

   //! time taken to do analysis in seconds
   float analyze_time;

   //! pointer to Mesh Data Base object
   MDBInterface *MDBObject;

   //! MDBClient that gives us access to the Mesh Data Base
   MDBClient* mdb_client;
   
};

inline void VerdeApp::do_output(VerdeBoolean value)
{
   doOutput = value;
}

inline void VerdeApp::do_metrics(VerdeBoolean value)
{
   doMetrics = value;
}

inline void VerdeApp::do_topology(VerdeBoolean value)
{
   doTopology = value;
}

inline void VerdeApp::do_interface(VerdeBoolean value)
{
   doInterface = value;
}

inline void VerdeApp::print_failed_elements(VerdeBoolean value)
{
   printFailedElements = value;
}

inline void VerdeApp::do_individual(VerdeBoolean value)
{
   doIndividual = value;
}

inline Mesh * VerdeApp::mesh()
{
  return currentMesh;
}

inline void VerdeApp::add_block_to_process(int block_id)
{
  commandLineBlockList.push_back(block_id);
  std::unique(commandLineBlockList.begin(), commandLineBlockList.end());
}

//! global variable. All file that include this file have access to it.
extern VerdeApp *verde_app;

#endif
