//-------------------------------------------------------------------------
// Filename      : EdgeElem.cpp
//
// Purpose       : Class representing a four noded polygon in VERDE.  Unlike
//                 the EdgeRef, the EdgeElem exists independent of any
//                 ElementBlock (or its MeshContainer). 
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 04/02/99
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include "EdgeElem.hpp"
#include "VerdeMessage.hpp"

EdgeElem::EdgeElem(int owner_id, int node_0, int node_1)
   : BaseElem(owner_id)
{
   // load info so that lowest id is in position 1


   if (node_0 < node_1)
   {
      connectivity[0] = node_0;
      connectivity[1] = node_1;
   }
   else
   {
      connectivity[0] = node_1;
      connectivity[1] = node_0;
   }
   timesMarked = 0;
}


void EdgeElem::print()
{
   // print out the edge information

   PRINT_INFO("Edge Connectivity:  %d %d\n",
              connectivity[0], connectivity[1]);
}
