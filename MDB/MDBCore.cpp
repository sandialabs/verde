
#ifdef WIN32
#ifdef _DEBUG
// turn off warnings that say they debugging identifier has been truncated
// this warning comes up when using some STL containers
#pragma warning(disable : 4786)
#endif
#endif

#include <fstream>
#include <vector>
#include "MDBImplGeneral.hpp"
#include "TagServer.hpp"
#include "MDBEntitySequence.hpp"
#include "ReadWriteExoII.hpp"


//! Constructor
MDBImplGeneral::MDBImplGeneral() 
{
}

  //! destructor

MDBImplGeneral::~MDBImplGeneral()
{
}

//! load mesh from data in file
//! NOTE: if there is mesh already present, the new mesh will be added
MDBErrorCode  MDBImplGeneral::load_mesh(std::string &file_name)
{
  // see if the file exists

  std::ifstream in_file(file_name.c_str());
  if (!in_file)
  {
    return MDB_FILE_DOES_NOT_EXIST;
  }
  else
  {
    in_file.close();
  }

  // see if the file can be read by exodus
  ReadWriteExoII reader(this);
  MDBErrorCode error = reader.load_file(file_name.c_str(), NULL);

  return error; 
}

//! deletes all mesh entities from this datastore
MDBErrorCode MDBImplGeneral::delete_mesh()
{
  return MDB_NOT_IMPLEMENTED; 
}

//! get the coordinate information for this handle if it is of type Vertex
//! otherwise, return an error
MDBErrorCode  MDBImplGeneral::get_coords(MDBEntityHandle entity_handle, double *coords)
{
  
  MDBErrorCode status = MDB_FAILURE;

  if ( TYPE_FROM_HANDLE(entity_handle) == MDBVertex )
  {

    MDBEntitySequence* nodes; //KGM
    MDBErrorCode err = MDBEntitySequence::find(entity_handle, nodes);

    if (err != MDB_SUCCESS)
    {
      status = err;
    }
    else 
    {
      // Get x coordinate value
      status=nodes->get_tag_data(entity_handle, xcoord_tag(), &coords[0]); 
      if ( status != MDB_SUCCESS )
        return status;

      // Get y coordinate value
      status=nodes->get_tag_data(entity_handle, ycoord_tag(), &coords[1]); 
      if ( status != MDB_SUCCESS )
        return status;

      // Get z coordinate value
      status=nodes->get_tag_data(entity_handle, zcoord_tag(), &coords[2]); 
      if ( status != MDB_SUCCESS )
        return status;
        
      status = MDB_SUCCESS;
    }
  }

  return status; 
}

//! get the connectivity for element handles.  For non-element handles, return an error
MDBErrorCode  MDBImplGeneral::get_connectivity(MDBEntityHandle entity_handle, 
                               std::vector<MDBEntityHandle> &connectivity)
{
  MDBErrorCode status = MDB_FAILURE;

  // Make sure the entity should have a connectivity.
  // WARNING: This is very dependent on the ordering of the MDBEntityType enum
  MDBEntityType type = TYPE_FROM_HANDLE(entity_handle);

  if (type >= MDBPoint && type <= MDBWedge)
  {
    this->get_adjacencies(entity_handle, 0, connectivity); 

    status = MDB_SUCCESS;
  }

  return status;
}

//! return a pointer to the tag data for a given EntityHandle and TagHandle
MDBErrorCode  MDBImplGeneral::get_tag(MDBEntityHandle entity_handle, TagHandle tag_handle, 
                                 void *tag_data)
{
  MDBErrorCode status = MDB_FAILURE;

  if (tag_handle & TagInfo::TAGPROP_DENSE)
  {
    MDBEntitySequence* seq; //KGM
    status = MDBEntitySequence::find(entity_handle, seq);

    if (status != MDB_SUCCESS) 
      return status;

    status = seq->get_tag_data( entity_handle, tag_handle, tag_data );

    if (status != MDB_SUCCESS) 
      return status;

    status = MDB_SUCCESS;

  }
  else 
  {
    if (tagServer.get_data(tag_handle, entity_handle, tag_data))
      status = MDB_SUCCESS;
  }

  return status;
}

