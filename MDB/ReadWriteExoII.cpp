

#ifdef WIN32
#ifdef _DEBUG
// turn off warnings that say they debugging identifier has been truncated
// this warning comes up when using some STL containers
#pragma warning(disable : 4786)
#endif
#endif



#include "ReadWriteExoII.hpp"

#include <algorithm>
#include <iostream>

#include "TagServer.hpp"
#include "MDBEntitySequence.hpp"
#include "MDB_MeshSet.hpp"
#include "MDBImplGeneral.hpp"
#include "../util/MDBRange.hpp"
#include "string.h"
#include "assert.h"
#include "exodusII.h"

std::set<int> ReadWriteExoII::blockIds;
std::set<int> ReadWriteExoII::nodeSetIds;
std::set<int> ReadWriteExoII::sideSetIds;

int ReadWriteExoII::blockIdOffset = 0;
int ReadWriteExoII::nodesetIdOffset = 0;
int ReadWriteExoII::sidesetIdOffset = 0;

ReadWriteExoII::ReadWriteExoII(MDBImplGeneral *impl) 
    : mdbImpl(impl), exodusFile(-1), numberDimensions(-1)
{
  if (NULL == mdbImpl) 
    mdbImpl = new MDBImplGeneral();
}

ReadWriteExoII::~ReadWriteExoII() 
{
  if (-1 != exodusFile)
    ex_close(exodusFile);
}

MDBErrorCode ReadWriteExoII::load_file(const char *exodus_file_name,
                                       std::deque<int> *active_block_id_list) 
{
    // this function directs the reading of an exoii file, but doesn't do any of
    // the actual work
  
    // 1. Read the header
  MDBErrorCode status = read_exodus_header(exodus_file_name,
                                           active_block_id_list);
  if (MDB_FAILURE == status) return status;
  
    // 2. Read the nodes
  status = read_nodes();
  if (MDB_FAILURE == status) return status;
  
    // 3. Read block headers (blocks aren't actually read until they're needed);
    // blocks in the file and in active_block_id_list are returned in blocks_reading
  std::map<int, MDBEntitySequence*> ids_n_blocks;
  status = read_block_headers(ids_n_blocks, active_block_id_list);
  if (MDB_FAILURE == status) return status;

    // 4. Read elements (might not read them, depending on active blocks)
  status = read_elements(ids_n_blocks);
  if (MDB_FAILURE == status) return status;
  
    // 5. Read global ids
    //status = read_global_ids();
  if (MDB_FAILURE == status) return status;
  
    // 6. Read nodesets
  status = read_nodesets();
  if (MDB_FAILURE == status) return status;
  
    // 7. Read sidesets
  status = read_sidesets();
  if (MDB_FAILURE == status) return status;

    // what about properties???

  return MDB_SUCCESS;
}

MDBErrorCode ReadWriteExoII::read_exodus_header(const char *exodus_file_name,
                                                std::deque<int> *active_block_id_list)
{

  if (-1 == exodusFile) {
    
      // open exodus file
    float exodus_version;
    int CPU_WORD_SIZE = sizeof(double);  // With ExodusII version 2, all floats
    int IO_WORD_SIZE = sizeof(double);   // should be changed to doubles

    exodusFile = ex_open ( exodus_file_name, EX_READ, &CPU_WORD_SIZE,
                           &IO_WORD_SIZE, &exodus_version );
  }
  
  if ( exodusFile == -1 )
  {
    std::cout << "Verde:: problem opening genesis file " << exodus_file_name << std::endl;
    return MDB_FAILURE;
  }

  // read in initial variables
   
  numberDimensions_loading = 0;
  numberNodes_loading = 0;
  numberElements_loading = 0;
  numberElementBlocks_loading = 0; 

  char title[MAX_LINE_LENGTH+1];
  int error = ex_get_init ( exodusFile,
                            title,
                            &numberDimensions,
                            &numberNodes_loading,
                            &numberElements_loading,
                            &numberElementBlocks_loading,
                            &numberNodeSets_loading,
                            &numberSideSets_loading);
  if ( error == -1 )
  {
    std::cout << "Verde:: error reading exodus init variables...\n\n" << std::endl;
    return MDB_FAILURE;
  }

  return MDB_SUCCESS;
}
 
