#ifndef MDB_INTERFACE_HPP
#define MDB_INTERFACE_HPP

#include "MDBDefines.h"
#include <vector>
#include <string>
       
class MDBInterface {

public:

  //!constructor
  MDBInterface() {}

  //!destructor
  virtual ~MDBInterface() {}

  //! load mesh from data in file
  //! NOTE: if there is mesh already present, the new mesh will be added
  virtual MDBErrorCode  load_mesh(std::string &file_name)=0;

  //! deletes all mesh entities from this datastore
  virtual MDBErrorCode delete_mesh()=0;


  //! get the coordinate information for this handle if it is of type Vertex
  //! otherwise, return an error
  virtual MDBErrorCode  get_coords(MDBEntityHandle entity_handle, double *coords)=0;

  //! get the connectivity for element handles.  For non-element handles, return an error
  virtual MDBErrorCode  get_connectivity(MDBEntityHandle entity_handle, 
                                 std::vector<MDBEntityHandle> &connectivity)=0;

  //! return a pointer to the tag data for a given EntityHandle and TagHandle
  virtual MDBErrorCode  get_tag(MDBEntityHandle entity_handle, TagHandle tag_handle, 
                                   void *tag_data)=0;

  //! set the data  for a given EntityHandle and TagHandle
  virtual MDBErrorCode  set_tag(MDBEntityHandle entity_handle, TagHandle tag_handle, 
                                   void *tag_data, int num_tags=1 )=0;

  //! adds a sparse tag for this specific EntityHandle/tag_name combination
  virtual MDBErrorCode  create_tag(MDBEntityHandle entity_handle, 
                                   const std::string tag_name,
                                   int tag_size, TagHandle &tag_handle, 
                                    void *default_value)=0;

  //! add a dense tag for this EntityType/tag_name combination
  virtual MDBErrorCode create_tag(MDBEntityType type, const std::string tag_name,
                                   int tag_size, TagHandle &tag_handle, 
                                    void *default_value)=0;


  //! removes the tag from the data
  virtual MDBErrorCode  delete_tag(MDBEntityHandle entity_handle, 
                                      TagHandle tag_handle)=0;

  //! removes the tag from the database
  virtual MDBErrorCode  delete_tag(TagHandle tag_handle)=0;

  //! gets the tag name string for the tag_handle
  virtual MDBErrorCode  get_tag_name(TagHandle tag_handle, std::string& tag_name)=0;

  //! gets tag handle from its name.
  //! the type must be specified because the same name is valid for multiple types
  virtual MDBErrorCode  get_tag_handle(const std::string tag_name, 
                                       MDBEntityType type,
                                       TagHandle &tag_handle)=0;

  //! for the handle, retrieves handles to the adjacencies of the desired dimension
  virtual MDBErrorCode get_adjacencies(MDBEntityHandle handle, int adjacency_dimension,
                                       std::vector<MDBEntityHandle> &adjacenccies) = 0;

};


#endif   // MDB_INTERFACE_HPP




