//-------------------------------------------------------------------------
// Filename      : NodeRef.cc
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

#include "NodeRef.hpp"


NodeRef::NodeRef(int index)
{
   nodeIndex = index;


   // check if index is within bounds

   if (index < 1 || index > node_container()->number_nodes())
   {
      PRINT_INFO("Verde: Error, node index outside of range...\n");
   }

}

NodeRef::~NodeRef()
{}

void NodeRef::print()
{
   // print information

   //cout << "NodeRef " << nodeIndex << ":  "
   //     << "X = " << x << "  Y = " << y << "  Z = " << z << "\n";
}

VerdeVector NodeRef::coordinates() 
{
   return node_container()->coordinates(nodeIndex);
   
}

int NodeRef::index() const
{
   return nodeIndex;
}
