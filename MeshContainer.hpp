//-------------------------------------------------------------------------
// Filename      : MeshContainer.hpp 
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

#ifndef MESH_CONTAINER_HPP
#define MESH_CONTAINER_HPP

#include "VerdeDefines.hpp"
#include "VerdeVector.hpp"
#include "VerdeString.hpp"
#include "VerdeApp.hpp"
#include "MDBInterface.hpp"


//#include "ElementBlock.hpp"


class HexRef;
class VerdeString;
class ElementBlock;


//! container for storing nodes and elements
class MeshContainer
{
public:
   
   //! Constructor
   MeshContainer();

   //! Destructor
   ~MeshContainer();

   //! puts the proper nodal data in the MeshData
   VerdeStatus load_nodes();

   //! puts the proper element data in the MeshData
   int load_elements(int block_id, int *exodus_ids, int element_offset);
   
   //! returns the connectivity array at element_index
   int node_id(int element_index, int node_id);

   //! returns vector of coordinates of the node at node_index
   VerdeVector coordinates(int node_index);

   //! return number of nodes defined in this container
   int number_nodes();

   //! return number of elements defined in this container
   int number_elements();

   //! return the element type string for this container
   VerdeString element_type();

   //! return the element type string for this container
   void set_element_type(VerdeString new_type);

   //! returns the exodus id assigned (if any) for the node at index
   int exodus_id(int index);

   //! returns original id of element (from exodus element number map)
   int original_id(int index);

   //! sets the exodus id for the node at index
   void set_exodus_id(int index, int value);

   //! returns the marked flag for node at index
   VerdeBoolean is_marked(int index);

   //! sets the marked flag for node at index
   void mark(int index, VerdeBoolean value);

   //! returns the number of nodes in this container with marked flag set
   int count_marked_nodes();

   //! clears marked flag from all nodes in this container
   void clear_marked_nodes();

   //! routine to modify connectivity of degenerate hexes to make correct
   //  wedges
   void process_degenerate_wedges();

   //! read the block header information
   int read_block_header(int block_id);

   //! returns the element block of this MeshContainer
   //int element_block();
 
   //! sets the owning block of this mesh container
   void owner(ElementBlock *block);

   //! returns the owning block of this mesh container
   ElementBlock *owner();

   //! returns the block id of the owner
   int block_id();

  //! returns the number of attributes defines
  int number_attributes();

  //! returns a pointer to the attribute data for output
  double *attribute_data();
  

private:

   //! Arrays to hold nodal information for this meshData
   double *nodeX;
   double *nodeY;
   double *nodeZ;

   //! array to hold the element information for this mesheshData
   int *connectivity;

   //! array which holds the exodus id for each node
   int *exodusId;

   //! array which holds original exodus id for each element
   int *originalElementId;

   //! array which holds the marked flag for each node
   VerdeBoolean *markArray;

   //! exodus element type
   VerdeString elementType;

   //! number of elements in this meshData
   int numberElements;

   //! number of nodes in this meshData
   int numberNodes;

   //! number of nodes per element in this meshData
   int numberNodesPerElement;

  //! number of exodus attributes associated with this meshData
  int numberAttributes;
  
  //! pointer to attribute data read in from exodus file
  double *attributeData;
  

   ElementBlock *meshOwner;
   //- the owner of the MeshContainer

};



inline VerdeVector MeshContainer::coordinates(int node_index)
{
  double coords[3];
  verde_app->MDB()->GetCoords(node_index, coords);
  return VerdeVector(coords[0],coords[1],coords[2]);
}


inline int MeshContainer::exodus_id(int index)
{
   return exodusId[index-1];
}

inline int MeshContainer::original_id(int index)
{
   return originalElementId[index-1];
}

inline void MeshContainer::set_exodus_id(int index, int value)
{
   exodusId[index-1] = value;
}

inline VerdeBoolean MeshContainer::is_marked(int index)
{
   if(numberElements)
     assert(index <= numberElements);
   return markArray[index-1];
}

inline void MeshContainer::mark(int index, VerdeBoolean value)
{
   if(numberElements)
     assert(index <= numberElements);
   markArray[index-1] = value;
}

inline int MeshContainer::number_nodes()
{
   return numberNodes;
}

inline int MeshContainer::number_elements()
{
   return numberElements;
}

inline VerdeString MeshContainer::element_type()
{
   return elementType;
}

inline void MeshContainer::set_element_type(VerdeString new_type)
{
   elementType = new_type;
   elementType.to_upper_case();
}

inline ElementBlock *MeshContainer::owner()
{
   return meshOwner;
}

inline int MeshContainer::number_attributes()
{
   return numberAttributes;
}

inline double *MeshContainer::attribute_data()
{
  return attributeData;
}

#endif
