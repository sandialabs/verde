#include "TSTT_Mesh_QueryInterface.h"

const char *TSTT_Mesh_QueryInterface::EntityTypeNames[] =
{
  "TSTT_VERTEX", 
  "TSTT_EDGE", 
  "TSTT_FACE", 
  "TSTT_REGION", 
  "TSTT_NONE", 
  "TSTT_NONE", 
  "TSTT_NONE", 
  "TSTT_NONE", 
  "TSTT_POLYGON", 
  "TSTT_TRIANGLE", 
  "TSTT_QUADRILATERAL", 
  "TSTT_POLYHEDRON", 
  "TSTT_TETRAHEDRON", 
  "TSTT_HEXAHEDRON", 
  "TSTT_PRISM", 
  "TSTT_PYRAMID", 
  "TSTT_SEPTAHEDRON"
};

const int TSTT_Mesh_QueryInterface::VerticesPerElement[] = 
{
  1,   // TSTT_VERTEX    
  2,   // TSTT_EDGE      
  -1,   // TSTT_FACE      
  -1,   // TSTT_REGION    
  -1,   // TSTT_NONE      
  -1,   // TSTT_NONE      
  -1,   // TSTT_NONE      
  -1,   // TSTT_POLYGON   
  3,   // TSTT_TRIANGLE  
  4,   // TSTT_QUADRILATERAL
  -1,   // TSTT_POLYHEDRON
  4,   // TSTT_TETRAHEDRON
  8,   // TSTT_HEXAHEDRON
  6,   // TSTT_PRISM     
  5,   // TSTT_PYRAMID    
  -1    // TSTT_SEPTAHEDRON
};
