/*  Filename   :     MDBClient.cpp
 *  Creator    :     Clinton Stimpson
 *
 *  Date       :     10 Jan 2002
 *
 *  Owner      :     Clinton Stimpson
 *
 *  Description:     The MDBClient connects with a shared library which
 *                   contains an MDBServer.  Interfaces may be retrieved
 *                   from the MDBServer of a specified uuid.
 */


#include <string.h>
#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif
#include <stdlib.h>

// for the HP machines
#if defined(HP)
#include <dl.h>

// for all other unix machines
#elif !defined(WIN32)
#include <dlfcn.h>

// for windows machine
#else
 
// we will get what we need from MDBClient.hpp

#endif


#include "MDBClient.hpp"
#include "MDBInterface.hpp"


//! constructor
MDBClient::MDBClient(const char* shared_library)
{
   hLib = NULL;
   MDBS_GetInterface = NULL;
   MDBS_DeInitialize = NULL;
   MDBS_ReleaseInterface = NULL;
   MDBS_Version = NULL;

   set_library_name(shared_library);
}


void MDBClient::set_library_name(const char* libname)
{
   // allow the library name to be specified generically and 
   // we find out if it is a {libname}.dll   or lib{libname}.so

   const char* verde_dir = getenv("VERDE_DIR");
   if(verde_dir && strlen(verde_dir) > 0)
   {
	  // lets be a bit generous on the size
	  library_name = new char[strlen(verde_dir) + strlen(libname) + 15];
   
#ifdef WIN32
      strcpy(library_name, verde_dir);
      strcat(library_name, "\\");
      strcat(library_name, libname);
      strcat(library_name, ".dll");
#else
      strcpy(library_name, verde_dir);
      strcat(library_name, "/");
      strcat(library_name, "lib");
      strcat(library_name, libname);
      strcat(library_name, ".so");
#endif

   }
   // VERDE_DIR wasn't set so just hope LD_LIBRARY_PATH is set
   else
   {
	  library_name = new char[strlen(libname) + 15];

#ifdef WIN32
      strcpy(library_name, libname);
      strcat(library_name, ".dll");
#else
      strcpy(library_name, "lib");
      strcat(library_name, libname);
      strcat(library_name, ".so");
#endif
     
      std::cout << "WARNING: VERDE_DIR environment variable not set" << std::endl;
   }

}



// destructor
MDBClient::~MDBClient()
{
   // De-Initialize the library that is loaded
  if(MDBS_DeInitialize != NULL)
    (*MDBS_DeInitialize)();
   
   // free the library
   freeLibrary();
   
   // free our char* library_name
   if(library_name != NULL)
      delete [] library_name;
}

void MDBClient::ReleaseInterface( MDBUnknownInterface* iface)
{
   // see if the library is loaded and the symbols are available
   if(!loadLibrary() || !resolveSymbols())
      return;

   // send our interface to the MDBServer to be released
   (*MDBS_ReleaseInterface)(iface);
}


bool MDBClient::GetInterface(MDBuuid& uuid, MDBUnknownInterface** iface )
{
   // initialize the interface to NULL
   *iface = NULL;
   
   // see if our library is loaded and our symbols are available
   if(!loadLibrary() || !resolveSymbols())
   {
      return false;
   }

   // check to make sure the version of the interface in the 
   // library is not greater than the version we have here. 
   if(strcmp( (*MDBS_Version)() , MDB_INTERFACE_VERSION) > 0 )
   {
      std::cerr << "ERROR: Request for version " << MDB_INTERFACE_VERSION << " when "
		  << library_name << " is version " << (*MDBS_Version)() << std::endl;
      return false;
   }

   // get our interface from the MDBServer
   (*MDBS_GetInterface)(uuid, iface);
   // return success or failure
   return *iface != 0;
}


