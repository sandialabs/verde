//-------------------------------------------------------------------------
// Filename      : NodeRef.hpp 
//
// Purpose       : Lightweight class which is a reference to a mesh node.
//                 The actual data for the node is accessed through the
//                 MeshContainer.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef NODE_HPP
#define NODE_HPP

#include "VerdeVector.hpp"
#include "MeshContainer.hpp"
#include "MemoryManager.hpp"
#include "ElemRef.hpp"


//! represents a node in VERDE
class NodeRef
{
   static MemoryManager memoryManager;
   
   //static MeshContainer *nodeContainer;
   // singleton static node container for all nodes in Verde
   
   int nodeIndex;
#if defined ( DA ) || defined (SGI) || defined(__x86_64__) || defined(__ai64__) || defined (__ppc64__)
  int waste; //put here because some 64 bit platforms thinks the class is too small otherwise
#endif  
   
   
public:

   //! constructor
   NodeRef(int index);

   //! destructor
   ~NodeRef ();

   //! class specific new and delete operators
   SetDynamicMemoryAllocation(memoryManager)

   //! prints the nodal information
   void print();

   //! return coordinates of the node pointed to by this NodeRef
   VerdeVector coordinates();

   //! return index of the node pointed to by this NodeRef
   int index() const;
   
   //! returns the exodus id assigned (if any) for this nodeRef
   int exodus_id();

   //! sets the exodus id for this nodeRef
   void set_exodus_id(int value);

   //! general marking functions to see if a node has been traversed.
   void mark(VerdeBoolean value = VERDE_TRUE);
   //! general marking functions to see if a node has been traversed.
   VerdeBoolean is_marked();

   //! access to the global nodeContainer object
   MeshContainer *node_container(){return ElemRef::node_container();}


  
};

// inline functions
  
inline int NodeRef::exodus_id()
{
   return node_container()->exodus_id(nodeIndex);
}

inline void NodeRef::set_exodus_id(int value)
{
   node_container()->set_exodus_id(nodeIndex,value);
   
}

inline void NodeRef::mark(VerdeBoolean value)
{
   node_container()->mark(nodeIndex,value);
}

inline VerdeBoolean NodeRef::is_marked()
{
   return node_container()->is_marked(nodeIndex);
}



#endif

