//-------------------------------------------------------------------------
// Filename      : MeshContainer.cc 
//
// Purpose       : Manages mesh entities for its parent entity
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "MeshContainer.hpp"
#include "ElementBlock.hpp"
#include "Mesh.hpp"
#include "VerdeApp.hpp"
#include "exodusII.h"
#include "VerdeMessage.hpp"
#include "MDBInterface.hpp"



MeshContainer::MeshContainer()
{
   nodeX = NULL;
   nodeY = NULL;
   nodeZ = NULL;

   connectivity = NULL;
   exodusId = NULL;
   markArray = NULL;
   numberElements = 0;
   numberNodes = 0;
   numberNodesPerElement = 0;
   numberAttributes = 0;
   attributeData = NULL;
   
}

MeshContainer::~MeshContainer()
{
   // delete node arrays
   if (nodeX)
      delete [] nodeX;
   if (nodeY)
      delete [] nodeY;
   if (nodeZ)
      delete [] nodeZ;
   if (exodusId)
      delete [] exodusId;
   //if (connectivity)
      //delete [] connectivity;
   if (markArray)
      delete [] markArray;
   if (attributeData)
     delete [] attributeData;
   
}

VerdeStatus MeshContainer::load_nodes()
{

   // get the exodus file id
   int exodus_file = verde_app->mesh()->exodus_file();


   // allocate memory for nodal coords
   numberNodes = verde_app->mesh()->number_nodes();
   int previous_numberNodes = verde_app->mesh()->number_previous_nodes();

   int numberNodes_loading = numberNodes - previous_numberNodes; 

   double *x_coords = new double[numberNodes_loading];
   double *y_coords = new double[numberNodes_loading];
   double *z_coords = new double[numberNodes_loading];

   // if no z coords, fill with 0's
   if ( verde_app->mesh()->number_dimensions() == 2 )
   {
      for (int i = 0; i < numberNodes_loading; i++)
      {
         z_coords[i] = 0.0;
      }
   }
   
   if( exodusId ) { delete [] exodusId; } 
   if( markArray ) { delete [] markArray; } 

   exodusId = new int[numberNodes];
   markArray = new VerdeBoolean[numberNodes];

   for (int i = 0; i < numberNodes; i++)
   {
      exodusId[i] = 0;
      markArray[i] = VERDE_FALSE;
   }

   // read in coordinates
   int result = ex_get_coord( exodus_file, x_coords, y_coords, z_coords );

   if ( result == -1 )
   {
      PRINT_ERROR("\nVerde:: problems reading exodus coordinates...\n\n");
      return VERDE_FAILURE;
   }

   MDBErrorCode error;

   // see if we should add_mesh 
   if( verde_app->should_add_mesh() )
   {
     // new array big enough for both
     error = verde_app->MDB()->extend_xyz_arrays( previous_numberNodes, 
                                                             numberNodes_loading, 
                                                             x_coords, y_coords, z_coords);
   }
   else
   {
     // store coordinates in MOI
     error = verde_app->MDB()->load_mesh_vertices(numberNodes,
                                                     x_coords, y_coords, z_coords);
   }

   if (error != MDB_SUCCESS)
   {
     printf("Error storing coords in MOI...\n");
     return VERDE_FAILURE;
   }
  
   return VERDE_SUCCESS;

}

int MeshContainer::read_block_header(int block_id)
{
   // get the exodus file id

   int exodus_file = verde_app->mesh()->exodus_file();

   // get the ElementBlock data

   char element_type[MAX_STR_LENGTH+1];

   int block_id_offset = verde_app->mesh()->blockIdOffset();

   int result = ex_get_elem_block ( exodus_file,
                                block_id - block_id_offset,
                                element_type,
                                &numberElements,
                                &numberNodesPerElement,
                                &numberAttributes );

   if ( result == -1 )
   {
      PRINT_INFO("Verde:: error reading exodus element block %d\n",
           block_id );
      return VERDE_FAILURE;
   }

   elementType = element_type;
   elementType.to_upper_case();

   // read in the attribute data for possible later writing here

   if (numberAttributes)
   {
     // create a double array

     attributeData = new double[numberAttributes*numberElements];
     result = ex_get_elem_attr(exodus_file, block_id - block_id_offset,
                               attributeData);
     if ( result == -1 )
     {
       PRINT_INFO("Verde:: error reading exodus attributes for element block %d\n",
                  block_id );
       return VERDE_FAILURE;
     }
   }
   
   return VERDE_SUCCESS;

}

int MeshContainer::load_elements(int block_id, int *original_ids, int element_offset)
{

   // get the connectivity array

   int exodus_file = verde_app->mesh()->exodus_file();
   originalElementId = original_ids;

   int block_id_offset = verde_app->mesh()->blockIdOffset();

   connectivity = new int[numberElements * numberNodesPerElement];

   int result = ex_get_elem_conn ( exodus_file,
                               block_id - block_id_offset,
                               connectivity );
   if ( result == -1 )
   {
      PRINT_INFO("Verde:: error getting element connectivity for block %d\n",
                 block_id);
      return VERDE_FAILURE;
   }

   //new arrays to exodus ids and marks
   exodusId = new int[numberElements];
   markArray = new VerdeBoolean[numberElements];

   // load the elements into MDB

   // if adding another mesh, offset connectivity
   if( verde_app->should_add_mesh() )
   {
     int connectivity_offset = verde_app->mesh()->number_previous_nodes();

     for(int i = 0; i<numberNodesPerElement*numberElements; i++ )
       connectivity[i] += connectivity_offset;
   }

   MDBErrorCode error_code = verde_app->MDB()->
                             load_mesh_entities (numberElements,
                             element_offset+1, numberNodesPerElement,
                             connectivity, original_ids);


   if (error_code != MDB_SUCCESS)
     return VERDE_FAILURE;

   return VERDE_SUCCESS;

  // todo: make the return type VerdeStatus instead of int

}

int MeshContainer::count_marked_nodes()
{
   int number = 0;

   for(int i = 0; i < numberNodes; i++)
   {
      if (markArray[i] == VERDE_TRUE)
         number++;
   }
   return number;
}

void MeshContainer::clear_marked_nodes()
{
   for(int i = 0; i < numberNodes; i++)
   {
      markArray[i] = VERDE_FALSE;
   }
}

void MeshContainer::process_degenerate_wedges()
{
   // we assume here that the data was read in as hex information,
   // but that the element was intended to be a degenerate wedge

   // we will modify the connectivity of the block so that for each
   // element, we move the 5th, 6th, and 7th elements to the 4th, 5th,
   // and 6th spots respectively.

   // for now, we will not try to modify higher order nodes because
   // they are not used for any calcs


   int *conn_ptr = connectivity;

   for (int i = 0; i < numberElements; i++)
   {
      // move nodes to proper places

      conn_ptr[3] = conn_ptr[4];
      conn_ptr[4] = conn_ptr[5];
      conn_ptr[5] = conn_ptr[6];
      conn_ptr += numberNodesPerElement;
   }
}
/*
VerdeVector MeshContainer::coordinates(int node_index)
{
  double coords[3];
  verde_app->MDB()->GetCoords(node_index, coords);
  return VerdeVector(coords[0],coords[1],coords[2]);
}
*/
int MeshContainer::node_id(int index, int node_pos)
{
  // get the proper index from the MDB

   // look up the hexRef's connectivity

   return (connectivity[(index-1) * numberNodesPerElement + node_pos]);
}

void MeshContainer::owner(ElementBlock *block)
{
   meshOwner = block;
}

int MeshContainer::block_id()
{
   return meshOwner->block_id();
}
