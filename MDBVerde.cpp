//-------------------------------------------------------------------------
// Filename      : MDBVerde.cpp 
//
// Creator       : Ray Meyers
//
// Date          : 01/03/02
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "MDBVerde.hpp"

#include <algorithm>

int MDB_IDECL MDBVerde::QueryInterface( const MDBuuid& uuid, MDBUnknownInterface** iface)
{
   *iface = 0;
   // if the interface is unknown, just return this
   if (uuid == IDD_MDBUnknown)
   {
      *iface = this;
   }
   // if the interface is MDBVerde just return this
   if (uuid == IDD_MDBVerde)
   {
      *iface = this;
   }
   else
   {
      return 0;
   }

   return 1;
}


/*! boolean operator < for use in sorting the BlockInfo information using
std::list::sort function
*/
bool operator<(const BlockInfo &x, const BlockInfo &y)
{
  return x.starting_id < y.starting_id;
}


MDBVerde::MDBVerde()
{
  numberVertices = 0;
  xCoords = 0;
  yCoords = 0;
  zCoords = 0;
  mLastBlock.block_number = 0;
  mLastBlock.starting_id = -1;
  mLastBlock.number_elements = -1;
  mLastBlock.number_vertices_per_element = 0;
}

MDBVerde::~MDBVerde()
{
  reset();
}

MDBErrorCode MDB_IDECL MDBVerde::load_mesh_vertices(MDBInt num_vertices,
                             MDBDouble *x_coords,
                             MDBDouble *y_coords,
                             MDBDouble *z_coords)
{
  numberVertices = num_vertices;
  xCoords = x_coords;
  yCoords = y_coords;
  zCoords = z_coords;
  return MDB_SUCCESS;
}

MDBErrorCode MDB_IDECL MDBVerde::reset()
{
  // reset the cached block  
  mLastBlock.block_number = 0;
  mLastBlock.starting_id = -1;
  mLastBlock.number_elements = -1;
  mLastBlock.number_vertices_per_element = 0;

  // delete coordinates
  if (numberVertices > 0)
  {
    delete [] xCoords;
    delete [] yCoords;
    delete [] zCoords;
    xCoords = 0;
    yCoords = 0;
    zCoords = 0;
    numberVertices = 0;
  }

  // delete element connectivity

  int num_blocks = connectivityList.size();
  int *conn;
  for(int i = 0; i < num_blocks; i++)
  {
    conn = connectivityList[i];
    delete [] conn;
  }
  connectivityList.clear();
  blockInfo.clear();

  return MDB_SUCCESS;
}

MDBErrorCode MDB_IDECL MDBVerde::load_mesh_entities(MDBInt num_entities,
                                          MDBInt starting_id,
                                          MDBInt vertices_per_entity,
                                          MDBEntityHandle *connectivity,
                                          MDBInt *original_ids)
{
  // create a new BlockInfo and load the information
  BlockInfo info;

  info.number_elements = num_entities;
  info.starting_id = starting_id;
  info.number_vertices_per_element = vertices_per_entity;
  info.block_number = connectivityList.size();
  
  // add this info to the blockInfo list and sort the list
  blockInfo.push_back(info);

  // push the connectivity onto the connectivityList
  connectivityList.push_back(connectivity);

  return MDB_SUCCESS;
}


MDBErrorCode MDB_IDECL MDBVerde::extend_xyz_arrays(MDBInt old_array_size,
                                         MDBInt new_array_size,
                                         MDBDouble* new_x,
                                         MDBDouble* new_y, 
                                         MDBDouble* new_z ) 
{
  numberVertices = old_array_size + new_array_size;

  // new arrays big enough for new xyz coords
  double *x_coords = new double[old_array_size + new_array_size];
  double *y_coords = new double[old_array_size + new_array_size];
  double *z_coords = new double[old_array_size + new_array_size];

  // memcopy over old nodes into new array 
  memcpy( x_coords, xCoords, old_array_size*sizeof(MDBDouble) ); 
  memcpy( y_coords, yCoords, old_array_size*sizeof(MDBDouble) ); 
  memcpy( z_coords, zCoords, old_array_size*sizeof(MDBDouble) ); 

  // memcopy over new nodes into array 
  memcpy( x_coords+old_array_size, new_x, new_array_size*sizeof(MDBDouble) ); 
  memcpy( y_coords+old_array_size, new_y, new_array_size*sizeof(MDBDouble) ); 
  memcpy( z_coords+old_array_size, new_z, new_array_size*sizeof(MDBDouble) ); 

  // delete old arrays 
  delete [] xCoords;
  delete [] yCoords;
  delete [] zCoords;
  xCoords = 0;
  yCoords = 0;
  zCoords = 0;

  // set x,y,zCoords to new array 
  xCoords = x_coords;
  yCoords = y_coords;
  zCoords = z_coords;

  return MDB_SUCCESS;

} 


