/*  Filename   :     MDBUnkonwnInterface.h
 *  Creator    :     Clinton Stimpson
 *
 *  Date       :     10 Jan 2002
 *
 *  Owner      :     Clinton Stimpson
 *
 *  Description:     Contains declarations for MDBuuid which keeps
 *                   track of different interfaces.
 *                   Also contains the declaration for the base class
 *                   MDBUknownInterface from which all interfaces are
 *                   derived from
 */

#ifndef MDBUNKNOWNINTERFACE_HPP
#define MDBUNKNOWNINTERFACE_HPP

#include <memory.h>

// this is the calling convention for the interface
// this allows us to a dll made by borland and microsoft
// all interface functions must use the IDECL
// all non-interface exported functions must use the CDECL
#ifdef WIN32
#define MDB_IDECL __stdcall
#define MDB_CDECL __cdecl
#else
#define MDB_IDECL
#define MDB_CDECL
#endif

typedef unsigned char   MDBuchar;
typedef unsigned short  MDBushort;
typedef unsigned        MDBuint;

//!  struct that handles universally unique id's for the Mesh Database

// note: this MDBuuid is compliant with the windows GUID.  
// It is possible to do a memcpy() to copy the data from a MDBuuid to a GUID
// if we want to support dll registration
struct MDBuuid
{
   //! default constructor that initializes to zero
   MDBuuid()
   {
      memset( this, 0, sizeof(MDBuuid) );
   }
   //! constructor that takes initialization arguments
   MDBuuid( MDBuint l, MDBushort w1, MDBushort w2, 
         MDBuchar b1, MDBuchar b2, MDBuchar b3, 
         MDBuchar b4, MDBuchar b5, MDBuchar b6, 
         MDBuchar b7, MDBuchar b8 )
   {
      data1 = l;
      data2 = w1;
      data3 = w2;
      data4[0] = b1;
      data4[1] = b2;
      data4[2] = b3;
      data4[3] = b4;
      data4[4] = b5;
      data4[5] = b6;
      data4[6] = b7;
      data4[7] = b8;
   }
   //! copy constructor
   MDBuuid( const MDBuuid& mdbuuid )
   {
      memcpy( this, &mdbuuid, sizeof(MDBuuid));
   }
   //! sets this uuid equal to another one
   MDBuuid &operator=(const MDBuuid& orig)
   {
      memcpy( this, &orig, sizeof(MDBuuid));
      return *this;
   }
   //! returns whether two uuid's are equal
   bool operator==(const MDBuuid& orig) const
   {
      return !memcmp(this, &orig, sizeof(MDBuuid));
   }
   //! returns whether two uuid's are not equal
   bool operator!=(const MDBuuid& orig) const
   {
      return!(*this == orig);
   }

   //! uuid data storage
   MDBuint   data1;
   MDBushort data2;
   MDBushort data3;
   MDBuchar  data4[8];
};
  
//! MDBuuid for an unknown interface
//! this can be used to either return a default interface
//! or a NULL interface
static const MDBuuid IDD_MDBUnknown = MDBuuid( 0xf4f6605e, 0x2a7e, 0x4760, 
   0xbb, 0x06, 0xb9, 0xed, 0x27, 0xe9, 0x4a, 0xec );


//! base class for all interface classes
class MDBUnknownInterface
{
public:
   virtual int MDB_IDECL QueryInterface
      ( const MDBuuid&, MDBUnknownInterface** ) = 0;
};


#endif  // MDBUNKNOWNINTERFACE_HPP

