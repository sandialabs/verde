
#ifdef WIN32
#ifdef _DEBUG
// turn off warnings that say they debugging identifier has been truncated
// this warning comes up when using some STL containers
#pragma warning(disable : 4786)
#endif
#endif


#include "MDBEntitySequence.hpp"
#include <assert.h>

std::map< MDBEntityHandle, MDBEntitySequence* > MDBEntitySequence::mSequenceMap;

//Constructor
MDBEntitySequence::MDBEntitySequence( MDBEntityHandle start_handle, int num_entities, bool full)
{
  // set first entity handle in sequence
  mStartEntityHandle = start_handle;

  // set number of entity handles in sequence 
  mNumEntities = num_entities;

  // insert this sequence into static map
  mSequenceMap.insert( std::pair< MDBEntityHandle, MDBEntitySequence*> ( mStartEntityHandle, this));

  if(full)
    mNumEntitiesReserved = num_entities;
  else
    mNumEntitiesReserved = 0;
}

MDBEntitySequence::~MDBEntitySequence()
{
  //go through mTagList freeing
  for(unsigned int i=0; i<mTagList.size(); i++)
  {
    if(mTagSize[i] != 0)
      free(mTagList[i]);
  }
}

MDBErrorCode MDBEntitySequence::remove_tag(TagHandle tag_handle)
{
  MDBErrorCode ret_val = MDB_FAILURE;

  // get the tag id out of tag handle
  unsigned int tag_id = ID_FROM_TAG_HANDLE( tag_handle ); 
  
  // make sure the tag_id is valid
  if (tag_id < mTagSize.size())
  {
    if(mTagSize[tag_id] != 0)
    {
      free(mTagList[tag_id]);
      mTagSize[tag_id] = 0;
      mTagList[tag_id] = 0;

      ret_val = MDB_SUCCESS;
    }
  }

  return ret_val;
}

MDBErrorCode MDBEntitySequence::get_tag_data( MDBEntityHandle handle, 
                                              TagHandle tag_handle, 
                                              void* data)
{
  
  // get index so you know how much to offset into tag
  int index = handle - mStartEntityHandle;

  // get the tag id out of tag handle
  int tag_id = ID_FROM_TAG_HANDLE( tag_handle ); 

  // cast because you can use pointer arithmatic on void pointers
  unsigned char* tag_pointer = static_cast<unsigned char*>(mTagList[tag_id]);

  // offset into tag to get data
  unsigned char* temp_data = tag_pointer + (index * mTagSize[tag_id]);   
 
  // memcpy data so that client can't mess with data 
  memcpy(data, temp_data, mTagSize[tag_id]);

  return MDB_SUCCESS;

}

MDBErrorCode MDBEntitySequence::set_tag_data( const MDBEntityHandle handle, 
                                              const TagHandle tag_handle, 
                                              void *data, 
                                              const size_t tag_value_size)
{
  // get id out of tag handle 
  unsigned int tag_id = ID_FROM_TAG_HANDLE( tag_handle ); 

  // see if tag_id exists in mTagList
  while(tag_id >= mTagList.size())
  {
    //resize vector to fit new tag
    mTagList.push_back(0);
    mTagSize.push_back(0);
  }
  
  //determine if we need to allocate memory
  if(mTagList[tag_id] == 0 )
  {
    mTagList[tag_id] = malloc( tag_value_size * mNumEntities );
    mTagSize[tag_id] = tag_value_size;
  }

  // tag_value_size must be the same as what has been set in mTagSize
  else if(tag_value_size != mTagSize[tag_id]) 
    return MDB_FAILURE;

  // get index so you know how much to offset into tag
  int index = handle - mStartEntityHandle;

  // cast because you can use pointer arithmatic on void pointers
  unsigned char* tag_pointer = static_cast<unsigned char*>(mTagList[tag_id]);

  // offset into tag to get data
  unsigned char *temp_data = tag_pointer + (index * mTagSize[tag_id]);   

  // memcpy data so that client can't mess with data 
  memcpy(temp_data, data, tag_value_size);

  // grow this if necessary
  if(handle >= mStartEntityHandle + mNumEntitiesReserved)
    mNumEntitiesReserved = handle - mStartEntityHandle + 1;


  return MDB_SUCCESS;
  
}