MDBErrorCode ReadWriteExoII::read_nodes()
{

    // read the nodes into memory

  assert(-1 != exodusFile);

  // create a sequence to hold the node coordinates
  int error;
  MDBEntityHandle node_handle = CREATE_HANDLE(MDBVertex, 0, error);
  if (0 != error) {
    std::cout << "Problems creating entity handle for vertices." << std::endl;
    return MDB_FAILURE;
  }
  
  MDBEntitySequence* nodes = new MDBEntitySequence(node_handle, 
                                                   numberNodes_loading);
  double *x_array=0;
  double *y_array=0;
  double *z_array=0;
  
    // get tag array ptrs into which the coordinates are read
  nodes->get_tag_array_ptr(reinterpret_cast<void**>(&x_array), sizeof(double), 
                           mdbImpl->xcoord_tag());
  nodes->get_tag_array_ptr(reinterpret_cast<void**>(&y_array), sizeof(double), 
                           mdbImpl->ycoord_tag());
  nodes->get_tag_array_ptr(reinterpret_cast<void**>(&z_array), sizeof(double), 
                           mdbImpl->zcoord_tag());

  // read in the coordinates
  error = ex_get_coord( exodusFile, x_array, y_array, z_array );

    // if no z coords, fill with 0's
  if (numberDimensions == 2 )
  {
    for (int i = 0; i < numberNodes_loading; i++)
    {
      z_array[i] = 0.0;
    }
  }

  mdbImpl->total_num_nodes(mdbImpl->total_num_nodes() + numberNodes_loading);
  
  return MDB_SUCCESS;
}

MDBErrorCode ReadWriteExoII::read_block_headers(std::map<int,MDBEntitySequence*> &ids_n_blocks,
                                                std::deque<int> *active_block_id_list)
{
     
    // get the element block ids; keep this in a separate list, 
    // which is not offset by blockIdOffset; this list used later for
    // reading block connectivity

  std::vector<int> block_ids(numberElementBlocks_loading);
  int error = ex_get_elem_blk_ids(exodusFile, &block_ids[0]);

  int elementid = 0;

    // if we're only reading active blocks, go through the block list
    // removing inactive ones 
  if (NULL != active_block_id_list)
    remove_inactive_blocks(block_ids, *active_block_id_list);

    // read header information and initialize header-type block information
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

      // get the entity type corresponding to this ExoII element type
    MDBEntityType entity_type;
    get_type(element_type, entity_type);
    MDBEntityHandle elem_handle = CREATE_HANDLE(entity_type, elementid, 
                                                error);
    if (0 != error) {
      std::cout << "Problems creating entity handle for elements in block" 
                << block_handle << "." << std::endl;
      continue;
    }
    
    MDBEntitySequence *new_sequence = 
      new MDBEntitySequence( elem_handle, num_elements );
    
      // create a range for this sequence of elements
    MDBRange<MDBEntityHandle> new_range(elem_handle, 
                                        elem_handle+num_elements-1);
    
      // create a MeshSet for this block and set the materialBC tag
    MDB_MeshSet *new_ms = new MDB_MeshSet(new_range);
    MDBEntityHandle ms_handle = CREATE_HANDLE(MDBMeshset, (int)new_ms, error);
    if (0 != error) {
      std::cout << "Problems creating meshset for block" 
                << block_handle << "." << std::endl;
      continue;
    }
    mdbImpl->tag_server().set_data(mdbImpl->materialBC_tag(),
                                  ms_handle,
                                  &block_handle);
    
      // now put the sequence and block id in our list
    ids_n_blocks.insert( std::pair<int,MDBEntitySequence*>(block_handle, 
                                                           new_sequence));
    elementid += num_elements;
  }

  return MDB_SUCCESS;
}

MDBErrorCode ReadWriteExoII::remove_inactive_blocks(std::vector<int> &blocks_reading,
                                                    std::deque<int> &active_blocks) 
{
    // active_blocks is almost certain to be shorter than blocks_reading
  std::vector<int> dummy_blocks;
  std::deque<int>::iterator
    this_it = active_blocks.begin();
  for (; this_it != active_blocks.end(); this_it++) {
    if (std::find(blocks_reading.begin(), blocks_reading.end(), *this_it)
        != blocks_reading.end())
      dummy_blocks.push_back(*this_it);
  }
  
  blocks_reading.swap(dummy_blocks);

  return MDB_SUCCESS;
}

