
/*!
MDBTest.cpp
Test Harness for MDB mesh database system 
*/

#include <iostream>
#include "MDBImplGeneral.hpp"
#include "MDBEntitySequence.hpp" // should we really access this?

using std::cout;


/*!
prints out a result string based on the value of error_code
*/
void handle_error_code(MDBErrorCode error_code,
                       int &number_failed,
                       int &number_not_implemented,
                       int &number_successful)
{
  if (error_code == MDB_SUCCESS)
  {
    cout << "Success";
    number_successful++;
  }
  else if (error_code == MDB_NOT_IMPLEMENTED)
  {
    cout << "Not Implemented";    
    number_not_implemented++;
  }
  else if (error_code == MDB_INDEX_OUT_OF_RANGE)
  {
    cout << "Failure:  Index Out of Range ";  
    number_failed++;
  }
  else if (error_code == MDB_MEMORY_ALLOCATION_FAILED)
  {
    cout << "Failure:  Memory Allocation Failed ";   
    number_failed++;
  }
  else if (error_code == MDB_FAILURE)
  {
    cout << "Failure ";   
    number_failed++;
  }
}


/*!
@test 
Load Mesh
@li Load an Exodus II file
*/
MDBErrorCode mdb_load_mesh_test(MDBInterface *MDB)
{
  std::string file_name = "mdbtest.g";
  MDBErrorCode error = MDB->load_mesh(file_name);

  return error;
}


/*!
@test 
Vertex Coordinates
@li Get coordinates of vertex 1 correctly
@li Get coordinates of vertex 8 correctly
@li Get coordinates of vertex 6 correctly
*/

MDBErrorCode mdb_vertex_coordinate_test(MDBImplGeneral *MDB)
{
  double coords[3];
  MDBEntityHandle handle;
  MDBErrorCode error;
  int err;

  // coordinate 1 should be {3,-1,1}
  
  handle = CREATE_HANDLE(MDBVertex, 1, err);
  error = MDB->get_coords(handle, static_cast<double*>(coords) );
  if (error != MDB_SUCCESS)
    return error;

  if(coords[0] != 3.0 || coords[1] != -1.0 || coords[2] != 1.0)
    return MDB_FAILURE;

    // coordinate 8 should be {2,1,-1}
  handle = CREATE_HANDLE(MDBVertex, 8, err);
  error = MDB->get_coords(handle, static_cast<double*>(coords) );
  if (error != MDB_SUCCESS)
    return error;

  if(coords[0] != 2.0 || coords[1] != 1.0 || coords[2] != -1.0)
    return MDB_FAILURE;

  // coordinate 6 should be {5,-1,-1}
  handle = CREATE_HANDLE(MDBVertex, 6, err);
  error = MDB->get_coords(handle, coords);
  if (error != MDB_SUCCESS)
    return error;
  
  if(coords[0] != 5.0 || coords[1] != -1.0 || coords[2] != -1.0)
    return MDB_FAILURE;

  // Walk the entire list.  Stop when the error code indicates
  // failure (testing walking off the end of the list).  Count
  // the items and ensure a proper count.
  handle = CREATE_HANDLE(MDBVertex, 0, err);

  MDBEntityHandle i;
  int node_count = 0;
  for ( i = handle; error == MDB_SUCCESS; i++)
  {
    error = MDB->get_coords(i, static_cast<double*>(coords) );
    if (error == MDB_SUCCESS)
      node_count++;
  }

  // Number of vertices should node_countbe 83 assuming no gaps in the handle space
   if ( node_count != 83 )
    return MDB_FAILURE;

  // Try getting coordinates for a hex (should fail)
  handle = CREATE_HANDLE(MDBHex, 0, err);
  error = MDB->get_coords(handle, coords);
  if (error == MDB_SUCCESS)
    return MDB_FAILURE;

  return MDB_SUCCESS;
}

/*!
@test
MDB Vertex Tag Test
@li Add, Set and correctly get an int tag
@li Add, Set and correctly get a boolean tag
@li Add, Set and correctly get a double tag
@li Add, Set and correctly get a struct tag
*/

