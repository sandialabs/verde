//-------------------------------------------------------------------------
// Filename      : FailedElem.hpp 
//
// Purpose       : Abstract class containing information needed for an
//                 element when it is created as a result of failing
//                 some metric test
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 03/23/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef FAILED_ELEM_HPP
#define FAILED_ELEM_HPP

//! Base class for all failed element types
class FailedElem
{
public:

   //! Constructor
   FailedElem(int block_id, int index, int original_id, double metric_value);

   //! Destructor
   ~FailedElem () {};
   
   //SetDynamicMemoryAllocation(memoryManager)
   //! class specific new and delete operators

   //! returns the block id of the owning MeshContainer
   int block_id();

   //! returns metric value of this element
   double metric_value();
  
   //! returns the index of this element
   int index();

   //! returns the original id of the element
   int original_id();
 
private:

   //static MemoryManager memoryManager;
   
   //! block where this element came from
   int blockID;

   //! index of the element in the block in came from
   int elementIndex;

   //! the original id of the element (from the exodus element number map)
   int originalId;

   //! value of failed metric which caused this element to be created
   double metricValue;
   
   
};

// inline functions

inline FailedElem::FailedElem(int block_id, int index, int original_id,
                              double metric_value)
{
   blockID = block_id;
   elementIndex = index;
   metricValue = metric_value;
   originalId = original_id;
}


inline int FailedElem::block_id()
{
   return blockID;
}

inline double FailedElem::metric_value()
{
   return metricValue;
}

inline int FailedElem::index()
{
   return elementIndex;
}

inline int FailedElem::original_id()
{
   return originalId;
}

#endif