MDBErrorCode ReadWriteExoII::read_elements(std::map<int,MDBEntitySequence*> &ids_n_blocks) 
{
    // read in elements
  char element_type[MAX_STR_LENGTH+1];
  int number_attributes;

  std::map<int, MDBEntitySequence*>::iterator
    this_it = ids_n_blocks.begin();

  for (; this_it != ids_n_blocks.end(); this_it++) {
      // get some information about this block
    int num_elements, num_nodes_per_element, num_attribs;
    int result = ex_get_elem_block ( exodusFile,
                                     (*this_it).first,
                                     element_type,
                                     &num_elements,
                                     &num_nodes_per_element,
                                     &num_attribs );

    if ( result == -1 )
    {
      std::cout << "ReadWriteExoII:: error reading exodus element block " 
                << (*this_it).first << std::endl;
      return MDB_FAILURE;
    }

      // now get a pointer into the memory for connectivity
    int* conn = 0;
    (*this_it).second->
      get_tag_array_ptr(reinterpret_cast<void**>(&conn), sizeof(int)*num_nodes_per_element, 
                        mdbImpl->connectivity_tag());

      // read the connetivity into that memory
    result = ex_get_elem_conn ( exodusFile,
                                (*this_it).first,
                                conn);
    if ( result == -1 )
    {
      std::cout << "ReadWriteExoII:: error getting element connectivity for block "
                << (*this_it).first << std::endl;
      return MDB_FAILURE;
    }

      // for now, make sure we're only adding one mesh
    //assert(numberNodes_loading == mdbImpl->total_num_nodes());     
    int connectivity_offset = 0;

      // finally, increment the number of elements
    mdbImpl->total_num_elements(numberElements_loading + 
                               mdbImpl->total_num_elements());
  }
   
  return MDB_SUCCESS;
}
  
MDBErrorCode ReadWriteExoII::get_type(char *exo_element_type,
                                      MDBEntityType &entity_type) 
{
  if (NULL != strstr(exo_element_type, "HEX")) {
    entity_type = MDBHex;
  }
  else if (NULL != strstr(exo_element_type, "TET")) {
    entity_type = MDBTet;
  }
  else if (NULL != strstr(exo_element_type, "PYRAMID")) {
    entity_type = MDBPyramid;
  }
  else if (NULL != strstr(exo_element_type, "TRI")) {
    entity_type = MDBTri;
  }
  else if (NULL != strstr(exo_element_type, "QUAD") ||
           NULL != strstr(exo_element_type, "SHELL")) {
    entity_type = MDBQuad;
  }
  else if (NULL != strstr(exo_element_type, "BAR") ||
           NULL != strstr(exo_element_type, "BEAM") ||
           NULL != strstr(exo_element_type, "TRUSS")) {
    entity_type = MDBBar;
  }
  else if (NULL != strstr(exo_element_type, "SPHERE")) {
    entity_type = MDBPoint;
  }
  else {
    entity_type = MDBMaxType;
  }
  
  return (entity_type == MDBMaxType ? MDB_FAILURE :
          MDB_SUCCESS);
}
  
MDBErrorCode ReadWriteExoII::read_global_ids()
{

    // don't read global ids for now
  return MDB_FAILURE;

/*
  int * ptr;
  MDB_Mesh_QueryInterface *mdb = MDB_Mesh_QueryInterface::instance();

    // read in the map from the exodus file
  ptr = new int [numberElements_loading];

  int result = ex_get_elem_num_map(exodusFile, ptr);
  if ( result == -1 )
  {
    return MDB_FAILURE;
  };

    // load these global ids into a tag on the elements
  for (int i = 0; i < numberElements_loading; i++) {
      // get some initial data, then set the tag
    
    mdb->sparseTagServer.set_data(globalIdTagId, i+1, &ptr[i]);
  
  return MDB_SUCCESS;
*/
}

MDBErrorCode ReadWriteExoII::read_nodesets() 
{
    // read in the nodesets for the model

  if(0 == numberNodeSets_loading) return MDB_SUCCESS;
  
  int *id_array = new int[numberNodeSets_loading];

    // read in the nodeset ids
  ex_get_node_set_ids(exodusFile, id_array);

    // create a node set for each one

  int *node_id_array;
  double *factor_array;

    // for now, only treat reading one exoII file
  //assert(numberNodes_loading == mdbImpl->total_num_nodes());

    // use a vector of ints to read node handles
  std::vector<int> node_handles;

  int i;
  for(i = 0; i < numberNodeSets_loading; i++)
  {
      // get nodeset parameters
    int number_nodes_in_set;
    int number_dist_factors_in_set;
    ex_get_node_set_param(exodusFile, id_array[i], &number_nodes_in_set,
                          &number_dist_factors_in_set);

      // size new arrays and get ids and distribution factors
    if (node_handles.size() < number_nodes_in_set) {
      node_handles.reserve(number_nodes_in_set);
      node_handles.resize(number_nodes_in_set);
    }
    
    ex_get_node_set(exodusFile, id_array[i], &node_handles[0]);

      // make these into entity handles
    int j, temp;
    for (j = 0; j < number_nodes_in_set; j++)
      node_handles[j] = CREATE_HANDLE(MDBVertex, node_handles[j], temp);
    
      // create a meshset for this node set
    MDB_MeshSet *node_bc = new MDB_MeshSet();
    node_bc->AddEntities(&node_handles[0], number_nodes_in_set, &temp);
    
    
      // need to new a double array for factors, as this array ptr gets
      // stored in a tag for this nodeset meshset
    factor_array = new double[number_dist_factors_in_set];
    ex_get_node_set_dist_fact(exodusFile, id_array[i],
                              factor_array);

      // set a tag signifying dirichlet bc
    id_array[i] += nodesetIdOffset;
    int nodebc_handle = CREATE_HANDLE(MDBMeshset, node_bc->GetId(&temp), 
                                      temp);
    mdbImpl->tag_server().set_data(mdbImpl->dirichletBC_tag(), nodebc_handle, 
                              &id_array[i]);

      // also set the dist factor tag
    mdbImpl->tag_server().set_data(mdbImpl->distFactor_tag(), nodebc_handle, 
                                  &factor_array);

      // note, we do not delete the factor_array, since this was a shallow
      // copy into the dist factor tag
  }
  delete [] id_array;
  
  return MDB_SUCCESS;
}

