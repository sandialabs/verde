//-------------------------------------------------------------------------
// Filename      : FailedHex.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the HexRef, the FailedHex exists independent of any
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

#ifndef FAILED_HEX_HPP
#define FAILED_HEX_HPP

#include "VerdeMessage.hpp"
#include "FailedElem.hpp"
#include "HexRef.hpp"

//! represents a failed hex in VERDE
class FailedHex : public FailedElem
{
   
public:

  //! constructor
  FailedHex(HexRef &hex_ref, double failed_value);

  //! destructor
  ~FailedHex () {};
   
  //! class specific new and delete operators
  //SetDynamicMemoryAllocation(memoryManager)

  //! prints the element information
  void print();

  //! returns the node id at position index in connectivity
  int node_id(int index);

private:

   //! connectivitiy array for failed hex 
   int connectivity[NUM_HEX_NODES];
   
};

inline FailedHex::FailedHex(HexRef &elem_ref, double failed_value)
         :FailedElem(elem_ref.block_id(),elem_ref.index(),
                     elem_ref.original_id(), failed_value) 
{
  for (int i = 0; i < NUM_HEX_NODES; i++)
  {
    connectivity[i] = elem_ref.node_id(i);
  }
}

inline void FailedHex::print()
{
  // print out the elem information

  PRINT_INFO("Hex Connectivity:  ");
  for (int i = 0; i < NUM_HEX_NODES; i++)
     PRINT_INFO("%d ",connectivity[i]);
  PRINT_INFO("\n");
}

inline int FailedHex::node_id(int index)
{
  return connectivity[index];
}

#endif

