
/**********************************************
 * Filename   :     SparseTagServer.hpp
 *
 * Purpose    :     To store any size data with
 *                  any entity handle
 *
 * Creator    :     Clinton Stimpson
 *
 * Date       :     3 April 2002
 *
 * ********************************************/



#ifndef TAG_SERVER_HPP
#define TAG_SERVER_HPP


// system includes
#include <string>
#include <vector>

// 3rd party includes

// our includes
#include "MDBDefines.h"
#include "SparseTagCollections.hpp"


// ! stores information about a tag
class TagInfo
{
public:

  //! defines relatively how many entities this tag will be
  //! attached to so we can store it efficiently
  enum TagProperties 
  {
    TAGPROP_SPARSE = 0x20000000,
    TAGPROP_DENSE  = 0x40000000,
    TAGPROP_STATIC = 0x80000000 
  };
                    
  //! constructor
  TagInfo() : mTagName(""), mDataSize(0) {}
  
  //! destructor
  ~TagInfo(){}
  
  //! copy constructor
  TagInfo(const TagInfo&);

  //! constructor that takes all parameters
  TagInfo(const std::string, int);
  
  //! set the name of the tag
  void set_name( const std::string name) { mTagName = name; }

  //! set the size of the data
  void set_size( const int size ) { mDataSize = size; }

  //! get the name of the tag
  const std::string& get_name() const { return mTagName; }

  //! get the size of the data
  int get_size() const { return mDataSize; }

private:    

  //! stores the tag name
  std::string mTagName;

  //! stores the size of the data for this tag
  unsigned short mDataSize;

};


//! SparseTagServer class which associates tag data with entities
class TagServer
{
public:
  //! constructor
  TagServer();
  //! destructor
  virtual ~TagServer();

  //! add a tag
  TagHandle add_tag(const std::string tag_name, 
              const MDBEntityType entity_type,
              const int data_size,
              const TagInfo::TagProperties tag_property);

  // tag name is the name of the tag
  // entity_type is the type of entity (in implementation - it means
  // tag data is stored in a different namespace)
  // entity_handle is the entity to which data is tagged to
  // tag_usage defines which storage mechanism to use
  // data is the actual data 
  // data_size is the size of the data in bytes

  //! remove a tag from this tag server
  void remove_tag(const TagHandle tag_handle);

  //! set the value of a tag
  bool set_data(const TagHandle tag_handle, const MDBEntityHandle entity_handle, const void* data );

  //! get the value of a tag
  bool get_data(const TagHandle tag_handle, const MDBEntityHandle entity_handle, void* data );

  //! remove the data
  void remove_data( const TagHandle tag_handle, const MDBEntityHandle entity_handle );

  //! finds the first entity handle that matches this data
  MDBEntityHandle find_entity( const TagHandle tag_handle, const void* data );

  //! gets a tag handle by name and entity handle
  TagHandle get_handle(const std::string tag_name, MDBEntityType entity_type);

  //! get information about a tag
  TagInfo get_tag_info( const std::string tag_name );
  TagInfo get_tag_info( const TagHandle tag_handle );

private:

  //! generates a tag id this is unique
  TagHandle generate_tag_handle(TagInfo::TagProperties tag_prop, MDBEntityType entity_type);

  //! returns whether a tag id and entity handle are compatible
  bool tag_entity_compatible(const TagHandle tag_handle, const MDBEntityHandle entity_handle);


  //! table of tag ids and tag information
  //! do we really need to do it this way?
  //! we at least need a table between names and tag ids
  //! and we need information between tag ids and tag usage
  std::map< TagHandle, TagInfo >  mTagTable;

  std::vector< std::vector< MDBEntityType > > mDenseTagIdTypeTable;

  //! container for storing the sparse data and tag ids
  SparseTagSuperCollection mSparseData;

  //! container for storing the static sparse data and tag ids
  StaticSparseTagCollection mStaticSparseData;

};



inline TagInfo::TagInfo(const TagInfo& copy)
{
  mTagName = copy.mTagName;
  mDataSize = copy.mDataSize;
}

inline TagInfo::TagInfo(const std::string name, int size)
{
  mTagName = name;
  mDataSize = size;
};


#endif //TAG_SERVER_HPP




