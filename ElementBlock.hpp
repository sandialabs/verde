//! ElementBlock: Contains information for an Exodus element block
//-------------------------------------------------------------------------
// Filename      : ElementBlock.hpp 
//
// Purpose       : Contains information for an Exodus element block
//
// Special Notes :  
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef ELEMENT_BLOCK_HPP
#define ELEMENT_BLOCK_HPP

#include "MeshContainer.hpp"
#include "BoundingBox.hpp" 
#include "SkinTool.hpp" 
#include "EdgeTool.hpp" 


//! Contains information for an Exodus element block
class ElementBlock
{

  //! block ID for this block
  int blockID;

  //! active flag for this block
  VerdeBoolean blockActive;
  VerdeBoolean elementsLoaded;

  //! Has its own SkinTool and EdgeTool
  SkinTool skinTool;
  //EdgeTool edgeTool;

  //! contains the element data for the ElementBlock 
  MeshContainer elementData;

  //! bounding box of Element Block
  BoundingBox boundingBox;

  //! reads block information from the mesh file
  int read_block_header();

  //! resets element block if required for degenerate elements
  void set_block_element_type();

  //! array housing element number map (read in from Exodus file)
  static int *elementNumberMap;
 
  //! offset into global array of elemnts 
  static int elementNumberMapIndex;

  //! total number active element blocks
  static int numActiveBlocks;
 
  static bool wasJustZero;
 
  //! offset that element ids start from ( block 1 starts with 1 )
  int elementOffset;
  
  //! array of element id's cooresponding with this block
  int *elementNumber;

  //! these allow the tracking of original element numbers
  //
  //! process hex elements
  void process_hex_block(VerdeBoolean do_skin,
                          VerdeBoolean do_metrics,
                          VerdeBoolean do_individual,
                          VerdeBoolean do_euler);
   
  //! process tet elements
  void process_tet_block(VerdeBoolean do_skin,
                          VerdeBoolean do_metrics,
                          VerdeBoolean do_individual,
                          VerdeBoolean do_euler);
   
  //! process pyramid elements
  void process_pyramid_block(VerdeBoolean do_skin,
                              VerdeBoolean do_metrics,
                              VerdeBoolean do_individual,
                              VerdeBoolean do_euler);
   
  //! process wedge elements
  void process_wedge_block(VerdeBoolean do_skin,
                            VerdeBoolean do_metrics,
                            VerdeBoolean do_individual,
                            VerdeBoolean do_euler);
   
   //! process knife elements
   void process_knife_block(VerdeBoolean do_skin,
                            VerdeBoolean do_metrics,
                            VerdeBoolean do_individual,
                            VerdeBoolean do_euler);
   
   //! process quad elements
   void process_quad_block(VerdeBoolean do_skin,
                           VerdeBoolean do_metrics,
                           VerdeBoolean do_individual,
                           VerdeBoolean do_euler);
   
   //! process tri elements
   void process_tri_block(VerdeBoolean do_skin,
                          VerdeBoolean do_metrics,
                          VerdeBoolean do_individual,
                          VerdeBoolean do_euler);

   //! process beam elements
   void process_edge_block(VerdeBoolean do_skin,
                           VerdeBoolean do_metrics,
                           VerdeBoolean do_individual,
                           VerdeBoolean do_euler);

   
public:

   //! Constructor
   ElementBlock(int block_id);

   //! Destructor
   ~ElementBlock ();

   /*! loads the elements within the ElementBlock, returns
   //  number of elements in block */
   int load_elements();

   VerdeBoolean elements_loaded() { return elementsLoaded; }

   //! get edge and skin tools
   SkinTool* skin_tool(); 
   //EdgeTool* edge_tool();

   //! process the element block
   VerdeStatus process(VerdeBoolean do_skin,
                       VerdeBoolean do_metrics,
                       VerdeBoolean do_individual,
                       VerdeBoolean do_euler);

   //! return the block id of the block
   int block_id();

   //! returns a pointer to the mesh_container
   MeshContainer *mesh_container();

  //! reads in the exodus_element_number_map to track original ideas
  static VerdeStatus read_element_number_map(int number_prev_elements,
                                             int number_new_elements,
                                             int exodus_id);

  int number_elements();
  int element_offset();

  //! returns the total number of active element blocks
  static int number_active_blocks() { return numActiveBlocks; }

  static bool was_just_zero() { return wasJustZero; }

  //! reset static variable 'numActiveBlocks'
  static void static_reset() { numActiveBlocks = 0; }
 
  //! returns the original id of an element
  static int original_id(int index);

  //! returns the node id of an element with a node offset
  int node_id(int element_index, int node_index);

  VerdeString element_type();

  //! sets active flag ElementBlock::blockActive
  void active(VerdeBoolean value);

  //! reads active flag ElementBlock::blockActive
  VerdeBoolean is_active();

  //! retuns the bouding box member
  BoundingBox* bounding_box() { return &boundingBox; }

  VerdeVector centroid();
};

// inline functions

inline int ElementBlock::block_id()
{
   return blockID;
}

inline MeshContainer *ElementBlock::mesh_container()
{
   return &elementData;
}

inline VerdeString ElementBlock::element_type()
{
	return elementData.element_type();
}

inline int ElementBlock::number_elements()
{
	return elementData.number_elements();
}

inline SkinTool* ElementBlock::skin_tool()
{
	return &skinTool;
}

/*
inline EdgeTool* ElementBlock::edge_tool()
{
	return &edgeTool;
}
*/

inline int ElementBlock::element_offset()
{
	return elementOffset;
}

inline int ElementBlock::original_id(int index)
{
	return elementNumberMap[index-1];
}

inline int ElementBlock::node_id(int element_index, int node_index)
{
	return elementData.node_id(element_index, node_index);
}

inline void ElementBlock::active(VerdeBoolean value)
{

        if(value && !blockActive)
        {
          if(numActiveBlocks == 0)
            wasJustZero = true;
          else
            wasJustZero = false;

          numActiveBlocks++;
        }
        else if( !value && blockActive)
          numActiveBlocks--;

	blockActive = value;
}

inline VerdeBoolean ElementBlock::is_active()
{
	return blockActive;
}

#endif

