//-------------------------------------------------------------------------
// Filename      : FailedQuad.hpp 
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the QuadRef, the FailedQuad exists independent of any
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

#ifndef FAILED_QUAD_HPP
#define FAILED_QUAD_HPP

#include "QuadElem.hpp"
#include "FailedElem.hpp"

//! represents a failed quad in VERDE
class FailedQuad : public QuadElem, public FailedElem
{
   
public:

  //! constructor
  FailedQuad(QuadRef &quad_ref, double failed_value);

  //! destructor
  ~FailedQuad () {};
   
  //! class specific new and delete operators
  //SetDynamicMemoryAllocation(memoryManager)

  //! prints the element information
  void print();

};

inline FailedQuad::FailedQuad(QuadRef &elem_ref, double failed_value)
        :QuadElem(elem_ref.index(), elem_ref.node_id(0), elem_ref.node_id(1),
                  elem_ref.node_id(2), elem_ref.node_id(3)),
         FailedElem(elem_ref.block_id(),elem_ref.index(),
                    elem_ref.original_id(),failed_value) 
{}

#endif

