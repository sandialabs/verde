
/**********************************************
 * Filename   :     SparseTagCollections.cpp
 *
 * Purpose    :     To store any size data with
 *                  any entity handle
 *
 * Creator    :     Clinton Stimpson
 *
 * Date       :     3 April 2002
 *
 * ********************************************/


#include "SparseTagCollections.hpp"


/*
  SparseTagSuperCollection functions -----------------------------
*/


SparseTagSuperCollection::~SparseTagSuperCollection()
{
  SparseTagCollection* tag_collection = NULL;

  std::map<TagHandle, SparseTagCollection*>::iterator tag_iterator;
  for(tag_iterator = mDataTags.begin(); tag_iterator != mDataTags.end(); ++tag_iterator)
  {
    tag_collection = (*tag_iterator).second;
    if(tag_collection != NULL)
      delete tag_collection;
  }
  mDataTags.clear();
}

bool SparseTagSuperCollection::add_tag(const TagHandle tag_handle, const int data_size)
{
  if(data_size<=0)
    return false;

  SparseTagCollection* new_tag_collection = new SparseTagCollection(data_size);
  if(!(mDataTags[tag_handle] = new_tag_collection))
  {
    // we couldn't add this to our collection
    // most likely because the tag_handle was not unique
    delete new_tag_collection;
    return false;
  }

  return true;
}

void SparseTagSuperCollection::remove_tag(const TagHandle tag_handle)
{
  std::map<TagHandle, SparseTagCollection*>::iterator iterator =
    mDataTags.find(tag_handle);

  if(iterator != mDataTags.end())
  {
    delete iterator->second;
    mDataTags.erase(iterator);
  }
}

bool SparseTagSuperCollection::set_data(const TagHandle tag_handle, 
    const MDBEntityHandle entity_handle, const void* data)
{
  std::map<TagHandle, SparseTagCollection*>::iterator iterator =
    mDataTags.find(tag_handle);

  if(iterator != mDataTags.end())
    return iterator->second->set_data(entity_handle, data);
  else
    return false;
}

bool SparseTagSuperCollection::get_data(const TagHandle tag_handle, const MDBEntityHandle entity_handle, void* data)
{
  std::map<TagHandle, SparseTagCollection*>::iterator iterator =
    mDataTags.find(tag_handle);

  if(iterator != mDataTags.end())
    return iterator->second->get_data(entity_handle, data);
  else
    return false;
}

void SparseTagSuperCollection::remove_data( const TagHandle tag_handle, const MDBEntityHandle entity_handle )
{
  std::map<TagHandle, SparseTagCollection*>::iterator iterator =
    mDataTags.find(tag_handle);

  if(iterator != mDataTags.end())
    iterator->second->remove_data(entity_handle);
}

TagHandle SparseTagSuperCollection::find_entity( const TagHandle tag_handle, const void* data )
{
  std::map<TagHandle, SparseTagCollection*>::iterator iterator =
    mDataTags.find(tag_handle);

  if(iterator != mDataTags.end())
    return iterator->second->find_entity(data);
  else
    return 0;

}


/*
  SparseTagCollection functions ----------------------------------
*/

SparseTagCollection::SparseTagCollection(int data_size)
{
  mDataSize = data_size;
}

SparseTagCollection::~SparseTagCollection()
{
  void* tag_data = NULL;

  std::map<MDBEntityHandle, void*>::iterator tag_iterator;
  for(tag_iterator = mData.begin(); tag_iterator != mData.end(); ++tag_iterator)
  {
    tag_data = tag_iterator->second;
    if(tag_data != NULL)
      mAllocator.destroy(tag_data);
  }
  mData.clear();
}

bool SparseTagCollection::set_data(const MDBEntityHandle entity_handle, const void* data)
{
  bool ret_val = false;

  std::map<MDBEntityHandle, void*>::iterator iterator =
    mData.find(entity_handle);

  // data space already exists
  if(iterator != mData.end())
  {
    memcpy( iterator->second, data, mDataSize);
    ret_val = true;
  }
  // we need to make some data space
  else 
  {
    void* new_data = mAllocator.allocate(mDataSize);
    memcpy(new_data, data, mDataSize);
    mData.insert( std::pair<MDBEntityHandle, void*>(entity_handle, new_data));
    ret_val = true;
  }

  return ret_val;
}