/*
bool MDBEntitySequence::set_tag_data( int tag_handle, void *data, int num_values, size_t tag_value_size)
{
  //get id out of tag handle 
  int tag_id = ID_FROM_TAG_HANDLE( tag_handle ); 

  //set num_entities
  mNumEntities = num_values;

  //set tag size
  mTagSize[tag_id] = tag_value_size;

  //put data into tag list vector 
  mTagList[tag_id] = data;

  //get entity type
  int type = TYPE_FROM_TAG_HANDLE( tag_handle ); 
  
  MDBEntityHandle next_handle = get_next_handle(type);

  mStartEntityHandle = next_handle;

  return true;
  
}
*/

MDBErrorCode MDBEntitySequence::get_tag_array_ptr(void ** tag_pointer, 
                                                  size_t tag_value_size, 
                                                  TagHandle tag_handle )
{

  MDBErrorCode ret_val = MDB_FAILURE;

  // put pointer to tag_pointer in mTagList 
  unsigned int tag_id = ID_FROM_TAG_HANDLE( tag_handle ); 

  // see if tag_id exists in mTagList
  while(tag_id >= mTagList.size())
  {
    // resize vectors to fit new tag
    mTagList.push_back(0);
    mTagSize.push_back(0);
  }

  // if tag list doesn't have correct size, croak 
  //assert( sizeof(mTagList[tag_id]) == tag_value_size * mNumEntities );

  // malloc needed size of tag array
  if(!mTagList[tag_id])
  {
    mTagList[tag_id] = malloc( tag_value_size * mNumEntities );
    mTagSize[tag_id] = tag_value_size;
  }

  *tag_pointer = mTagList[tag_id];

  if (!tag_pointer)
    ret_val = MDB_MEMORY_ALLOCATION_FAILED;
  else
    ret_val = MDB_SUCCESS;

  return ret_val;
}

/*
MDBEntityHandle MDBEntitySequence::split(MDBEntityHandle handle, MDBEntitySequence *new_sequence)
{
  // get data needed to create new sequence
  int number_entities = mNumEntities - handle + 1;

  // create new sequence 
  new_seqence = new MDBEntitySequence(handle, number_entities);
  
  // deal with tag list

  // fix "this" sequence
  mNumEntities = handle - mStartEntityHandle; 
}
*/


MDBErrorCode MDBEntitySequence::get_first(MDBEntityType type, MDBEntityHandle& handle)
{
  // create possibly bogus lowest allowable handle
  int error;
  MDBErrorCode status = MDB_FAILURE;

  MDBEntityHandle lowest_handle = CREATE_HANDLE( type, 0, error);
  if (error)
    return MDB_TYPE_OUT_OF_RANGE;
    
  // the sequence map _should_ never be empty
  if ( mSequenceMap.empty() )
    return MDB_FAILURE;

  // using the map, find out where last sequence (sequence with highest EntityHandle) is.
  std::map<MDBEntityHandle, MDBEntitySequence*>::iterator iter =
      mSequenceMap.lower_bound( lowest_handle );

  //MDBEntitySequence* seq = (--iter)->second;
  MDBEntitySequence* seq = iter->second;

  if( seq && seq->get_type() == type)
  {
    handle = seq->mStartEntityHandle;
    status = MDB_SUCCESS;
  }

  return status;
}


