//-------------------------------------------------------------------------
// Filename      : FailedWedge.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the WedgeRef, the FailedWedge exists independent of any
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

#ifndef FAILED_WEDGE_HPP
#define FAILED_WEDGE_HPP

#include "FailedElem.hpp"
#include "WedgeRef.hpp"

//! represents a failed wedge in VERDE
class FailedWedge : public FailedElem
{
   
public:

  //! Constructor
  FailedWedge(WedgeRef &wedge_ref, double failed_value);

  //! Destructor
  ~FailedWedge () {};
  
  //! class specific new and delete operators
  //SetDynamicMemoryAllocation(memoryManager)

  //! prints the element information
  void print();

  //! returns the node id at position index in connectivity
  int node_id(int index);

private:

  //! connectivity array for wedges
  int connectivity[NUM_WEDGE_NODES];
  
};

inline FailedWedge::FailedWedge(WedgeRef &elem_ref, double failed_value)
         :FailedElem(elem_ref.block_id(),elem_ref.index(),
                     elem_ref.original_id(), failed_value) 
{
  for (int i = 0; i < NUM_WEDGE_NODES; i++)
  {
    connectivity[i] = elem_ref.node_id(i);
  }
}

inline void FailedWedge::print()
{
  // print out the elem information

  PRINT_INFO("Wedge Connectivity:  ");
  for (int i = 0; i < NUM_WEDGE_NODES; i++)
    PRINT_INFO("%d ",connectivity[i]);
  PRINT_INFO("\n");
}

inline int FailedWedge::node_id(int index)
{
  return connectivity[index];
}

#endif

