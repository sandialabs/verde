//-       Class: MemoryManager
//-       Owner: Jim Hipp
//- Description: MemoryManager provides object information and stack
//-              storage pointer management used in association with
//-              MemoryBlock and MemoryAllocation classes.
//- Checked By: 
//-    Version:

#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#ifndef VERDE_64_BIT_BUILD
#define USE_DYNAMIC_MEMORY_ALLOCATION
#endif//- comment out this #define to use global system level dynamic memory
//- allocation ... Otherwise overloaded operators new and delete will
//- be set to call operator_new and operator_delete defined in this module

const  int DEFAULT_MEMORY_ALLOC_SIZE = 1024;
//- Default MemoryBlock allocation size

const  int STATIC_MEMORY_MANAGER = 1;
//- Static Memory Manager definition argument

#include <stdlib.h>
#include <assert.h>

class MemoryBlock;

//! maintains dynamically allocated objects in memory blocks
class MemoryManager
{
  private:

    //- name of the class for which this memory manager is declared
    char*          objectName;

    //- memBlockStack is the head of a stack of MemoryBlock objects
    //- that contain allocated memory chunks
    MemoryBlock*   memBlockStack;

    //- headOfFreeList is the head of a stack that contains free
    //- elements that have not yet been constructed by the allocating
    //- object
    char*          headOfFreeList;
    
    //- objectSize is the size of the allocating object
    size_t         objectSize;

    //- memAllocatnSize is the size of the MemoryBlocks that will be
    //- allocated by the allocating object
    int            memAllocatnSize;

    //- next object pointer is used to save memory managers on a
    //- static stack.
    MemoryManager* next;

    //- static flag to indicate that this memory manager is a static
    //- object.  This parameter is used by the destructor when the
    //- program terminates and static object destructors are called.
    //- The destructor calls destroy_memory() which asserts if
    //- the used objects attached to this memory manager have not been
    //- deleted (generally true for static memory managers at program
    //- termination).  Forcing an assert is desirable during normal
    //- program operation but not during a normal program termination
    //- sequence (Typing QUIT at the VERDE command line prompt and
    //- observing an assert in MemoryManager is not comforting).
    int            staticManager;

    //- stack head containing all allocated memory managers.
    static MemoryManager* memoryManagerListHead;

    //- do not allow default constructor (must assign objectSize)
    MemoryManager();
    MemoryManager(const MemoryManager&);

  public:

    //! constructor
    MemoryManager(const char* name, size_t size, int mem_size = 0,
		  int static_flag = 0);
    //! destructor
   ~MemoryManager();

    //! set / get memory block allocation increment
    void   set_memory_allocation_increment(int mem_size = 0);
    //! set / get memory block allocation increment
    int    get_memory_allocation_increment() const;

    //! get object size
    size_t get_object_size() const;

    //! destroy allocated memory
    void   destroy_memory(int static_flag = 0);

    //! compresses memory for the object by removing unused MemoryBlocks
    int    compress_memory();

    //! returns number of object storage locations that have been
    //! allocated
    int    get_allocated_objects();

    //! returns number of objects that are not in use but have been
    //! allocated
    int    get_free_objects();

    //! returns number of objects that are currently in use from those
    //! that have been allocated
    int    get_used_objects();

    //! generic operator new
    void*  operator_new(size_t size);

    //! generic operator delete
    void   operator_delete(void *deadObject, size_t size);

    //! prints allocation information to the command line.  The
    //! functions are declared static to enable their use throughout
    //! the code.  The character string argument is interpreted as the
    //! class name provided as input when the memory manager was
    //! constructed.
    static void show_object_memory(char* s);

    //! prints allocation information to the command line.  The
    //! functions are declared static to enable their use throughout
    //! the code.  The character string argument is interpreted as the
    //! class name provided as input when the memory manager was
    //! constructed.
    static void show_all_object_memory();

    //! recaptures unused memory blocks and returns them to free store
    //! and returns the amount of reclaimed memory.  The functions are
    //! declared static to enable their use throughout the code.  The
    //! character string argument is interpreted as the class name
    //! provided as input when the memory manager was constructed.
    static int compress_object_memory(char* s);

    //! recaptures unused memory blocks and returns them to free store
    //! and returns the amount of reclaimed memory.  The functions are
    //! declared static to enable their use throughout the code.  The
    //! character string argument is interpreted as the class name
    //! provided as input when the memory manager was constructed.
    static int compress_all_object_memory();
};

// return memory size allocation increment
inline int   MemoryManager::get_memory_allocation_increment() const
{
  return memAllocatnSize;
}

// return object size
inline size_t MemoryManager::get_object_size() const
{
  return objectSize;
}

#ifdef USE_DYNAMIC_MEMORY_ALLOCATION

// operator new has argument 'size_t dummy = 0' to prevent warnings
// on windows machine that say - "no matching delete operator found"

#define SetDynamicMemoryAllocation(memManager)                               \
                                                                             \
  void*  operator new(size_t size, size_t /*dummy*/ = 0)                     \
         {return memManager.operator_new(size);}                             \
  void   operator delete(void *deadObject, size_t size)                      \
         {memManager.operator_delete(deadObject, size);}                     \
  /*  overloaded new and delete operators */                                 \
                                                                             \

#else

#define SetDynamicMemoryAllocation(memManager)                               \
                                                                             \
  /* DO NOT overload new and delete operators ... use global allocation */   \
                                                                             \

#endif

#endif // MEMORY_MANAGER_HPP  