MDBErrorCode MDBEntitySequence::get_next_available_handle(MDBEntityType type, 
                                                          MDBEntitySequence*& ent_seq, 
                                                          MDBEntityHandle& next_handle)
{
  if ( mSequenceMap.empty() )
    return MDB_FAILURE; 

  // create bogus highest allowable handle (2^27) 
  int error;
  MDBEntityHandle max_allowable_handle = CREATE_HANDLE( type, ~MDB_HANDLE_MASK, error);

  // using the map, find out where last sequence (sequence with highest EntityHandle) is.
  std::map<MDBEntityHandle, MDBEntitySequence*>::iterator iter =
    mSequenceMap.upper_bound(max_allowable_handle);
  ent_seq = (--iter)->second;

  // this is the first element of this type
  if(ent_seq->get_type() != type)
  {
    ent_seq = 0;
    next_handle = CREATE_HANDLE(type, 0, error);

    return MDB_SUCCESS;
  }

  // get last entity handle + 1
  MDBEntityHandle last_handle = ent_seq->mStartEntityHandle + ent_seq->mNumEntitiesReserved; 

  //make sure last_handle is < max_allowable_handle
  if(last_handle < max_allowable_handle )
  {
    next_handle = last_handle;
    return MDB_SUCCESS;
  } 
  else
  {
    ent_seq = 0; 
    next_handle = 0;  // \bug is there a way to indicate that a handle is bad?

    return MDB_FAILURE;
  }
}

MDBErrorCode MDBEntitySequence::find( MDBEntityHandle entity_handle,
                                      MDBEntitySequence*& sequence )
{
  // create an iterator to look for which sequence entity handle will be found in
  // using lower bounds function of map

  MDBErrorCode ret_val = MDB_FAILURE;
  sequence = 0;

  if ( !mSequenceMap.empty() )
  {
    std::map<MDBEntityHandle, MDBEntitySequence*>::iterator iter =
      mSequenceMap.upper_bound(entity_handle);

    --iter;

    // ensure that the entity is bounded by this sequence.  
    // upper_bound will indicate that this sequence can hold this entity
    // but doesn't guarantee that it does!
    MDBEntitySequence* seq = iter->second; 
    
    if ( (entity_handle >= seq->mStartEntityHandle) &&
         (entity_handle < seq->mStartEntityHandle + seq->mNumEntities))
    {
      sequence = seq;
      ret_val = MDB_SUCCESS; 
    }
    else
      ret_val = MDB_ENTITY_NOT_FOUND;
  }

  return ret_val;
}

#ifdef ENTITY_SEQUENCE_TEST

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <exodusII.h>

