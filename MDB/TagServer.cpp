
/**********************************************
 * Filename   :     TagServer.hpp
 *
 * Purpose    :     To store any size data with
 *                  any entity handle
 *
 * Creator    :     Clinton Stimpson
 *
 * Date       :     3 April 2002
 *
 * ********************************************/



#ifdef WIN32
#ifdef _DEBUG
// turn off warnings that say they debugging identifier has been truncated
// this warning comes up when using some STL containers
#pragma warning(disable : 4786)
#endif
#endif


#include <assert.h>
#include "TagServer.hpp"
#include "MDBEntitySequence.hpp" // find used by set_data
/*
  TagServer functions ----------------------------------
*/

TagServer::TagServer()
{}

TagServer::~TagServer()
{}

TagHandle TagServer::add_tag(const std::string tag_name, 
                       const MDBEntityType entity_type, 
                       const int data_size,
                       const TagInfo::TagProperties tag_prop)
{

  // verify that the name doesn't already exist for this entity type
  for(std::map<TagHandle, TagInfo>::iterator tag_iterator = mTagTable.begin();
      tag_iterator != mTagTable.end();
      ++tag_iterator)
  {
    // \bug there ought to be a better way to compare to std::strings
    if(( TYPE_FROM_TAG_HANDLE( tag_iterator->first ) == entity_type ) &&
       ( tag_name  == tag_iterator->second.get_name() ) )
    {
      return 0;
    }
  }


  TagHandle tag_handle = generate_tag_handle(tag_prop, entity_type);
  if(tag_handle == 0)
    return 0;


  if(tag_handle & TagInfo::TAGPROP_DENSE)
  {
    //todo: tell RMeshSet to add tag
  }
  // we are attaching sparse data to the entities
  else if (tag_handle & TagInfo::TAGPROP_SPARSE)
  {
    if(!mSparseData.add_tag(tag_handle, data_size))
      tag_handle = 0;
  }
  else if(tag_handle & TagInfo::TAGPROP_STATIC)
  {
    if(!mStaticSparseData.add_tag(tag_handle, data_size))
      tag_handle = 0;
  }
  else
    return 0;

  // we have a valid id, lets register it
  if(tag_handle > 0)
  {
    mTagTable[tag_handle] = TagInfo(tag_name, data_size);
  }
  return tag_handle;

}


void TagServer::remove_tag(const TagHandle tag_handle)
{

  const std::map<TagHandle, TagInfo>::iterator iterator = mTagTable.find(tag_handle);
  
  if(iterator == mTagTable.end())
    return;

  mTagTable.erase(iterator);

  if(tag_handle & TagInfo::TAGPROP_DENSE)
  {
    //todo: tell RMeshSet to remove tag
  }
  else if(tag_handle & TagInfo::TAGPROP_SPARSE)
  {
    mSparseData.remove_tag(tag_handle);
  }
  else if(tag_handle & TagInfo::TAGPROP_STATIC)
  {
    mStaticSparseData.remove_tag(tag_handle);
  }

}

bool TagServer::set_data(const TagHandle tag_handle, 
                       const MDBEntityHandle entity_handle, 
                       const void* data)
{
  if(!tag_entity_compatible(tag_handle, entity_handle))
    return false;

  // The entity handle is valid iff it is in an entity sequence
  MDBEntitySequence* seq;

  MDBErrorCode status = MDBEntitySequence::find(entity_handle, seq);

  if (status != MDB_SUCCESS || seq == 0) //these two tests are probably synomous
    return false;

  if( tag_handle & TagInfo::TAGPROP_DENSE)
  {
    //todo: call RMS set data 
  }
  else if(tag_handle & TagInfo::TAGPROP_SPARSE)
  {
    return mSparseData.set_data(tag_handle, entity_handle, data);
  }
  else if(tag_handle & TagInfo::TAGPROP_STATIC)
  {
    return mStaticSparseData.set_data(tag_handle, data);
  }
  
  return true;
}


bool TagServer::get_data(const TagHandle tag_handle,
                       const MDBEntityHandle entity_handle,
                       void* data)
{

  if(!tag_entity_compatible(tag_handle, entity_handle))
    return false;


  if(tag_handle & TagInfo::TAGPROP_DENSE)
  {
    //todo: call RMS get data
  }
  else if(tag_handle & TagInfo::TAGPROP_SPARSE)
  {
    return mSparseData.get_data(tag_handle, entity_handle, data);
  }
  else if(tag_handle & TagInfo::TAGPROP_STATIC)
  {
    return mStaticSparseData.get_data(tag_handle, data);
  }

  return true;
}


