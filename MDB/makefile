
# this makefile just contains custom build rules, and controls
# what gets built



include MDB.$(MACHINE_TYPE)

DEBUG_FLAG = -g

SHELL = /bin/sh

MDB_LIB_SRCS = \
	MDB_MeshSet.cpp \
	MDBDefines.cpp \
	MDBEntitySequence.cpp \
	MDBImplGeneral.cpp \
	ReadWriteExoII.cpp \
	TagServer.cpp \
	SparseTagCollections.cpp

MDB_LIB_OBJS = ${MDB_LIB_SRCS:.cpp=.o} 
default : libMDB.a

libMDB.a : ${MDB_LIB_OBJS}
	${PREFIX} ${ARCHIVER} libMDB.a $?

mdb_test : MDBTest.o libMDB.a
	${LINKER} ${DEBUG_FLAG} MDBTest.o -L. -lMDB -L${EXODUS_LIB_DIR} -lexoIIv2c -lnetcdf -o mdb_test

test_rms : libMDB.a test_rms.o
	${LINKER} ${DEBUG_FLAG} test_rms.o -L. -lMDB -o test_rms
test_exo : libMDB.a test_exo.o
	${LINKER} ${DEBUG_FLAG} test_exo.o -L. -lMDB -L../util -lVerdeutil -L${EXODUS_LIB_DIR} -lexoIIv2c -lnetcdf -o test_exo

test_tag_server : TagServer.o MDBEntitySequence.o SparseTagCollections.o
	${CXX} ${DEBUG_FLAG} -DTAG_SERVER_TEST -o test_tag_server TagServer.cpp MDBEntitySequence.cpp SparseTagCollections.cpp

test_ent_seq : MDBEntitySequence.o
	${CXX} ${DEBUG_FLAG} -DENTITY_SEQUENCE_TEST -o test_ent_seq ${EXODUS_INCLUDE} -L${EXODUS_LIB_DIR} -lexoIIv2c -lnetcdf MDBEntitySequence.cpp

test_adj : test_adj.o libMDB.a
	${LINKER} ${DEBUG_FLAG} test_adj.o -o test_adj -L. -lMDB -L${EXODUS_LIB_DIR} -lexoIIv2c -lnetcdf

depend : 
	${MAKEDEPEND} -f make.dependencies ${MDB_LIB_SRCS} test_exo.cpp test_rms.cpp MDBTest.cpp 

clean_all : clean
	@- rm -f *.a
	@- rm -rf ${TEMPLATE_DIR}

clean : 
	@- rm -f *.o
	@- rm -rf ${TEMPLATE_DIR}

.SUFFIXES : .o .cpp .c

.cpp.o:
	${ECHO_COMMAND} 
	${PREFIX} ${CXX} ${DEBUG_FLAG} ${MACH_CXXFLAGS} ${EXODUS_INCLUDE} -c -o $@ $<


include make.dependencies

# DO NOT DELETE THIS LINE -- make depend depends on it.