MDBErrorCode mdb_vertex_tag_test(MDBImplGeneral *MDB)
{
  // Add an int Vertex Tag to the database

  int tag_size = sizeof(int);
  TagHandle tag_id;

  // Create a dense tag for all vertices
  MDBErrorCode error = MDB->create_tag(MDBVertex, "int_tag", tag_size, tag_id, 0);
  if (error != MDB_SUCCESS)
    return error;

  // put a value in vertex 1 and retrieve

  int err;
  MDBEntityHandle handle = CREATE_HANDLE(MDBVertex, 1, err);
  int input_value = 11;
  MDB->set_tag(handle, tag_id, &input_value);
  int output_value;
  MDB->get_tag(handle, tag_id, &output_value);
  if(output_value != input_value)
    return MDB_FAILURE;

  // put a value in vertex 5 and retrieve

  handle = CREATE_HANDLE(MDBVertex, 5, err);
  input_value = 11;
  MDB->set_tag(handle, tag_id, &input_value);
  MDB->get_tag(handle, tag_id, &output_value);
  if(output_value != input_value)
    return MDB_FAILURE;

  // put a value in vertex 100 which doesn't exist

  handle = CREATE_HANDLE(MDBVertex, 100, err);
  input_value = 11;

  error = MDB->set_tag(handle, tag_id, &input_value);
  if (error == MDB_SUCCESS)
    return MDB_FAILURE;

  error = MDB->get_tag(handle, tag_id, &output_value);
  if (error == MDB_SUCCESS)
    return MDB_FAILURE;

  if(output_value != input_value)
    return MDB_FAILURE;

  // Add a bool Vertex Tag to the database

  tag_size = sizeof(bool);
  error = MDB->create_tag(MDBVertex, "bool_tag", tag_size, tag_id, 0);
  if (error != MDB_SUCCESS)
    return error;

  // put a value in vertex 5 and retrieve

  handle = CREATE_HANDLE(MDBVertex, 5, err);
  bool bool_input_value = true;
  bool bool_output_value = false;
  MDB->set_tag(handle, tag_id, &bool_input_value);
  MDB->get_tag(handle, tag_id, &bool_output_value);
  if(bool_output_value != bool_input_value)
    return MDB_FAILURE;

  // Add a double Vertex Tag to the database

  tag_size = sizeof(double);
  error = MDB->create_tag(MDBVertex, "double_tag", tag_size, tag_id, 0);
  if (error != MDB_SUCCESS)
    return error;

  // put a value in vertex 8: and retrieve

  handle = CREATE_HANDLE(MDBVertex, 8, err);
  double double_input_value = true;
  double double_output_value = false;
  MDB->set_tag(handle, tag_id,&double_input_value);
  MDB->get_tag(handle, tag_id,&double_output_value);
  if(double_output_value != double_input_value)
    return MDB_FAILURE;

  // Add a struct Vertex Tag to the database

  struct TagStruct {
    int test_int;
    double test_double;
  };
  tag_size = sizeof(TagStruct);
  error = MDB->create_tag(MDBVertex, "struct_tag", tag_size, tag_id, 0);
  if (error != MDB_SUCCESS)
    return error;

  // put a value in vertex 7 and retrieve

  handle = CREATE_HANDLE(MDBVertex, 7, err);
  TagStruct input_tag_struct;
  input_tag_struct.test_int = 55;
  input_tag_struct.test_double = -1.2345;
  TagStruct output_tag_struct;
  MDB->set_tag(handle, tag_id, &input_tag_struct);
  MDB->get_tag(handle, tag_id, &output_tag_struct);
  if(output_tag_struct.test_int != input_tag_struct.test_int ||
     output_tag_struct.test_double != input_tag_struct.test_double)
    return MDB_FAILURE;

  
  // Create sparse tags for 10 random entities including some outside the 
  // range of allowable entities.

  unsigned int node_ids[] = {0, 93, 5, 97, 8, 100, 11, 84, 1, 0x00FFFFFF}; 

  error = MDB->create_tag(handle, "sparse_int_tag", tag_size, tag_id, 0);

  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  int i;
  for (i=0; i<10; i++)
  {
    int err;
    MDBEntityHandle handle = CREATE_HANDLE(MDBVertex, node_ids[i], err);

    if (err)
      return MDB_FAILURE;
  
    int input_value = 11;
    error = MDB->set_tag(handle, tag_id, &input_value);

    // should fail on odd values of i
    if ( !(i % 2) )
    {
      if (error != MDB_SUCCESS )  // even case and if failed
        return MDB_FAILURE;
    }
    else 
    {
      if ( error == MDB_SUCCESS)  // odd case and it says it worked!
        return MDB_FAILURE;
    }

    int output_value;
    error = MDB->get_tag(handle, tag_id, &output_value);
    if ( (i % 2) && error != MDB_FAILURE)
        return error;

    if( (i % 2) && output_value != input_value)
      return MDB_FAILURE;
  }

  // get the tag_name of the last tag created above
  std::string int_tag_name;
  error = MDB->get_tag_name (tag_id, int_tag_name);
  if (error != MDB_SUCCESS)
      return MDB_FAILURE;

  if (int_tag_name != "sparse_int_tag")
      return MDB_FAILURE;

  // get the tag handle of the last tag created above
  TagHandle int_tag_handle;
  error = MDB->get_tag_handle (int_tag_name, MDBVertex, int_tag_handle);
  if (error != MDB_SUCCESS)
      return MDB_FAILURE;
  
  if (int_tag_handle != tag_id)
      return MDB_FAILURE;

  // delete tags test

  // delete 2 of the sparse tags that were created above.
  handle = CREATE_HANDLE(MDBVertex, node_ids[2], err);
  error = MDB->delete_tag(tag_id, handle);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  handle = CREATE_HANDLE(MDBVertex, node_ids[6], err);
  error = MDB->delete_tag(tag_id, handle);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;
  
  // delete all the rest of the sparse tags.

  error = MDB->delete_tag(tag_id);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  // delete the dense tag named bool_tag 
  TagHandle bool_tag_handle;
  error = MDB->get_tag_handle ("bool_tag", MDBVertex, bool_tag_handle);
  if (error != MDB_SUCCESS)

  error = MDB->delete_tag(bool_tag_handle);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  return MDB_SUCCESS;
}


