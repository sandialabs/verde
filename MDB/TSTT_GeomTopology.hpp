//
//-------------------------------------------------------------------------
// Filename      : TSTT_GeomTopology.hpp
// Creator       : Tim Tautges
//
// Date          : 03/01/02
//
// Owner         : Tim Tautges
//
// Description   : TSTT_GeomTopology is a class of geometry-related functions,
//                 like skinning a mesh and imprinting meshsets together
//-------------------------------------------------------------------------

#ifndef TSTT_GEOMTOPOLOGY_HPP
#define TSTT_GEOMTOPOLOGY_HPP

#include <vector>

#include "MDB.hpp"

class TSTT_GeomTopology 
{
public:

    //! imprint the sets together; assumes mesh entities are shared,
    //! just changes the sets to be imprinted
  static int imprint_sets(TSTT_MeshSet_Handle *imprint_sets,
                          const int num_imprint_sets,
                          int *info);
  
    //! imprint the sets in imprint_sets1 with those in imprint_sets2;
    //! assumes mesh entities are shared, just changes the sets to be imprinted
  static int imprint_sets(std::vector<TSTT_MeshSet_Handle> &imprint_sets1,
                          std::vector<TSTT_MeshSet_Handle> &imprint_sets2,
                          int *info);
  
    //! imprint set1 and set2 together
  static int imprint_sets(TSTT_MeshSet_Handle set1,
                          TSTT_MeshSet_Handle set2,
                          int *info);
  
  static void XorBoundary(TSTT_MeshSet_Handle set1,
                          TSTT_MeshSet_Handle intersection_set);
};
#endif