MDBErrorCode ReadWriteExoII::read_sidesets() 
{
  // now, read in the sidesets

  // read in the sideset ids

  if(0 == numberSideSets_loading) return MDB_SUCCESS;

  else {
    std::cout << "Cannot currently read sidesets into MDB." << std::endl;
    return MDB_FAILURE;
  }

/*  
  if (-1 == sidesetIdOffset) sidesetIdOffset = get_offset(3);

  int *id_array = new int[numberSideSets];

  ex_get_side_set_ids(exodusFile, id_array);

  // create a side set for each one

  int *side_list;
  int *element_list;
  int number_sides_in_set;
  int number_dist_factors_in_set;
  int offset = 
    (0 == mdb->number_elements()) ? 0 :
    (mdb->number_elements() - numberElements_loading);
  int neumann_tag_id = get_neumannBC_tag_id();
  int sideno_tag_id = get_side_number_tag_id();
  int dist_tag_id = get_dist_factor_tag_id();

  int max_sideset_id = sidesetIdOffset;

  for(i = 0; i < numberSideSets_loading; i++)
  {
    // before loading anything, create a new node set
    MDB_MeshSet *side_bc = new MDB_MeshSet();
    if (NULL == side_bc) {
      delete [] id_array;
      return MDB_FAILURE;
    }

    // get sideset parameters
    ex_get_side_set_param(exodusFile, id_array[i], &number_sides_in_set,
                          &number_dist_factors_in_set);

    // size new arrays and get element and side lists
    side_list = new int[number_sides_in_set];
    element_list = new int[number_sides_in_set];
    ex_get_side_set(exodusFile, id_array[i], element_list, side_list);

    // offset element ids if adding a mesh
    if (0 != offset) {
      for(int j=0; j<number_sides_in_set; j++)
      {
         element_list[j] += offset;
      }
    }
    
    // now get factors

    factor_array = new double[number_dist_factors_in_set];
    ex_get_side_set_dist_fact(exodusFile, id_array[i],
                              factor_array);

      // add the element handles to it
    int temp_info;
    side_bc->AddEntities(element_list, number_sides_in_set, &temp_info);
    
      // set a tag signifying neumann bc
    int temp_sidebc = (int) side_bc;
    id_array[i] += sidesetIdOffset;
    MDB_Mesh_QueryInterface::instance()->SetTag(&temp_sidebc, neumann_tag_id, &id_array[i]);

      // represent the side #'s using a tag for now (until we can represent
      // lower-dimension elements)
    MDB_Mesh_QueryInterface::instance()->SetTag(&temp_sidebc, sideno_tag_id, &side_list);
    
      // also set the dist factor tag
    MDB_Mesh_QueryInterface::instance()->SetTag(&temp_sidebc, dist_tag_id, &factor_array);
    
    sidesetIds.insert(id_array[i]);

      // accumulate the max sideset id
    max_sideset_id = (id_array[i] > max_sideset_id) ? id_array[i] : max_sideset_id;
    
      // note, we do not delete the node_id_array or factor_array;
      // these were shallow-copied into entity list and/or tags
  }

  // delete the id_array
  delete [] id_array;

    // update the max sideset id
  sidesetIdOffset = max_sideset_id;

  return MDB_SUCCESS;
*/
}

int ReadWriteExoII::get_int_tag(const MDB_MeshSet *this_ms,
                                const TagHandle tag_id)
{
    // see if it has this tag
  int temp;
  int ms_handle = CREATE_HANDLE(MDBMeshset, this_ms->GetId(&temp),
                                temp);
  bool result = mdbImpl->tag_server().get_data(tag_id, ms_handle, &temp);

  if (false == result) return -1;

  else return temp;
}
