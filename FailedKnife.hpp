//-------------------------------------------------------------------------
// Filename      : FailedKnife.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the KnifeRef, the FailedKnife exists independent of any
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

#ifndef FAILED_KNIFE_HPP
#define FAILED_KNIFE_HPP

#include "FailedElem.hpp"
#include "KnifeRef.hpp"

//! represents a failed knife in VERDE
class FailedKnife : public FailedElem
{
   
public:

  //! Constructor
  FailedKnife(KnifeRef &knife_ref, double failed_value);

  //! Destructor
  ~FailedKnife () {};
   
  //! class specific new and delete operators
  //SetDynamicMemoryAllocation(memoryManager)

  //! prints the element information
  void print();

  //! returns the node id at position index in connectivity
  int node_id(int index);

private:

  //! connectivity array for knife
  int connectivity[NUM_KNIFE_NODES];
  
};

inline FailedKnife::FailedKnife(KnifeRef &elem_ref, double failed_value)
         :FailedElem(elem_ref.block_id(),elem_ref.index(),
                     elem_ref.original_id(),failed_value) 
{
  for (int i = 0; i < NUM_KNIFE_NODES; i++)
  {
     connectivity[i] = elem_ref.node_id(i);
  }
}

inline void FailedKnife::print()
{
  // print out the elem information

  PRINT_INFO("Knife Connectivity:  ");
  for (int i = 0; i < NUM_KNIFE_NODES; i++)
     PRINT_INFO("%d ",connectivity[i]);
  PRINT_INFO("\n");
}

inline int FailedKnife::node_id(int index)
{
  return connectivity[index];
}

#endif

