//-------------------------------------------------------------------------
// Filename      : FailedEdge.hpp 
//
// Purpose       : Class representing a two noded edge in VERDE
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/05/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef FAILED_EDGE_HPP
#define FAILED_EDGE_HPP

#include "VerdeDefines.hpp"
#include "MemoryManager.hpp"
#include "EdgeElem.hpp"
#include "FailedElem.hpp"
#include "EdgeRef.hpp"

//! represents a failed edge in VERDE
class FailedEdge : public EdgeElem, public FailedElem
{
   
public:

   //! Constructor
   FailedEdge(EdgeRef &edge_ref, double metric_value);

   //! Destructor
   ~FailedEdge () {};

   //SetDynamicMemoryAllocation(memoryManager)
   // class specific new and delete operators   
   
};

inline FailedEdge::FailedEdge(EdgeRef &edge_ref, double metric_value)
        :EdgeElem(edge_ref.index(), edge_ref.node_id(0), edge_ref.node_id(1)),
         FailedElem(edge_ref.block_id(),edge_ref.index(), 
                    edge_ref.original_id(),metric_value)
{}
   

#endif