// load the library
bool MDBClient::loadLibrary()
{
   // if the library is already loaded
   if ( hLib != NULL )
      return true;

   // load the library

   // for the HP platform
#ifdef HP

   hLib = (void*)shl_load( this->library_name, BIND_DEFERRED | BIND_NONFATAL | DYNAMIC_PATH, 0 );
   if(hLib == NULL)
      std::cerr << "Failed to load library " << library_name << std::endl;

   // for all other unix/linux platforms
#elif !defined(WIN32)

   hLib = dlopen( this->library_name, RTLD_LAZY );
   // if there was an error, print it
   if(hLib == NULL)
      std::cerr << dlerror() << std::endl;

   // for the windows platform
#else

   hLib = LoadLibrary( this->library_name );
   // if there was an error, print it
   if(hLib == NULL)
      std::cerr << "Failed to load library " << library_name << std::endl;

#endif

   return hLib != 0;
}

// free the library
bool MDBClient::freeLibrary()
{
   // if there is no library connection, return
   if (hLib != NULL)
      return true;
   
   // unload the library
   
#ifdef HP
   if(!shl_unload( (shl_t)hLib ))
#elif !defined(WIN32)
   if(!dlclose( hLib ) )
#else
   if(!FreeLibrary( hLib ) )
#endif
   {
      hLib = NULL;
      return true;
   }      
   else
      return false;
}


// resolve the symbols we need to get interfaces
bool MDBClient::resolveSymbols()
{
   // return failure if the library isn't loaded
   if(hLib == NULL)
      return false;

   // return success if the symbols are already loaded
   if (MDBS_GetInterface && MDBS_DeInitialize && MDBS_ReleaseInterface && MDBS_Version)
      return true;   

   // get the symbols we need to communicate with the MDBServer
#ifdef HP
   
   shl_findsym( (shl_t*)&hLib, "GetInterface", TYPE_UNDEFINED, &MDBS_GetInterface );
   shl_findsym( (shl_t*)&hLib, "DeInitialize", TYPE_UNDEFINED, &MDBS_DeInitialize );
   shl_findsym( (shl_t*)&hLib, "ReleaseInterface", TYPE_UNDEFINED, &MDBS_ReleaseInterface );
   shl_findsym( (shl_t*)&hLib, "Version", TYPE_UNDEFINED, &MDBS_Version );

#elif !defined(WIN32)
   
   MDBS_GetInterface = (void(*)(MDBuuid&, MDBUnknownInterface**))  
      dlsym( hLib, "GetInterface" );
   MDBS_DeInitialize = (void(*)())  
      dlsym( hLib, "DeInitialize" );
   MDBS_ReleaseInterface = (void(*)(MDBUnknownInterface*))
      dlsym( hLib, "ReleaseInterface" );
   MDBS_Version = (const char* (*)())
      dlsym( hLib, "Version" );
   
#else

   MDBS_GetInterface = (void(MDB_CDECL*)(MDBuuid&, MDBUnknownInterface**))
      GetProcAddress( hLib, "GetInterface" );
   MDBS_DeInitialize = (void(MDB_CDECL*)())
      GetProcAddress( hLib, "DeInitialize" );
   MDBS_ReleaseInterface = (void(MDB_CDECL*)(MDBUnknownInterface*))
      GetProcAddress( hLib, "ReleaseInterface" );
   MDBS_Version = (const char* (MDB_CDECL*)())
      GetProcAddress( hLib, "Version" );

   if( !MDBS_GetInterface )
   {
     MDBS_GetInterface = (void(MDB_CDECL*)(MDBuuid&, MDBUnknownInterface**))
       GetProcAddress( hLib, "_GetInterface" );
     MDBS_DeInitialize = (void(MDB_CDECL*)())
       GetProcAddress( hLib, "_DeInitialize" );
     MDBS_ReleaseInterface = (void(MDB_CDECL*)(MDBUnknownInterface*))
       GetProcAddress( hLib, "_ReleaseInterface" );
     MDBS_Version = (const char* (MDB_CDECL*)())
       GetProcAddress( hLib, "_Version" );
   }

#endif

   // return success or failure
   if( MDBS_GetInterface && MDBS_DeInitialize && MDBS_ReleaseInterface && MDBS_Version )
      return true;
   else
   {
	   std::cerr << "Failed to load symbols for library " << library_name << std::endl;
      return false;
   }
   
}

