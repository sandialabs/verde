TARGET = MDB
TEMPLATE = lib
CONFIG += warn_on dll verde_MDB
VERSION = 1.0.0

include(platforms.pro.inc)
include(verde.local)

SOURCES = MDBServer.cpp MDBVerde.cpp
HEADERS = MDBInterface.hpp MDBUnknownInterface.h MDBVerde.hpp

