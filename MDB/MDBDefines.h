#ifndef MDB_HPP
#define MDB_HPP

enum MDBErrorCode { MDB_SUCCESS = 0,
                    MDB_INDEX_OUT_OF_RANGE,
                    MDB_TYPE_OUT_OF_RANGE,
                    MDB_MEMORY_ALLOCATION_FAILED,
                    MDB_ENTITY_NOT_FOUND,
                    MDB_FILE_DOES_NOT_EXIST,
                    MDB_NOT_IMPLEMENTED,
                    MDB_FAILURE};

// a useful macro for checking the size of a vector and resizing if necessary
#define CHECK_SIZE(this_vector_ptr, curr_size, target_size, this_type) \
  {if (NULL == this_vector_ptr || curr_size < target_size) { \
    if (NULL != this_vector_ptr) free(this_vector_ptr); \
    this_vector_ptr = (this_type*) malloc(target_size*sizeof(this_type)); \
    curr_size = target_size;}}



/*! I don't think that the type definitions as currently
 * defined in TSTT will work for MDB because they are spread
 * across the EntityTopology and the EntityType enums. Therefore
 * we need our own enum
 */


/*! The ordering here must ensure that all element types are 
 *  grouped together and all elements of similar dimension are
 *  grouped together.
 */
enum MDBEntityType
{
  MDBVertex = 0, //!< Mesh Vertex AKA node
  MDBEdge,       //!< Mesh Edge
  MDBFace,       //!< Face  (either quad or tri. Do we really need this?)
  MDBRegion,     //!< Volume region (all between tet and wedge. Do we really need this?)
  MDBPoint,      //!< Point element, e.g. point mass (do we need this?)
  MDBBar,        //!< Bar, beam, truss, spring
  MDBTri,        //!< Triangular element (including shells)
  MDBQuad,       //!< Quadrilateral element (including shells)
  MDBTet,        //!< Tetrahedral element
  MDBHex,        //!< Hexahedral element
  MDBPyramid,    //!< Pyramid element (where are the face ids for this defined?)
  MDBKnife,      //!< Knife element
  MDBWedge,      //!< Wedge element (Exodus has one, Cubit doesn't. Does MDB need it?)
  MDBMeshset,    //!< MeshSet
  MDBMaxType=14  //!< Just a place keeper - must be the # of entities, for array
                 //!< dimensioning purposes 
};


#define MDB_0D_ENTITY 0
#define MDB_1D_ENTITY 1
#define MDB_2D_ENTITY 2
#define MDB_3D_ENTITY 3

extern const int dimension_table[];
extern const char hex_quads[];
extern const char hex_edges[];
extern const char tet_tris[];
extern const char tet_edges[];
extern const char quad_edges[];
extern const char tri_edges[];

  
/*! Define MDBEntityHandle for both 32 bit and 64 bit systems.
 *  The decision to use 64 bit handles must be made at compile time.
 *  \bug we should probably have an Int64 typedef
 *
 *  MDBEntityHandle format:
 *  0xXYYYYYYY  (assuming a 32-bit handle.  Top 4 bits reserved on a 64 bit system)
 *  X - reserved for entity type.  This system can only handle 15 different types
 *  Y - Entity id space.  Max id is over 200M
 *
 *  Note that for specialized databases (such as all hex) 16 bits are not
 *  required for the entity type and the id space can be increased to over 2B.
 */

#ifdef USE_64_BIT_HANDLES
  #define MDB_HANDLE_MASK 0xF000000000000000
  #define MDB_HANDLE_SHIFT_WIDTH 60
  typedef unsigned long long MDBEntityHandle; //!< \bug test this definition
#else
  #define MDB_HANDLE_MASK 0xF0000000
  #define MDB_HANDLE_SHIFT_WIDTH 28
  typedef unsigned long MDBEntityHandle;      //!< Entity handles are stored in 32 bits
#endif

/*! Methods that could have been macros have been defined here as inline
 *  functions to provide type checking by the compiler and error handling.
 */