TagHandle TagServer::get_handle(const std::string tag_name, MDBEntityType entity_type)
{
  std::map<TagHandle, TagInfo>::iterator iterator;
  for(iterator = mTagTable.begin(); iterator != mTagTable.end(); ++iterator)
  {
    if(( TYPE_FROM_TAG_HANDLE (iterator->first) == entity_type ) &&
       ( tag_name  == iterator->second.get_name() ) )
    {
        return iterator->first;
    }
  }

  return 0;
}

TagHandle TagServer::generate_tag_handle(TagInfo::TagProperties tag_prop, MDBEntityType entity_type)
{

  TagHandle new_tag_handle = 0;
  
  // we have 8 bits reserved for entity type
  // if we go over this, then we need to make modifications
  assert((entity_type & 0xFF) == entity_type );

  // for dense tags, we try to reuse the base tag id
  if(tag_prop == TagInfo::TAGPROP_DENSE)
  {
    // see if we have a tag id we can resuse
    TagHandle tag_id = 0;
    for(tag_id = 0; tag_id < mDenseTagIdTypeTable.size(); tag_id++ )
    {
      bool tag_id_used = false;
      std::vector< MDBEntityType >::iterator jter;
      for(jter = mDenseTagIdTypeTable[tag_id].begin(); 
          jter != mDenseTagIdTypeTable[tag_id].end(); ++jter)
      {
        // if the flag is used
        if( *jter == entity_type )
        {
          tag_id_used = true;
          break;
        }
      }
      if(tag_id_used == false)
        break;        
    }

    // we didn't find a base tag id we could resuse
    if(tag_id == mDenseTagIdTypeTable.size())
    {
      // add another tag_id to this list
      mDenseTagIdTypeTable.push_back( std::vector<MDBEntityType>() );      
    }

    // add this entity type to the list
    mDenseTagIdTypeTable[tag_id].push_back(entity_type);

    // create the TagHandle
    new_tag_handle = tag_id;
    new_tag_handle |= (entity_type << 16);
    new_tag_handle |= TagInfo::TAGPROP_DENSE;
   
    // and return the tag handle
    return new_tag_handle;
    
  }
  else
  {
    static unsigned short last_tag_handle = 0;
    
    bool tag_handle_used = true;
    
    int attempt_limit = 10000;
    int tries = 0;
    
    while((tag_handle_used == true) && (tries < attempt_limit))
    {
      last_tag_handle++;
      if(last_tag_handle == 0)
        last_tag_handle++;
      
      
      new_tag_handle = last_tag_handle;
      
      if(tag_prop & TagInfo::TAGPROP_SPARSE)
        new_tag_handle |= TagInfo::TAGPROP_SPARSE;
      else if(tag_prop & TagInfo::TAGPROP_STATIC)
        new_tag_handle |= TagInfo::TAGPROP_STATIC;
      
  
      new_tag_handle = new_tag_handle | (entity_type << 16);
      
      if(mTagTable.find(new_tag_handle) == mTagTable.end())
        tag_handle_used = false;
      
      tries++;
      
    }
    
    if(tries >= attempt_limit)
      return 0;
  
    return new_tag_handle;
  }
  
  
}


void TagServer::remove_data( const TagHandle tag_handle, const MDBEntityHandle entity_handle )
{
  if(!tag_entity_compatible(tag_handle, entity_handle))
    return;

/*
  // there is no remove_data equivalent for Dense tags
  if( tag_handle & TagInfo::TAGPROP_DENSE)
  {

  }
  else
*/
  if(tag_handle & TagInfo::TAGPROP_SPARSE)
  {
    mSparseData.remove_data(tag_handle, entity_handle);
    return;
  }

/*
  // use the delete_tag function instead of this
  else if(tag_handle & TagInfo::TAGPROP_STATIC)
  {
    return mStaticSparseData.set_data(tag_handle, data);
  }
*/
  

}

bool TagServer::tag_entity_compatible(const TagHandle /*tag_handle*/, const MDBEntityHandle /*entity_handle*/)
{
  // todo: put some more code here
  return true;
}


MDBEntityHandle TagServer::find_entity( const TagHandle tag_handle, const void* data )
{
  
  if(tag_handle & TagInfo::TAGPROP_SPARSE)
  {
    return mSparseData.find_entity(tag_handle, data);
  }
  else if (tag_handle & TagInfo::TAGPROP_SPARSE)
  {
    // todo:  call RMS find entity
    return 0;
  }
  else 
    return 0;

}