/*!
@test
MDB Bar Element Connectivity Test
@li Get coordinates for 2 node bar elements
*/

MDBErrorCode mdb_bar_connectivity_test(MDBInterface *MDB)
{

  std::vector<MDBEntityHandle> conn;

  MDBEntityHandle handle;
  MDBErrorCode error = MDBEntitySequence::get_first(MDBBar, handle);

  if (error != MDB_SUCCESS)
    return error;

  // get the connectivity of the second bar
  handle++;

  error = MDB->get_connectivity(handle, conn);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  if (conn.size() != 2)
    return MDB_FAILURE;

  // from ncdump the connectivity of bar 1 (0 based) is
  //  29, 10

  if (conn[0] != 29)
    return MDB_FAILURE;

  if (conn[1] != 10)  
    return MDB_FAILURE;

  return MDB_SUCCESS;
}

MDBErrorCode mdb_tri_connectivity_test(MDBInterface *MDB)
{

  std::vector<MDBEntityHandle> conn;

  MDBEntityHandle handle;
  MDBErrorCode error = MDBEntitySequence::get_first(MDBTri, handle);

  if (error != MDB_SUCCESS)
    return error;

  // get the connectivity of the second tri
  handle++;

  error = MDB->get_connectivity(handle, conn);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  if (conn.size() != 3)
    return MDB_FAILURE;

  // from ncdump the connectivity of hex 1 (0 based) is
  //  17, 63, 1

  if (conn[0] != 17)
    return MDB_FAILURE;

  if (conn[1] != 63)  
    return MDB_FAILURE;

  if (conn[2] !=  1) 
    return MDB_FAILURE;

  return MDB_SUCCESS;
}

