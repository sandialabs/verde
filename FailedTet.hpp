//-------------------------------------------------------------------------
// Filename      : FailedTet.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the TetRef, the FailedTet exists independent of any
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

#ifndef FAILED_TET_HPP
#define FAILED_TET_HPP

#include "FailedElem.hpp"
#include "TetRef.hpp"

//! represents a failed tet in VERDE
class FailedTet : public FailedElem
{
   
public:

  //! constructor
  FailedTet(TetRef &tet_ref, double failed_value);

  //! destructor
  ~FailedTet () {};
   
  //! class specific new and delete operators
  //SetDynamicMemoryAllocation(memoryManager)

  //! prints the element information
  void print();

  //! returns the node id at position index in connectivity
  int node_id(int index);

private:

  //! connectivity array for tet
  int connectivity[NUM_TET_NODES];
  
};

inline FailedTet::FailedTet(TetRef &elem_ref, double failed_value)
         :FailedElem(elem_ref.block_id(),elem_ref.index(),
                     elem_ref.original_id(), failed_value) 
{
  for (int i = 0; i < NUM_TET_NODES; i++)
  {
     connectivity[i] = elem_ref.node_id(i);
  }
}

inline void FailedTet::print()
{
  // print out the elem information

  PRINT_INFO("Tet Connectivity:  ");
  for (int i = 0; i < NUM_TET_NODES; i++)
     PRINT_INFO("%d ",connectivity[i]);
  PRINT_INFO("\n");
}

inline int FailedTet::node_id(int index)
{
  return connectivity[index];
}

#endif

