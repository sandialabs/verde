#ifndef MDB_IMPL_GENERAL_HPP
#define MDB_IMPL_GENERAL_HPP

#include "MDBInterface.hpp"
#include "MDBDefines.h"
#include "TagServer.hpp"
       
class MDBImplGeneral : public MDBInterface 
{

public:

  //!constructor
  MDBImplGeneral();

  //!destructor
  ~MDBImplGeneral();

  //! load mesh from data in file
  //! NOTE: if there is mesh already present, the new mesh will be added
  virtual MDBErrorCode  load_mesh(std::string &file_name);

  //! deletes all mesh entities from this datastore
  virtual MDBErrorCode delete_mesh();

  //! get the coordinate information for this handle if it is of type Vertex
  //! otherwise, return an error
  virtual MDBErrorCode  get_coords(MDBEntityHandle entity_handle, double *coords);

  //! get the connectivity for element handles.  For non-element handles, return an error
  virtual MDBErrorCode  get_connectivity(MDBEntityHandle entity_handle, 
                                 std::vector<MDBEntityHandle> &connectivity);

  //! return a pointer to the tag data for a given EntityHandle and TagHandle
  virtual MDBErrorCode  get_tag(MDBEntityHandle entity_handle, TagHandle tag_handle, 
                                   void *tag_data);

  //! set the data  for a given EntityHandle and TagHandle
  virtual MDBErrorCode  set_tag(MDBEntityHandle entity_handle, TagHandle tag_handle, 
                                   void *tag_data, int num_tags=1);

  //! adds a sparse tag for this specific EntityHandle/tag_name combination
  virtual MDBErrorCode  create_tag(MDBEntityHandle entity_handle, 
                                   const std::string tag_name,
                                   int tag_size, TagHandle &tag_handle, 
                                    void *default_value);

  //! add a dense tag for this EntityType/tag_name combination
  virtual MDBErrorCode create_tag(MDBEntityType type, const std::string tag_name,
                                   int tag_size, TagHandle &tag_handle, 
                                    void *default_value);


  //! removes the tag from the data
  virtual MDBErrorCode  delete_tag(MDBEntityHandle entity_handle, 
                                      TagHandle tag_handle);

  //! removes the tag from the database
  virtual MDBErrorCode  delete_tag(TagHandle tag_handle);

  //! gets the tag name string for the tag_handle
  virtual MDBErrorCode  get_tag_name(TagHandle tag_handle, std::string& tag_name);

  //! gets tag handle from its name.
  //! the type must be specified because the same name is valid for multiple types
  virtual MDBErrorCode  get_tag_handle(const std::string tag_name, 
                                       MDBEntityType type,
                                       TagHandle &tag_handle);

  //! for the handle, retrieves handles to the adjacencies of the desired dimension
  virtual MDBErrorCode get_adjacencies(MDBEntityHandle handle, int adjacency_dimension,
                                       std::vector<MDBEntityHandle> &adjacenccies);

  
  virtual MDBErrorCode create_element(MDBEntityHandle& handle, MDBEntityType type, 
    MDBEntityHandle* nodes, int num_nodes);


    //! functions to return coordinate tag handles; this version checks for tag
    //! and creates one if missing
  TagHandle xcoord_tag();
  TagHandle ycoord_tag();
  TagHandle zcoord_tag();

    //! function to return connectivity tag handle
  TagHandle connectivity_tag();

    //! return various specific tag handles
  TagHandle materialBC_tag();
  TagHandle neumannBC_tag();
  TagHandle dirichletBC_tag();
  TagHandle distFactor_tag();
  TagHandle sideNumber_tag();
  TagHandle exodusId_tag();
  TagHandle globalId_tag();

    //! get/set the number of nodes
  int total_num_nodes() const;
  void total_num_nodes(const int val);
  
    //! get/set the number of elements
  int total_num_elements() const;
  void total_num_elements(const int val);

    //! return a reference to the tag server
  TagServer &tag_server() {return tagServer;}
  
private:

    //! store the total number of elements defined in this interface
  int totalNumElements;
  
    //! store the total number of nodes defined in this interface
  int totalNumNodes;

    //! coordinate tags
  TagHandle xcoordTag, ycoordTag, zcoordTag;
  
    //! various types of element connectivity tags
  TagHandle connectivityTag;

  TagHandle materialBCTag;
  TagHandle neumannBCTag;
  TagHandle dirichletBCTag;
  TagHandle distFactorTag;
  TagHandle sideNumberTag;
  TagHandle exodusIdTag;
  TagHandle globalIdTag;

    //! tag server for this interface
  TagServer tagServer;

};

inline int MDBImplGeneral::total_num_nodes() const 
{
  return totalNumNodes;
}

inline int MDBImplGeneral::total_num_elements() const 
{
  return totalNumElements;
}

inline void MDBImplGeneral::total_num_nodes(int num) 
{
  totalNumNodes = num;
}

inline void MDBImplGeneral::total_num_elements(int num) 
{
  totalNumElements = num;
}



#endif   // MDB_IMPL_GENERAL_HPP




