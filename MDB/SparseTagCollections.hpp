
/**********************************************
 * Filename   :     SparseTagCollections.hpp
 *
 * Purpose    :     To store any size data with
 *                  any entity handle
 *
 * Creator    :     Clinton Stimpson
 *
 * Date       :     3 April 2002
 *
 * ********************************************/



#ifndef SPARSE_TAG_COLLECTIONS_HPP
#define SPARSE_TAG_COLLECTIONS_HPP


// system includes
#include <map>

// 3rd party includes

// our includes
#include "MDBDefines.h"


//! allocator for tag data
class SparseTagDataAllocator
{
public:
  //! constructor
  SparseTagDataAllocator(){}
  //! destructor
  ~SparseTagDataAllocator(){}
  //! allocates memory of size and returns pointer
  void* allocate(size_t data_size) { return malloc(data_size); }
  //! frees the memory
  void destroy(void* p){ free(p); }
};


//! collection of tag data associated with entity ids
class SparseTagCollection
{
public:
  
  //! constructor takes tag data size
  SparseTagCollection(int data_size);
  
  //! destructor
  ~SparseTagCollection();
  
  //! set the tag data for an entity id
  bool set_data(const MDBEntityHandle entity_handle, const void* data);

  //! get the tag data for an entity id
  bool get_data(const MDBEntityHandle entity_handle, void* data);

  //! removes the data
  void remove_data(const MDBEntityHandle entity_handle);
  
  //! finds the first entity handle that matches this data
  MDBEntityHandle find_entity( const void* data );

protected:
  
  //! hidden constructor
  SparseTagCollection(){}
  
  //! size of the data
  unsigned short mDataSize;

  //! allocator for this collection
  SparseTagDataAllocator mAllocator;

  //! map of entity id and tag data
  std::map<MDBEntityHandle /*entity_handle*/ , void* /*data*/ > mData;

};


//! collection of static tag data
class StaticSparseTagCollection
{
public:
  //! constructor
  StaticSparseTagCollection(){}

  //! destructor
  ~StaticSparseTagCollection();

  //! add a tag 
  bool add_tag(const TagHandle tag_handle, const int data_size);
  
  //! remove a tag
  void remove_tag(const TagHandle tag_handle);

  //! set the tag data
  bool set_data(const TagHandle tag_handle, const void* data);

  //! get the tag data
  bool get_data(const TagHandle tag_handle, void* data);
  
protected:

  //! allocator for this container
  SparseTagDataAllocator mAllocator;

  //! map between tag ids and tag sizes
  std::map<TagHandle /*tag_handle*/, unsigned short /*tag_data_size*/ > mTagSize;

  //! map between tag ids and data
  std::map<TagHandle /*tag_handle*/, void* /*data*/ > mData;
};


//! a collection of SparseTagCollections
class SparseTagSuperCollection
{
public:
  //! constructor
  SparseTagSuperCollection(){}
  
  //! destructor
  virtual ~SparseTagSuperCollection();

  //! add a tag to this collection
  bool add_tag(const TagHandle tag_handle, const int data_size);

  //! remove a tag from this collection
  void remove_tag(const TagHandle tag_handle);

  //! set the data of a tag
  bool set_data(const TagHandle tag_handle, const MDBEntityHandle entity_handle, const void* data);

  //! get the data of a tag
  bool get_data(const TagHandle tag_handle, const MDBEntityHandle entity_handle, void* data);

  //! removes data
  void remove_data(const TagHandle tag_handle, const MDBEntityHandle entity_handle);
  
  //! finds the entity handle with this data
  TagHandle find_entity( const TagHandle tag_handle, const void* data );

private:

  //! map of tag_ids with tag containers
  std::map< TagHandle /*tag_handle*/ , SparseTagCollection* > mDataTags;
};


#endif //SPARSE_TAG_COLLECTIONS_HPP