TagInfo TagServer::get_tag_info( const std::string tag_name )
{
  std::map<TagHandle, TagInfo>::iterator iterator;
  for(iterator = mTagTable.begin(); iterator != mTagTable.end(); ++iterator)
  {
      if ( tag_name  == iterator->second.get_name() ) 
        return iterator->second;
  }
  return TagInfo();
}

TagInfo TagServer::get_tag_info( const TagHandle tag_handle )
{
  std::map<TagHandle, TagInfo>::iterator iterator = mTagTable.find( tag_handle);

  if ( iterator != mTagTable.end() )
      return iterator->second;
  
  return TagInfo();
}


#ifdef TAG_SERVER_TEST

#include <iostream>
#include <time.h>
#include <assert.h>
#include <memory.h>

#include "TagServer.hpp"


const int MAX_ID = 10000;
const int SET_TAG_LOOPS = 0xFFF;
const int TEST_LOOPS = 3000;
const int TAG_NAME_SIZE = 3;
const int MAX_TAG_DATA_SIZE = 10;

int main()
{
  TagServer tag_server;

  std::string tag_name;
  tag_name.reserve(TAG_NAME_SIZE+1);
  //char tag_name[TAG_NAME_SIZE+1] = {0};
  TagHandle my_tag_handle;
  unsigned char tag_data[MAX_TAG_DATA_SIZE] = {0};
  unsigned char tag_size;
  unsigned int entity_type;


  tag_server.add_tag("densex", static_cast<MDBEntityType>(0x3), 4, TagInfo::TAGPROP_DENSE);

  // test for robustness
  tag_server.get_data(3245, 324, tag_data);

  for(int test_loops = 0; test_loops < TEST_LOOPS; test_loops++)
  {
    tag_name.resize(0);
    srand(clock());
    for(int stn=0; stn<TAG_NAME_SIZE; stn++)
    {
      // get a letter between A-D
      tag_name += static_cast<char>((rand()/(RAND_MAX+1.0))*('D' - 'A') + 'A');
    }

    // tag size between 1-MAX
    tag_size = static_cast<char>((rand()/(RAND_MAX+1.0))*MAX_TAG_DATA_SIZE + 1);

    // random entity type
    entity_type = (0x3 & rand()) + 1;

    if((my_tag_handle = tag_server.add_tag(tag_name, static_cast<MDBEntityType>(entity_type), tag_size, TagInfo::TAGPROP_SPARSE)) != 0)
    {
      std::cout << "adding tag - " << tag_name << " " << my_tag_handle << " with entity type " << entity_type << " of size " << static_cast<int>(tag_size) << std::endl;

      TagHandle tmp_tag_handle = tag_server.get_handle(tag_name, static_cast<MDBEntityType>(entity_type));

      if(tmp_tag_handle != my_tag_handle)
      {
        std::cout << " error ---- " << tag_name << " is " << my_tag_handle << " but get_id returned " << tmp_tag_handle << std::endl;
        assert(0);
      }

      // test for robustness
      tag_server.get_data(my_tag_handle, rand(), tag_data);

      // test the find_entity function.  This test succeeds on failure
      tag_data[0] = rand();
      tag_server.set_data(my_tag_handle, 200, tag_data);
      assert( 200 != tag_server.find_entity(my_tag_handle, tag_data));

      for( int i=0; i<SET_TAG_LOOPS; i++ )
      {
        MDBEntityHandle id = (MAX_ID*rand())/(RAND_MAX+1);
        for(int j=0; j<tag_size; j++)
          tag_data[j] = rand();

        unsigned char tag_data_save[MAX_TAG_DATA_SIZE];
        memcpy(tag_data_save, tag_data, sizeof(tag_data));
        if(tag_server.set_data(my_tag_handle, id, &tag_data))
        {
          for(int j=0; j<tag_size; j++)
            tag_data[j] = rand();

          if(tag_server.get_data(my_tag_handle, id, &tag_data))
            assert(memcmp(tag_data, tag_data_save, tag_size) == 0);
        }
        tag_server.remove_data(my_tag_handle, id);
      }

    }
    else
    {
      unsigned int entity_type_tmp = (0x3 & rand()) + 1;
      // try to get the handle and remove it
      TagHandle tmp = tag_server.get_handle(tag_name, 
            static_cast<MDBEntityType>( entity_type_tmp )); 
      if(tmp)
      {
        std::cout << " removing tag - " << tag_name << " with entity type " << entity_type_tmp << std::endl;
        tag_server.remove_tag(tmp);
      }
    }
  }
 
  std::cout << std::endl << "---SUCCESS---" << std::endl << std::endl; 
  return 0;
}


#endif