//! set the data  for a given EntityHandle and TagHandle
MDBErrorCode  MDBImplGeneral::set_tag(MDBEntityHandle entity_handle, TagHandle tag_handle, 
                                 void *tag_data, int num_tags)
{
  MDBErrorCode status = MDB_FAILURE;

  if (tag_handle & TagInfo::TAGPROP_DENSE)
  {
    MDBEntitySequence* seq; //KGM
    MDBErrorCode err = MDBEntitySequence::find(entity_handle, seq);

    if (err != MDB_SUCCESS)
      return err;

    if (seq == 0)
      return MDB_ENTITY_NOT_FOUND;

    //! Query the tag and determine data size
    TagInfo tag_info = tagServer.get_tag_info(tag_handle);

    int data_size = tag_info.get_size();

    if (data_size > 0)
    {
      //! Now actually set the data
      seq->set_tag_data(entity_handle, tag_handle, tag_data, num_tags * data_size);

      status = MDB_SUCCESS;
    }
  }
  else 
  {
    if (tagServer.set_data(tag_handle, entity_handle, tag_data))
      status = MDB_SUCCESS;
  }

  return status;
}

//! adds a sparse tag for this specific EntityHandle/tag_name combination
MDBErrorCode  MDBImplGeneral::create_tag(MDBEntityHandle entity_handle, 
                                         const std::string tag_name,
                                         int tag_size, TagHandle &tag_handle, 
                                         void *default_value)
{
  MDBErrorCode status = MDB_FAILURE;

  // Don't know what to do with the default value yet.
  default_value = default_value;

  MDBEntityType type = TYPE_FROM_HANDLE(entity_handle);
  tag_handle = tagServer.add_tag(tag_name, type, tag_size, TagInfo::TAGPROP_SPARSE);

  if (tag_handle)
     status = MDB_SUCCESS;

  return status;
}

//! add a dense tag for this EntityType/tag_name combination
MDBErrorCode MDBImplGeneral::create_tag(MDBEntityType type, const std::string tag_name,
                                 int tag_size, TagHandle &tag_handle, 
                                  void *default_value)
{
  MDBErrorCode status = MDB_FAILURE;

  // Don't know what to do with the default value yet.
  default_value = default_value;

  tag_handle = tagServer.add_tag(tag_name, type, tag_size, TagInfo::TAGPROP_DENSE);

  if (tag_handle)
  {
    status = MDB_SUCCESS;
  }

  return status;
}


//! removes the tag from the entity
MDBErrorCode  MDBImplGeneral::delete_tag(MDBEntityHandle entity_handle, 
                                    TagHandle tag_handle)
{
  MDBErrorCode status = MDB_FAILURE;

  if (tag_handle & TagInfo::TAGPROP_DENSE)
  {
    // Can't delete a single instance of a dense tag.
    ;
  }
  else 
  {
    //! \bug no return value on remove data.
    tagServer.remove_data(tag_handle, entity_handle);
    status = MDB_SUCCESS;
  }

  return status;
}

//! removes the tag from MDB
MDBErrorCode  MDBImplGeneral::delete_tag(TagHandle tag_handle)
{
  MDBErrorCode status = MDB_FAILURE;

  if (tag_handle & TagInfo::TAGPROP_DENSE)
  {
    // find the EntitySequence that stores this type of handle
   unsigned int type = TYPE_FROM_TAG_HANDLE(tag_handle);

    int err;
    MDBEntityHandle handle = CREATE_HANDLE(type, 0, err);

    if (err)
      return MDB_TYPE_OUT_OF_RANGE;

    MDBEntitySequence* seq; //KGM
    status = MDBEntitySequence::find(handle, seq);

    if (status != MDB_SUCCESS)
      return status;

    if (seq)
    {
      seq->remove_tag (tag_handle);
      status =  MDB_SUCCESS;
    }
  }
  else 
  {
    //! \bug no return value on remove tag.
    tagServer.remove_tag(tag_handle);
    status = MDB_SUCCESS;
  }

  return status;
}

//! gets the tag name string for the tag_handle
MDBErrorCode  MDBImplGeneral::get_tag_name(TagHandle tag_handle, std::string& tag_name)
{
  MDBErrorCode status = MDB_FAILURE;

  TagInfo tag_info = tagServer.get_tag_info( tag_handle );
  
  tag_name = tag_info.get_name();

  if ( !tag_name.empty() )
  {
    status = MDB_SUCCESS;
  }

  return status;
}

//! gets tag handle from its name.
//! the type must be specified because the same name is valid for multiple types
MDBErrorCode  MDBImplGeneral::get_tag_handle( const std::string tag_name, 
                                              MDBEntityType type,
                                              TagHandle &tag_handle)
{
  MDBErrorCode status = MDB_FAILURE;

  tag_handle = tagServer.get_handle( tag_name, type );
  
  if (tag_handle)
  {
    status = MDB_SUCCESS;
  }

  return status;
}


