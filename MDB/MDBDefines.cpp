

#include "MDBDefines.h"


const int dimension_table[] = {
  MDB_0D_ENTITY,    //MDBVertex
  MDB_1D_ENTITY,    //MDBEdge
  MDB_2D_ENTITY,    //MDBFace
  MDB_3D_ENTITY,    //MDBRegion
  MDB_0D_ENTITY,    //MDBVertex
  MDB_1D_ENTITY,    //MDBBar
  MDB_2D_ENTITY,    //MDBTri
  MDB_2D_ENTITY,    //MDBQuad
  MDB_3D_ENTITY,    //MDBTet
  MDB_3D_ENTITY,    //MDBHex
  MDB_3D_ENTITY,    //MDBPyramid
  MDB_3D_ENTITY,    //MDBKnife ???
  MDB_3D_ENTITY,    //MDBWedge
};


const char hex_quads[] = {
  0, 1, 5 ,4,
  1, 2, 6, 5,
  2, 3, 7, 6,
  3, 0, 4, 7,
  0, 3, 2, 1,
  4, 5, 6, 7
};

const char hex_edges[] = {
  0, 1,
  0, 4,
  1, 2,
  1, 5,
  2, 3,
  2, 6,
  3, 0,
  3, 7,
  4, 5,
  5, 6,
  6, 7,
  7, 4
};

const char tet_tris[] = {
  0, 2, 1,
  0, 1, 3,
  0, 3, 2,
  1, 2, 3
};

const char tet_edges[] = {
  0, 1,
  0, 2,
  0, 3,
  1, 2,
  1, 3,
  2, 3
};

const char quad_edges[] = {
  0, 1,
  1, 2,
  2, 3,
  3, 0
};

const char tri_edges[] = {
  0, 1,
  1, 2,
  2, 0
};