bool SparseTagCollection::get_data(const MDBEntityHandle entity_handle, void* data)
{
  std::map<MDBEntityHandle, void*>::iterator iterator =
    mData.find(entity_handle);

  if(iterator == mData.end())
    return false;
  
  memcpy(data, iterator->second, mDataSize);
  return true;
}


void SparseTagCollection::remove_data( const MDBEntityHandle entity_handle )
{
  std::map<MDBEntityHandle, void*>::iterator iterator =
    mData.find(entity_handle);

  if(iterator != mData.end())
  {
    mAllocator.destroy(iterator->second);
    mData.erase(iterator);
  }

}


MDBEntityHandle SparseTagCollection::find_entity( const void* data )
{
  std::map<MDBEntityHandle, void*>::iterator iterator;
  for(iterator = mData.begin();
      iterator != mData.end();
      iterator++)
  {
    if(memcmp(data, iterator->second, mDataSize) == 0)
      return iterator->first;
  }

  return 0;
}


/*
  StaticSparseTagCollection functions --------------------------
*/


StaticSparseTagCollection::~StaticSparseTagCollection()
{
  std::map<TagHandle, void*>::iterator tag_iterator;
  for(tag_iterator = mData.begin(); tag_iterator != mData.end(); ++tag_iterator)
  {
    if(tag_iterator->second)
      mAllocator.destroy(tag_iterator->second);
  }
  mData.clear();
  mTagSize.clear();
}


bool StaticSparseTagCollection::add_tag(const TagHandle tag_handle, const int data_size)
{
  std::map<TagHandle, unsigned short>::iterator existing_tag = mTagSize.find(tag_handle);
  if(existing_tag == mTagSize.end())
  {
    mTagSize[tag_handle] = data_size;
    return true;
  }
  return false;
}


void StaticSparseTagCollection::remove_tag(const TagHandle tag_handle)
{
  std::map< TagHandle, void* >::iterator existing_data = mData.find(tag_handle);
  if(existing_data != mData.end())
  {
    mAllocator.destroy( existing_data->second );
    mData.erase(existing_data);
    mTagSize.erase(tag_handle);
  }

}

bool StaticSparseTagCollection::set_data(const TagHandle tag_handle, const void* data)
{
  // find existing data
  std::map<TagHandle, void*>::iterator existing_data = mData.find(tag_handle);
  // find tag data size
  std::map<TagHandle, unsigned short>::iterator existing_tag_size = mTagSize.find(tag_handle);
  
  // check to make sure this tag exists
  if(existing_tag_size == mTagSize.end())
    return false;

  // get data size
  unsigned short data_size = existing_tag_size->second;
  
  // if data space already existed, reuse it
  if(existing_data != mData.end())
    memcpy( existing_data->second, data, data_size);

  // make some space and set the data
  else
  {
    void* new_data = mAllocator.allocate(existing_tag_size->second);
    memcpy(new_data, data, data_size);
    std::pair<TagHandle, void*> tmp(tag_handle, new_data);
    mData.insert(tmp);
  }

  return true;
}

bool StaticSparseTagCollection::get_data(const TagHandle tag_handle, void* data)
{
  // find existing data
  std::map<TagHandle, void*>::iterator existing_data = mData.find(tag_handle);
  // find tag data size
  std::map<TagHandle, unsigned short>::iterator existing_tag_size = mTagSize.find(tag_handle);
  
  // check to make sure this tag and data exists and that data != NULL
  if((existing_tag_size == mTagSize.end()) || (existing_data == mData.end())
    || (existing_data->second == NULL) )
    return false;

  memcpy(data, existing_data->second, existing_tag_size->second);
  return true;
}