int main()
{
  MDBEntityHandle start_ids[4] = {0};
  start_ids[1] = static_cast<MDBEntityHandle>((rand()/(RAND_MAX+1.0))*1000 + 100);
  start_ids[2] = static_cast<MDBEntityHandle>((rand()/(RAND_MAX+1.0))*1000+start_ids[1]);
  start_ids[3] = static_cast<MDBEntityHandle>(start_ids[2]+100);
  std::cout << "start ids " << start_ids[0] << " " << start_ids[1] << " " << start_ids[2] << std::endl;

  MDBEntitySequence* e_seqs[3];
  e_seqs[0] = new MDBEntitySequence(start_ids[0], start_ids[1]-start_ids[0]);
  e_seqs[1] = new MDBEntitySequence(start_ids[1], start_ids[2]-start_ids[1]);
  e_seqs[2] = new MDBEntitySequence(start_ids[2], start_ids[3]-start_ids[2]);

  TagHandle tags[5] = {0,1,2,3,4};
  int junk_data = 100;
  int tmp;

  for(int i=0; i<1000; i++)
  { 
    junk_data = tmp = rand();
    MDBEntityHandle ent_hndl = static_cast<MDBEntityHandle>((rand()/(RAND_MAX+1.0))*(start_ids[1]-start_ids[0]));
    e_seqs[0]->set_tag_data(ent_hndl, tags[1],&junk_data, 4);  
    junk_data = rand();
    e_seqs[0]->get_tag_data(ent_hndl, tags[1],&junk_data);  
    assert(tmp == junk_data);
  }  

  delete e_seqs[0];
  delete e_seqs[1];
  delete e_seqs[2];


  char filename[100];

  std::cout << "Enter filename: ";
  //std::cin >> filename;
  strcpy(filename, "e:\\verde\\test\\high_order_elems\\hex.gen");

  float exodus_version;
  int CPU_WORD_SIZE = sizeof(double);  // With ExodusII version 2, all floats
  int IO_WORD_SIZE = sizeof(double);   // should be changed to doubles

  int exodusFile = ex_open ( filename, EX_READ, &CPU_WORD_SIZE,
                         &IO_WORD_SIZE, &exodus_version );

  if(exodusFile == -1)
  {
    std::cout << "couldn't open file " << filename << std::endl;
    return 1;
  }

  int numberDimensions = 0;
  int numberNodes_loading = 0;
  int numberElements_loading = 0;
  int numberElementBlocks_loading = 0; 
  int numberNodeSets_loading = 0;
  int numberSideSets_loading = 0;


  char title[MAX_LINE_LENGTH+1];
  int error = ex_get_init ( exodusFile,
                            title,
                            &numberDimensions,
                            &numberNodes_loading,
                            &numberElements_loading,
                            &numberElementBlocks_loading,
                            &numberNodeSets_loading,
                            &numberSideSets_loading);


  // create a sequence to hold the node coordinates
  MDBEntitySequence* nodes = new MDBEntitySequence(0, numberNodes_loading);
  double *x_array=0;
  double *y_array=0;
  double *z_array=0;
  
  nodes->get_tag_array_ptr(reinterpret_cast<void**>(&x_array), sizeof(double), 0);
  nodes->get_tag_array_ptr(reinterpret_cast<void**>(&y_array), sizeof(double), 1);
  nodes->get_tag_array_ptr(reinterpret_cast<void**>(&z_array), sizeof(double), 2);

  // read in the coordinates
  error = ex_get_coord( exodusFile, x_array, y_array, z_array );
  std::vector<int> block_ids(numberElementBlocks_loading);
  error = ex_get_elem_blk_ids(exodusFile, &block_ids[0]);

  std::vector<MDBEntitySequence*> blocks;
  int elementid = 0;

  for(int num_blocks = 0; num_blocks < block_ids.size(); num_blocks++)
  {
     int block_handle = block_ids[num_blocks];
     int num_elements, num_nodes_per_element, num_attribs;
     char element_type[MAX_STR_LENGTH+1];

     error = ex_get_elem_block ( exodusFile,
                                      block_handle,
                                      element_type,
                                      &num_elements,
                                      &num_nodes_per_element,
                                      &num_attribs );

     
     blocks.push_back( new MDBEntitySequence( elementid, num_elements ) );
     int* conn = 0;
     blocks[num_blocks]->get_tag_array_ptr(reinterpret_cast<void**>(&conn), sizeof(int)*num_nodes_per_element, 0);
     error = ex_get_elem_conn ( exodusFile,
                                 block_handle,
                                 conn);
     

     elementid += num_elements;
  }

  
  int num_nodes;
  for(num_nodes = 0; num_nodes < numberNodes_loading; num_nodes++)
  {
    double x,y,z;
    nodes->get_tag_data(num_nodes, 0, &x);
    nodes->get_tag_data(num_nodes, 1, &y);
    nodes->get_tag_data(num_nodes, 2, &z);

    std::cout << "node " << num_nodes << " x=" << x << " y=" << y << " z=" << z << std::endl;

  }

  int entity_handle = 0;
  for(int j=0; j<blocks.size(); j++)
  {
    MDBEntitySequence *seq_pointer = blocks[j];

    int number_elements = seq_pointer->get_num_entities();
  
      int conn_array[8] = {0};

    for(int k=0; k<number_elements; k++)
    {
      seq_pointer->get_tag_data(k+entity_handle,0,conn_array);
      std::cout << "conn is for element " << k+entity_handle << " is ";
      for(int kk=0; kk<8; kk++)
        std::cout<<" "<<conn_array[kk];
      std::cout<<std::endl;
      
    }

      entity_handle += number_elements;
  }


  
  ex_close(exodusFile);

    
  return 0;
}

#endif

