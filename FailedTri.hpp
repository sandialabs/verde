//-------------------------------------------------------------------------
// Filename      : FailedTri.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the TriRef, the FailedTri exists independent of any
//                 ElementBlock (or its MeshContainer). 
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/05/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef FAILED_TRI_HPP
#define FAILED_TRI_HPP

#include "TriElem.hpp"
#include "FailedElem.hpp"
#include "TriRef.hpp"

//! represents a failed tri in VERDE
class FailedTri : public TriElem, public FailedElem
{
   
public:

  //! constructor
  FailedTri(TriRef &tri_ref, double failed_value);

  //! destructor
  ~FailedTri () {};
   
  //! class specific new and delete operators
  //SetDynamicMemoryAllocation(memoryManager)

  //! prints the element information
  void print();

};

inline FailedTri::FailedTri(TriRef &tri_ref, double failed_value)
        :TriElem(tri_ref.index(), tri_ref.node_id(0),tri_ref.node_id(1),tri_ref.node_id(2)),
         FailedElem(tri_ref.block_id(),tri_ref.index(),
                    tri_ref.original_id(),failed_value) 
{}

#endif