//! Get the entity id out of the handle.
inline int ID_FROM_HANDLE (MDBEntityHandle handle)
{
  return (handle & ~MDB_HANDLE_MASK);
}

//! Get the type out of the handle.
inline MDBEntityType TYPE_FROM_HANDLE(MDBEntityHandle handle) 
{
  return static_cast<MDBEntityType> (
      ((handle) & MDB_HANDLE_MASK) >> MDB_HANDLE_SHIFT_WIDTH);

}

//! Given a type and an id create a handle.  
inline MDBEntityHandle CREATE_HANDLE(int type, MDBEntityHandle id, int& err) 
{
  err = 0; //< Assume that there is a real error value defined somewhere

  if (id > ~MDB_HANDLE_MASK || type > MDBMaxType)
  {
    err = 1;   //< Assume that there is a real error value defined somewhere
    return 1;  //<You've got to return something.  What do you return?
  }
  
  MDBEntityHandle handle = (type << MDB_HANDLE_SHIFT_WIDTH) + id;

  return handle;
}


/* TagHandle format
 * 0xXXYYZZZZ  ( 32 bits total )
 * Z - reserved for internal sub-tag id
 * Y - reserved for internal entity type
 * X - reserved for internal properties & lookup speed
 */

typedef unsigned int TagHandle;

#define TAG_ID_MASK              0x0000FFFF
#define TAG_ENTITY_TYPE_MASK     0x00FF0000
#define TAG_PROP_MASK            0xFF000000

inline int ID_FROM_TAG_HANDLE(TagHandle tag_handle) 
{
  return ( (tag_handle) & TAG_ID_MASK );
}

inline MDBEntityType TYPE_FROM_TAG_HANDLE(TagHandle tag_handle) 
{
  return static_cast<MDBEntityType>( ((tag_handle) & TAG_ENTITY_TYPE_MASK) >> 16 );
}

#endif


#ifdef UNIT_TEST
#include <iostream>

//! Sample code on a 32-bit system.
int main()
{
  //! define a sample handle
  unsigned int handle = 0x10000010;

  //! display the handle in hex
  std::cout << "Handle: " << std::hex << handle << std::endl;

  //! Get the type and id out of the handle
  std::cout << "Type: " << TYPE_FROM_HANDLE (handle)  << std::endl;
  std::cout << "Id  : " << ID_FROM_HANDLE (handle)  << std::endl;

  //! Create a new handle and make sure it is the same as the first handle
  int err;
  handle = CREATE_HANDLE(1, 16, err);
  if (!err)
    std::cout << "Handle: " << std::hex << handle << std::endl;

  //! define a sample handle
  handle = 0x1FFFFFFF;

  //! display the handle in hex
  std::cout << std::endl;
  std::cout << "Handle: " << std::hex << handle << std::endl;

  //! Get the type and id out of the handle
  std::cout << "Type: " << TYPE_FROM_HANDLE (handle)  << std::endl;
  std::cout << "Id  : " << ID_FROM_HANDLE (handle)  << std::endl;

  //! This handle represents the maximum possible ID in 32 bits.
  std::cout << "Max Id  : " << std::dec << ID_FROM_HANDLE (handle)  << std::endl;

  //! Create a new handle and make sure it is the same as the first handle
  handle = CREATE_HANDLE(1,16, err);
  if (!err)
    std::cout << "Handle: " << std::hex << handle << std::endl;

  for (int i = MDBVertex; i <= 16; i++)
  {
    handle = CREATE_HANDLE(i, 0x0FFFFFFF, err);
    if (!err)
    {
      std::cout << "Handle: " << std::hex << handle << std::endl;
      std::cout << "Type: " << std::dec << TYPE_FROM_HANDLE (handle)  << std::endl;
      std::cout << "Id  : " << ID_FROM_HANDLE (handle)  << std::endl;
    }
    else
      std::cout << "Out of ID space" << std::endl;
  }

  return 0;
}

#endif