MDBErrorCode MDBImplGeneral::create_element(MDBEntityHandle& handle, MDBEntityType type, 
                                    MDBEntityHandle* nodes, int num_nodes)
{

  MDBErrorCode status;
  MDBEntitySequence* ent_sequence = 0;
  status = MDBEntitySequence::get_next_available_handle( type, ent_sequence, handle );

  if (status != MDB_SUCCESS)
    return status;
  
  // make a new entity sequence if necessary
  if( ent_sequence == 0 || handle > ent_sequence->get_last_reserved_handle() )
  {
    ent_sequence = new MDBEntitySequence(handle, 1024, false);
  }

  ent_sequence->set_tag_data(handle, connectivity_tag(), nodes, sizeof(int)*num_nodes );

  return MDB_SUCCESS;
}

MDBErrorCode MDBImplGeneral::get_adjacencies(MDBEntityHandle entity, int dimension, 
    std::vector<MDBEntityHandle>& adjacent_entities)
{

  MDBEntitySequence* ent_sequence = 0;
  adjacent_entities.clear();

  // get the nodes
  // case n to 0
  if(dimension == 0 && (TYPE_FROM_HANDLE(entity) != MDBVertex))
  {
    MDBErrorCode status = MDBEntitySequence::find(entity, ent_sequence);

    if (status != MDB_SUCCESS) //KGM
      return status;
    
    if(ent_sequence == 0)
      // need to return some information
      return MDB_ENTITY_NOT_FOUND;

    int num_nodes = ent_sequence->get_tag_data_size( connectivity_tag() ) / sizeof(int);
    int *nodes = new int[num_nodes];

    adjacent_entities.resize( num_nodes );
    
    status = ent_sequence->get_tag_data( entity, connectivity_tag(), nodes );
    if (status != MDB_SUCCESS)
      return status;

    int err = 0;
    for(int i=0; i<num_nodes; i++)
    {
      adjacent_entities[i] = CREATE_HANDLE( MDBVertex, nodes[i], err );
    }

    delete [] nodes;
    
    return MDB_SUCCESS;
  }

  int entity_dimension = dimension_table[TYPE_FROM_HANDLE(entity)];  

  MDBEntityType entity_type = TYPE_FROM_HANDLE(entity);

  MDBErrorCode err = MDBEntitySequence::find(entity, ent_sequence);

  if (err != MDB_SUCCESS) //KGM
    return err;

  int num_nodes = ent_sequence->get_tag_data_size( connectivity_tag() ) / sizeof(int);
  int *nodes = new int[num_nodes];

  err = ent_sequence->get_tag_data( entity, connectivity_tag(), nodes );
  if (err != MDB_SUCCESS)
      return err;


  // case 3 to 2 -- create faces
  if( entity_dimension == 3 && dimension == 2)
  {
    // build the quads or tris
    if( entity_type == MDBHex )
    {

      // resize the vector to fit 6 quads
      adjacent_entities.resize( 6 );
  
      // make node array
      MDBEntityHandle node_array[4];

      int i = 0;
      for( int j=0; j<6; j++)
      {
        node_array[0] = nodes[hex_quads[i++]];
        node_array[1] = nodes[hex_quads[i++]];
        node_array[2] = nodes[hex_quads[i++]];
        node_array[3] = nodes[hex_quads[i++]];



        create_element( adjacent_entities[j], MDBQuad, node_array, 4);
      }
    }

    // build the quads or tris
    else if( entity_type == MDBTet )
    {

      // resize the vector to fit 4 tris
      adjacent_entities.resize( 4 );
  
      // make node array
      MDBEntityHandle node_array[3];

      int i = 0;
      for( int j=0; j<4; j++)
      {
        node_array[0] = nodes[tet_tris[i++]];
        node_array[1] = nodes[tet_tris[i++]];
        node_array[2] = nodes[tet_tris[i++]];
        
        create_element( adjacent_entities[j], MDBTri, node_array, 3);
      }

    }
  }

  // case 3 to 1 -- create edges
  else if (entity_dimension == 3 && dimension == 1)
  {
    if( entity_type == MDBHex )
    {

      // resize the vector to fit 12 edges
      adjacent_entities.resize( 12 );
  
      // make node array
      MDBEntityHandle node_array[2];

      int i = 0;
      for( int j=0; j<12; j++)
      {
        node_array[0] = nodes[hex_edges[i++]];
        node_array[1] = nodes[hex_edges[i++]];
        
        create_element( adjacent_entities[j], MDBEdge, node_array, 2);
      }
    }

    // build the quads or tris
    else if( entity_type == MDBTet )
    {

      // resize the vector to fit 6 edges
      adjacent_entities.resize( 6 );
  
      // make node array
      MDBEntityHandle node_array[2];

      int i = 0;
      for( int j=0; j<6; j++)
      {
        node_array[0] = nodes[tet_edges[i++]];
        node_array[1] = nodes[tet_edges[i++]];
                
        create_element( adjacent_entities[j], MDBEdge, node_array, 2);
      }
    }
  }
  
  // case 2 to 1 -- create edges
  else if (entity_dimension == 2 && dimension == 1)
  {
    if( entity_type == MDBQuad )
    {

      // resize the vector to fit 4 edges
      adjacent_entities.resize( 4 );
  
      // make node array
      MDBEntityHandle node_array[2];

      int i = 0;
      for( int j=0; j<4; j++)
      {
        node_array[0] = nodes[quad_edges[i++]];
        node_array[1] = nodes[quad_edges[i++]];
        
        create_element( adjacent_entities[j], MDBEdge, node_array, 2);
      }
    }

    // build the quads or tris
    else if( entity_type == MDBTri )
    {

      // resize the vector to fit 3 egdes
      adjacent_entities.resize( 3 );
  
      // make node array
      MDBEntityHandle node_array[2];

      int i = 0;
      for( int j=0; j<3; j++)
      {
        node_array[0] = nodes[tri_edges[i++]];
        node_array[1] = nodes[tri_edges[i++]];
                
        create_element( adjacent_entities[j], MDBEdge, node_array, 2);
      }
    }
  }
  

  delete [] nodes;

  return MDB_SUCCESS;
}




  //! functions to return coordinate tag handles; this version checks for tag
  //! and creates one if missing
