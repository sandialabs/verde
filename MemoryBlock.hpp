//-------------------------------------------------------------------------
// Filename      : MemoryBlock.hpp 
//
// Purpose       : MemoryBlock provides stack storage for block or "Chunk"
//                 memory allocated with overloaded 'new' operators in classes
//                 utilizing a MemoryManager object.
//
// Special Notes : 
//
// Creator       : Jim Hipp
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef MEMORY_BLOCK_HPP
#define MEMORY_BLOCK_HPP

#include <stdlib.h>

//! memory block class for maintaining chunks of memory
class MemoryBlock
{
  private:

    //! pointer to the next MemoryBlock object
    MemoryBlock* next;

    //! block is the pointer to the beginning of the memory block of objects of type char
    char*        block;

    //! size of the memory block
    int          size;

  public:

    //! Constructor: performs initialization assignment
    MemoryBlock (MemoryBlock* Head, char* Block, int Size)
    : next(Head), block(Block), size(Size) {}

    //! Destructor: performs recursive delete of MemoryBlock stack
   ~MemoryBlock();

    //! returns total amount of allocated memory (bytes)
    int get_memory_allocation();

    //! retreive / set next pointer
    MemoryBlock* next_block() const;
    //! retreive / set next pointer
    void         next_block(MemoryBlock* next_block);

    //! get block_size
    int block_size() const;

    //! get block
    char* get_block() const;
};

inline MemoryBlock* MemoryBlock::next_block() const
{
  return next;
}

inline void         MemoryBlock::next_block(MemoryBlock* nxt_blck)
{
  next = nxt_blck;
}

inline int          MemoryBlock::block_size() const
{
  return size;
}

inline char*        MemoryBlock::get_block() const
{
  return block;
}

#endif // MEMORY_BLOCK_HPP
