//-      Module: AAllocMemManagers.cc
//-       Owner: Jim Hipp
//- Description: This module is used to initialize all MemoryManager objects.
//-              The file is intended to be processed before any other source
//-              modules to ensure that these global objects are intialized
//-              before they are used.
//- Checked By: 
//-    Version:
#include "NodeRef.hpp"
#include "HexRef.hpp"
#include "TetRef.hpp"
#include "TriRef.hpp"
#include "QuadRef.hpp"
#include "PyramidRef.hpp"
#include "WedgeRef.hpp"
#include "KnifeRef.hpp"
#include "EdgeRef.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"
#include "EdgeElem.hpp"
#include "ArrayBasedContainer.hpp"

// allocation constants

const int ARRAYMEMORY_ALLOC_SIZE    = 8192;

const int NODE_ALLOC_SIZE           = 1024;
const int EDGE_ALLOC_SIZE           = NODE_ALLOC_SIZE;
const int QUAD_ALLOC_SIZE           = NODE_ALLOC_SIZE;
const int HEX_ALLOC_SIZE            = NODE_ALLOC_SIZE;
const int TRI_ALLOC_SIZE            = QUAD_ALLOC_SIZE;

// ArrayMemory, and List (DLList, QueueNode, Tree) memory managers

MemoryManager ArrayMemory::memoryManager("ArrayMemory", sizeof(ArrayMemory),
                                         ARRAYMEMORY_ALLOC_SIZE,
                                         STATIC_MEMORY_MANAGER);
//MemoryManager DLList::memoryManager("DLList", sizeof(DLList),
//                                    DLLIST_ALLOC_SIZE,
//                                    STATIC_MEMORY_MANAGER);
				  
// Mesh database memory managers

MemoryManager NodeRef::memoryManager("NodeRef", sizeof(NodeRef),
                                       NODE_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager HexRef::memoryManager("HexRef", sizeof(HexRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager TetRef::memoryManager("TetRef", sizeof(TetRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager TriRef::memoryManager("TriRef", sizeof(TriRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager QuadRef::memoryManager("QuadRef", sizeof(QuadRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager EdgeRef::memoryManager("EdgeRef", sizeof(EdgeRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager PyramidRef::memoryManager("PyramidRef", sizeof(PyramidRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager WedgeRef::memoryManager("WedgeRef", sizeof(WedgeRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager KnifeRef::memoryManager("KnifeRef", sizeof(KnifeRef),
                                       HEX_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager QuadElem::memoryManager("QuadElem", sizeof(QuadElem),
                                       QUAD_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager TriElem::memoryManager("TriElem", sizeof(TriElem),
                                       TRI_ALLOC_SIZE,
                                       STATIC_MEMORY_MANAGER);

MemoryManager EdgeElem::memoryManager("EdgeElem", sizeof(EdgeElem),
                                       EDGE_ALLOC_SIZE,  
                                       STATIC_MEMORY_MANAGER);