TagHandle MDBImplGeneral::xcoord_tag()
{
  if (0 == xcoordTag)
    xcoordTag =
      tagServer.add_tag("xcoord", MDBVertex, sizeof(double), 
                        TagInfo::TAGPROP_DENSE);
  return xcoordTag;
}
TagHandle MDBImplGeneral::ycoord_tag() 
{
  if (0 == ycoordTag)
    ycoordTag =
      tagServer.add_tag("ycoord", MDBVertex, sizeof(double), 
                        TagInfo::TAGPROP_DENSE);
  return ycoordTag;
}
TagHandle MDBImplGeneral::zcoord_tag() 
{
  if (0 == zcoordTag)
    zcoordTag =
      tagServer.add_tag("zcoord", MDBVertex, sizeof(double), 
                        TagInfo::TAGPROP_DENSE);
  return zcoordTag;
}
TagHandle MDBImplGeneral::connectivity_tag() 
{
  if (0 == connectivityTag)
    connectivityTag =
        // hardwire to hex elements (8 nodes) for now...
      tagServer.add_tag("connectivity", MDBRegion, 8*sizeof(int), 
                        TagInfo::TAGPROP_DENSE);
  return connectivityTag;
}

TagHandle MDBImplGeneral::materialBC_tag()
{
  if (0 == materialBCTag)
    materialBCTag =
      tagServer.add_tag("materialBC", MDBRegion, sizeof(int), 
                        TagInfo::TAGPROP_SPARSE);
  return materialBCTag;
}

TagHandle MDBImplGeneral::neumannBC_tag()
{
  if (0 == neumannBCTag)
    neumannBCTag =
      tagServer.add_tag("neumannBC", MDBRegion, sizeof(int), 
                        TagInfo::TAGPROP_SPARSE);
  return neumannBCTag;
}

TagHandle MDBImplGeneral::dirichletBC_tag()
{
  if (0 == dirichletBCTag)
    dirichletBCTag =
      tagServer.add_tag("dirichletBC", MDBRegion, sizeof(int), 
                        TagInfo::TAGPROP_SPARSE);
  return dirichletBCTag;
}

TagHandle MDBImplGeneral::distFactor_tag()
{
  if (0 == distFactorTag)
    distFactorTag =
      tagServer.add_tag("distFactor", MDBRegion, sizeof(double*), 
                        TagInfo::TAGPROP_SPARSE);
  return distFactorTag;
}

TagHandle MDBImplGeneral::sideNumber_tag()
{
  if (0 == sideNumberTag)
    sideNumberTag =
      tagServer.add_tag("sideNumber", MDBRegion, sizeof(int*), 
                        TagInfo::TAGPROP_SPARSE);
  return sideNumberTag;
}

TagHandle MDBImplGeneral::exodusId_tag()
{
  if (0 == exodusIdTag)
    exodusIdTag =
      tagServer.add_tag("exodusId", MDBRegion, sizeof(int), 
                        TagInfo::TAGPROP_DENSE);
  return exodusIdTag;
}

TagHandle MDBImplGeneral::globalId_tag()
{
  if (0 == globalIdTag)
    globalIdTag =
      tagServer.add_tag("globalId", MDBRegion, sizeof(int), 
                        TagInfo::TAGPROP_DENSE);
  return globalIdTag;
}
