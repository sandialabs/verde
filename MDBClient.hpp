/*  Filename   :     MDBClient.hpp
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


#include "MDBUnknownInterface.h"

#ifdef WIN32
// just get the declarations we need
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
 #define WINAPI
#endif


//! MDBClient connects with the MDBServer and retrieves interfaces
class MDBClient
{
public:
   //! constructor - takes shared library name
   MDBClient(const char* shared_library);
   //! destructor
   ~MDBClient();

   //! retrieves the interface with specified uuid
   bool GetInterface(MDBuuid& uuid, MDBUnknownInterface** );
   //! releases the interface
   void ReleaseInterface(MDBUnknownInterface*);

private:

   //! sets the absolute name of the library based on a generic name
   void set_library_name(const char*);
   //! loads the library
   bool loadLibrary();
   //! frees the library
   bool freeLibrary();
   //! resolves the symbols we need to connect to the MDBServer
   bool resolveSymbols();
   //! string representation of library
   char* library_name;
   //! pointer to library
#ifdef WIN32
   HINSTANCE hLib;
#else
   void* hLib;
#endif

   // symbols to resolve from MDBServer
   void ( MDB_CDECL *MDBS_GetInterface)(MDBuuid&, MDBUnknownInterface**);
   void ( MDB_CDECL *MDBS_DeInitialize)();
   void ( MDB_CDECL *MDBS_ReleaseInterface)(MDBUnknownInterface*);
   const char* ( MDB_CDECL *MDBS_Version)();
};