MDBErrorCode mdb_quad_connectivity_test(MDBInterface *MDB)
{

  std::vector<MDBEntityHandle> conn;

  MDBEntityHandle handle;
  MDBErrorCode error = MDBEntitySequence::get_first(MDBQuad, handle);

  if (error != MDB_SUCCESS)
    return error;

  // get the connectivity of the second quad
  handle++;

  error = MDB->get_connectivity(handle, conn);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  if (conn.size() != 4)
    return MDB_FAILURE;

  // from ncdump the connectivity of hex 1 (0 based) is
  // 13, 33, 71, 39,

  if (conn[0] != 13)
    return MDB_FAILURE;

  if (conn[1] != 33)  
    return MDB_FAILURE;

  if (conn[2] != 71) 
    return MDB_FAILURE;

  if (conn[3] != 39)
    return MDB_FAILURE;

  return MDB_SUCCESS;
}

MDBErrorCode mdb_hex_connectivity_test(MDBInterface *MDB)
{

  std::vector<MDBEntityHandle> conn;

  MDBEntityHandle handle;
  MDBErrorCode error = MDBEntitySequence::get_first(MDBHex, handle);

  if (error != MDB_SUCCESS)
    return error;

  // get the connectivity of the second hex
  handle++;

  error = MDB->get_connectivity(handle, conn);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  if (conn.size() != 8)
    return MDB_FAILURE;

  // from ncdump the connectivity of hex 1 (0 based) is
  //69, 36, 11, 31, 83, 72, 30, 67,

  if (conn[0] != 69)
    return MDB_FAILURE;

  if (conn[1] != 36)  
    return MDB_FAILURE;

  if (conn[2] != 11) 
    return MDB_FAILURE;

  if (conn[3] != 31)
    return MDB_FAILURE;

  if (conn[4] != 83)
    return MDB_FAILURE;

  if (conn[5] != 72)  
    return MDB_FAILURE;

  if (conn[6] != 30) 
    return MDB_FAILURE;

  if (conn[7] != 67)
    return MDB_FAILURE;

  return MDB_SUCCESS;
}

MDBErrorCode mdb_tet_connectivity_test(MDBInterface *MDB)
{

  std::vector<MDBEntityHandle> conn;

  MDBEntityHandle handle;
  MDBErrorCode error = MDBEntitySequence::get_first(MDBTet, handle);

  if (error != MDB_SUCCESS)
    return error;

  // get the connectivity of the second tet
  handle++;

  error = MDB->get_connectivity(handle, conn);
  if (error != MDB_SUCCESS )  
    return MDB_FAILURE;

  if (conn.size() != 4)
    return MDB_FAILURE;

  // from ncdump the connectivity of text 1 (0 based) is 41, 17, 2, 64,

  if (conn[0] != 41)
    return MDB_FAILURE;

  if (conn[1] != 17)  
    return MDB_FAILURE;

  if (conn[2] != 2 ) 
    return MDB_FAILURE;

  if (conn[3] != 64)
    return MDB_FAILURE;

  return MDB_SUCCESS;
}

/*!
main routine for test harness 
*/