MDBErrorCode MDB_IDECL MDBVerde::GetCoords(MDBEntityHandle vertex_handle,
                                MDBDouble *coords)
{
   vertex_handle--;
   if (vertex_handle >= numberVertices)
   {
     return MDB_INDEX_OUT_OF_RANGE;
   }

   *coords++ = xCoords[vertex_handle];
   *coords++ = yCoords[vertex_handle];
   *coords =   zCoords[vertex_handle];

   return MDB_SUCCESS;
}


MDBErrorCode MDB_IDECL MDBVerde::GetNumAdjacencies(MDBEntityHandle entity_handle,
                                         MDBEntityType calling_type,
                                         MDBEntityDimension adjacency_dimension,
                                         int *num_entities)
{ 
  // This implementation contains minimum adjacencies for now

  // check for element type doing the calling
  if(calling_type > VERTEX)
  {
    if (adjacency_dimension == DIM_0_ENTITY)
    {
      BlockInfo * currentBlock;

      // get the block in which entity_handle resides 
      currentBlock = find_block(entity_handle);

      // return the number of vertices in this block
      *num_entities =  currentBlock->number_vertices_per_element;
      return MDB_SUCCESS;
    }
  }
  return  MDB_INDEX_OUT_OF_RANGE;
}

MDBErrorCode MDB_IDECL MDBVerde::GetAdjacencies(MDBEntityHandle entity_handle,
                                      MDBEntityType calling_type,
                                      MDBEntityDimension adjacency_dimension,
                                      MDBInt *num_entities,
                                      MDBEntityHandle *adj_entities_ptr)
{
 // check faor element type doing the calling
  if(calling_type > VERTEX)
  {
    if (adjacency_dimension == DIM_0_ENTITY)
    {

      BlockInfo * currentBlock;
      // get the block in which entity_handle resides 
      currentBlock = find_block(entity_handle);

      // find location of connectivity information for this element
      int offset = entity_handle - currentBlock->starting_id;
      int block_num = currentBlock->block_number;
      *num_entities = currentBlock->number_vertices_per_element;
      int *conn_ptr = connectivityList[block_num] + 
                      offset * (*num_entities);

      // return the information in the adj_entities_ptr
      
      for(int i = 0; i < *num_entities; i++)
      {
        *(adj_entities_ptr+i) = *conn_ptr++;
      }
      return MDB_SUCCESS;
    }
  }
  return  MDB_INDEX_OUT_OF_RANGE;
} 

BlockInfo* MDBVerde::find_block( MDBEntityHandle element_handle )
{
  // last block = NULL;
  
  // mLastBlock is a cache, check first for a hit...
  
  if( (element_handle >= mLastBlock.starting_id) && 
      (element_handle <  mLastBlock.starting_id + 
                         mLastBlock.number_elements  ) )
  {
    return &( mLastBlock);
  }  

  std::vector<BlockInfo>::iterator block_location;

  for(block_location = blockInfo.begin(); 
      block_location != blockInfo.end(); ++block_location)
  {
    mLastBlock= *block_location;
    
    if( (element_handle >= mLastBlock.starting_id) && 
        (element_handle <  mLastBlock.starting_id + 
                         mLastBlock.number_elements  ) )
    {
      return &( mLastBlock);
    }  
  }
  


  return NULL;
}

