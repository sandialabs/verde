 /*!
 *  \class   MDBEntitySequence
 *  \authors Karl Merkley & Corey Ernst
 *  \date    3/27/02
 *  \brief   The MDBEntitySequence represents a contiguous range of 
 *           mesh entities of a single type.  MDBEntitySequence manages
 *           the internal ids of those entities using a starting id and 
 *           the number of entities in the range.  All MDBEntitySequences 
 *           for a given MeshEntity type are stored in an stl Map.  When a 
 *           MeshHandle references an entity in mdb, mdb finds the Entity-
 *           Sequence associated with the MeshHandle from the stl Map.  
 *           All enquires for any data associated with the MeshEntity are
 *           directed through the MeshEntitySequence object to which the
 *           MeshEntity's MeshHandle refers.  A MDBEntitySequence knows 
 *           about all DenseTags defined for that MDBEntitySequence, and
 *           contains one TagRange for each of these DenseTags.  The 
 *           MDBEntitySequence can query for and return any of the DenseTag
 *           data from the TagRanges it manages. 
 *          
 */ 

#ifndef MDB_ENTITY_SEQUENCE
#define MDB_ENTITY_SEQUENCE

#include <vector>
#include <map>
#include "MDBDefines.h" 

class MeshSet;

class MDBEntitySequence
{

private:

  MDBEntityHandle       mStartEntityHandle; //id to 1st element in EntitySequence
  int                   mNumEntities;       //number of Entities in EntitySequence 
  std::vector<MeshSet*> mListOfOwners;      //list of MeshSets that own this EntitySequence
  std::vector<void*>    mTagList;           //list of tags that EntitySequence has
  std::vector<size_t>   mTagSize;           //number bytes per individual tag value 
                                            //mTagSize is parallel to mTagList
  int                   mNumEntitiesReserved;
 
  //!class-wide (static) map of all the EntitySequences that are created 
  static std::map< MDBEntityHandle, MDBEntitySequence* >  mSequenceMap;
       

public:

  //! Constructor
  MDBEntitySequence(MDBEntityHandle start_handle, int num_entities, bool full = true);

  //! Destructor
  ~MDBEntitySequence();

  //! remove the tag handle and free the data
  MDBErrorCode remove_tag(TagHandle tag_handle);

  //! get type of elements in EntitySequence
  MDBEntityType get_type() { return TYPE_FROM_HANDLE(mStartEntityHandle); }

  //! gets data of tag
  MDBErrorCode get_tag_data( MDBEntityHandle handle, 
                             TagHandle tag_handle, 
                             void* data); 
 
  //! sets data of tag
  MDBErrorCode set_tag_data( MDBEntityHandle handle, 
                             TagHandle tag_handle, 
                             void* data, 
                             size_t tag_value_size); 

  size_t get_tag_data_size( TagHandle tag_id ) { return mTagSize[ID_FROM_TAG_HANDLE(tag_id)]; }


//  void set_tag_data( TagHandle tag_handle, void* data, int num_values, size_t tag_value_size); 
 
  //! sets starting MDBEntityHandle (mStartEntityHandle)
  void set_start_handle( MDBEntityHandle handle){ mStartEntityHandle = handle; }

  //! gets starting MDBEntityHandle (mStartEntityHandle) 
  MDBEntityHandle get_start_handle(){ return mStartEntityHandle; }

  //! gets a handle to the last MDBEntityHandle in MDBEntitySequence 
  MDBEntityHandle get_end_handle(){ return mStartEntityHandle + mNumEntities; }

  MDBEntityHandle get_last_reserved_handle() { return mNumEntitiesReserved + mStartEntityHandle; }

  //! returns number of MDBEntityHandles in MDBEntitySequence
  int get_num_entities() { return mNumEntities; }

  //! splits sequence into 2
  void split(MDBEntityHandle handle, MDBEntitySequence* new_sequence);

  //! gets the tag array pointer, allocates memory for tag array if necessary
  MDBErrorCode get_tag_array_ptr(void**    tag_pointer, 
                                 size_t    tag_value_size, 
                                 TagHandle tag_handle);

  //! gets the next avaliable MDBEntityHandle and the entity sequence that 
  //  contains it if it exist
  static MDBErrorCode get_next_available_handle( MDBEntityType       type, 
                                                 MDBEntitySequence*& ent_seq,
                                                 MDBEntityHandle&    next_handle); 

  //! Get the handle of the first entity of a given type
  static MDBErrorCode get_first(MDBEntityType type, MDBEntityHandle& handle);

  //! finds the specific MDBEntitySequence in which MDBEntityHandle resides
  static MDBErrorCode find( MDBEntityHandle     entity_handle,
                            MDBEntitySequence*& sequence );

};


#endif


