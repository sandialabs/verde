//-------------------------------------------------------------------------
// Filename      : FailedPyramid.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the PyramidRef, the FailedPyramid exists independent of any
//                 ElementBlock (or its MeshContainer). 
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 04/02/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef FAILED_PYRAMID_HPP
#define FAILED_PYRAMID_HPP

#include "FailedElem.hpp"
#include "PyramidRef.hpp"

//! represents a failed pyramid in VERDE
class FailedPyramid : public FailedElem
{
   
public:

  //! constructor
  FailedPyramid(PyramidRef &pyramid_ref, double failed_value);

  //! destructor
  ~FailedPyramid () {};
   
  //! class specific new and delete operators
  //SetDynamicMemoryAllocation(memoryManager)

  //! prints the element information
  void print();

  //! returns the node id at position index in connectivity
  int node_id(int index);

private:

  //! connectivity array for pyramid
  int connectivity[NUM_PYRAMID_NODES];
  
};

inline FailedPyramid::FailedPyramid(PyramidRef &elem_ref, double failed_value)
         :FailedElem(elem_ref.block_id(),elem_ref.index(),
                     elem_ref.original_id(),failed_value) 
{
  for (int i = 0; i < NUM_PYRAMID_NODES; i++)
  {
     connectivity[i] = elem_ref.node_id(i);
  }
}

inline void FailedPyramid::print()
{
  // print out the elem information

  PRINT_INFO("Pyramid Connectivity:  ");
  for (int i = 0; i < NUM_PYRAMID_NODES; i++)
     PRINT_INFO("%d ",connectivity[i]);
  PRINT_INFO("\n");
}

inline int FailedPyramid::node_id(int index)
{
  return connectivity[index];
}

#endif

