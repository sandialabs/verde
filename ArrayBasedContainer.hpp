// Class: ArrayBasedContainer
// Description: Base class for container classed implemented as arrays.
//              Contains 'void*' entities. This class allows copying
//              between container types.
// Assumptions: Containers derived from this are implemented as arrays.
//
// Owner: Paul Kinney
// Checked by: 
// Version: $Id: 

#ifndef ARRAYBASEDCONTAINER_HPP
#define ARRAYBASEDCONTAINER_HPP

#include "VerdeDefines.hpp"
#include "MemoryManager.hpp"

const unsigned int INCREMENT = 8; // Default list size increment
//! Array based container object
class ArrayBasedContainer
{

public:
  
    /*! Constructor: Create a list with initial size {size}. The list
    // will be grown by {size} each time it is filled. Memory for the
    // list is not allocated until the first element is inserted using
    // {insertLink}. 
    // If {size} is zero, the default increment ({INCREMENT}) will be used
    // From an efficiency standpoint, it is very important that the 
    // increment be set large enough to reduce the number of list 
    // growths, but small enough to not waste memory.
    // It is more efficient to sligthly overestimate the size than 
    // to underestimate the size.
    */
    ArrayBasedContainer (int size);

    //! Copy Constructor
    ArrayBasedContainer(const ArrayBasedContainer& copy_from);

    //! Destructor: Free all resources used by this list.
    ~ArrayBasedContainer();
    
    //! Return the number of items stored in the list.
    int  size() const;      
    
    /*! Delete all elements in the list, reset the pointer to zero.
    // Does not release memory already allocated for the list. This
    // call is more efficient creating a new list repeatadly within
    // a loop.
    */
    void clean_out();

    //! itemCount -= k;  Doesn't change array elements.
    void shrink(int k);
       
    /*! Change the list growth increment (specified in the constructor) 
    // to {increment}. The previous setting of the growth increment is
    // returned.
    */
    int set_increment(int increment);

    //! Create a copy of a list.
    ArrayBasedContainer& operator=(const ArrayBasedContainer&);
    
    //! Append one list to the end of the other list.
    ArrayBasedContainer& operator+=(const ArrayBasedContainer&);

    static unsigned int current_allocated_memory();
    static unsigned int maximum_allocated_memory();
protected:
    
  //! array of items in the list
  void **listArray;            
  //! number of items in list
  int  itemCount;              
  //! size of {listArray}
  unsigned int  listLength;    
  //! TRUE if allocated from special memory pool, FALSE if normal allocation 
  Bit  poolAllocated : 1;      
  //! Used in SDLList 
  Bit  listIsSorted : 1;       
  //! Amount by which to grow list. This is limited to 1.07e9 
  Bit  listIncrement : 30;     
                                   
  //! Protected function to lengthen the list
  void lengthen_list();

private:
  static unsigned int currentAllocatedMemory;
  static unsigned int maxAllocatedMemory;
};

inline void ArrayBasedContainer::shrink(int k)
{
  if (k > 0) 
  {
    if (itemCount >= k)
      itemCount -= k;
    else
      itemCount = 0;
  }
}

inline int ArrayBasedContainer::size() const { return itemCount; }

inline void ArrayBasedContainer::clean_out ()
{
    itemCount = 0;
}


/*!This class is used internally by ArrayBasedContainer to maintain
// a large pool of memory of size {INCREMENT} (The default size for
// ArrayBasedContainers). If a list of size {INCREMENT} is constructed,
// the memory is allocated from this pool rather than allocating the memory
// from the system global new routine.
*/
class ArrayMemory
{
  public:

    //! overloaded new and delete operators
    SetDynamicMemoryAllocation(memoryManager)

  private:

    static MemoryManager  memoryManager;

    void*  mem[INCREMENT];
};

#endif // ARRAYBASEDCONTAINER_HPP