int main()
{

  MDBErrorCode result;
  int number_tests = 0;
  int number_tests_successful = 0;
  int number_tests_not_implemented = 0;
  int number_tests_failed = 0;

  // init MDB interface

  MDBImplGeneral *MDB = new MDBImplGeneral();

  // Print out Header information

  cout << "\n\nMDB TEST PROGRAM:\n\n";

  // load_mesh test


  cout << "   mdb_load_mesh: ";
  result = mdb_load_mesh_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // nodal coordinates test

  // element connectivity test

  // MeshSet tests

  // sparse tag test (node, element, MeshSet, Mesh, ??)

  // dense tag test

 


  // MDB vertex coordinate test

  cout << "   mdb_vertex_coordinate_test: ";
  result = mdb_vertex_coordinate_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // MDB vertex tag test
  cout << "   mdb_vertex_tag_test: ";
  result = mdb_vertex_tag_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // MDB bar connectivity
  cout << "   mdb_bar_connectivity_test: ";
  result = mdb_bar_connectivity_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // MDB tri connectivity
  cout << "   mdb_tri_connectivity_test: ";
  result = mdb_tri_connectivity_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // MDB quad connectivity
  cout << "   mdb_quad_connectivity_test: ";
  result = mdb_quad_connectivity_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // MDB tet connectivity
  cout << "   mdb_tet_connectivity_test: ";
  result = mdb_tet_connectivity_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // MDB hex connectivity
  cout << "   mdb_hex_connectivity_test: ";
  result = mdb_hex_connectivity_test(MDB);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  cout << "\n";

  // summary

  cout << "\nMDB TEST SUMMARY: \n"
       << "   Number Tests:           " << number_tests << "\n"
       << "   Number Successful:      " << number_tests_successful << "\n"
       << "   Number Not Implemented: " << number_tests_not_implemented << "\n"
       << "   Number Failed:          " << number_tests_failed 
       << "\n\n";

  return 0;
}



/* old stuff

#include <iostream>
#include "MDB_Mesh_QueryInterface.hpp"

int main()
{
  
  MDB_Mesh_QueryInterface* mdb = MDB_Mesh_QueryInterface::instance();

  if (!mdb)
    std::cout << "Error: null mesh instance" << std::endl;

  int info; //error code
  mdb->TSTT_Mesh_Load("Should be a filename", &info);

  if (!info)
    std::cout << "Success loading mesh" << std::endl;

  // need ability to query how many entities using the MeshData functions
  TSTT_Entity_Handle entity_handles[200];
  int num_entities;

  mdb->TSTT_Mesh_GetEntities(TSTT_VERTEX, &num_entities, 
                             (TSTT_Entity_Handle*) entity_handles, 
                             &info);

  if (!info)
    std::cout << "Got vertex entities" << std::endl;

  int dimension, type, topology, id, conn[8];
  int i;
  for (i=0; i < num_entities; i++)
  {
    mdb->TSTT_Entity_GetID(entity_handles[i], &id, &info);
    mdb->TSTT_Entity_GetDimension(entity_handles[i], &dimension, &info);
    mdb->TSTT_Entity_GetType(entity_handles[i], &type, &info);
    mdb->TSTT_Entity_GetTopology(entity_handles[i], &topology, &info);

    if (!info)
    {
      cout << "ID: " << id << endl;
      cout << "   Dimension: " << dimension << endl;
      cout << "   Type     : " << type << endl;
      cout << "   Topology : " << topology << endl;
    }
  }

  mdb->TSTT_Mesh_GetEntities(TSTT_EDGE, &num_entities, 
                             (TSTT_Entity_Handle*) entity_handles, 
                             &info);
  if (!info)
    std::cout << "Got bar entities" << std::endl;

  for (i=0; i < num_entities; i++)
  {
    mdb->TSTT_Entity_GetID(entity_handles[i], &id, &info);
    mdb->TSTT_Entity_GetDimension(entity_handles[i], &dimension, &info);
    mdb->TSTT_Entity_GetType(entity_handles[i], &type, &info);
    mdb->TSTT_Entity_GetTopology(entity_handles[i], &topology, &info);

    int num_adjacent;
    TSTT_Entity_Handle adj_entities_ptr[8];
    mdb->TSTT_Entity_GetAdjacencies(entity_handles[i], TSTT_0D_ENTITY, &num_adjacent, 
                                    adj_entities_ptr, &info);

    if (!info)
    {
      std::cout << "ID: " << id << std::endl;
      std::cout << "   Dimension: " << dimension << std::endl;
      std::cout << "   Type     : " << type << std::endl;
      std::cout << "   Topology : " << topology << std::endl;
      std::cout << "   Connectivity: " << conn[0] << ":" << conn[1] << std::endl;
    }
  }
}

*/

