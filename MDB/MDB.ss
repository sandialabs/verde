#
# This file contains make macro definitions needed for a cubit developers
# installation.  This file is included directly into the CUBIT makefile,
# so definitions are in make macro format
#
# Any flags here can be overridden in the verde.local file
#

#
# location of exodus libs and makedepend
#
CUBITROOT = $(CUBIT)

#
# SOURCE_DIR:
# Developer's primary cubit source directory (i.e. the directory from which
# cvs operations are done)
#
SOURCE_DIR = .


#
# EXODUS_LIB_DIR, EXODUS_INCLUDE:
# Where to look for EXODUS includes and libs
#
EXODUS_INCLUDE =   -I$(CUBITROOT)/include
EXODUS_LIB_DIR = $(CUBITROOT)/lib
EXODUS_LIBS = -L$(EXODUS_LIB_DIR) -lexoIIv2c -lnetcdf
VERDICT_LIB_NAME = libverdict.a
VERDICT_LIB = -Lverdict -lverdict
LIBS = -lm

# Machine-dependent include paths
CC_INCLUDE = -I/net/sasn232/opt/SUNWspro.old/SC5.0/include/CCios

# Optimize flags for this platform 
DEBUG_FLAG = -fast -DNDEBUG

# machine CXX flags if you want to name verde something else, 
#change the -D'NAME="verde"' in the next line and set the EXE_NAME = new_name
#MACH_CXXFLAGS = -D__EXTERN_C__ -DSOLARIS \
#                -DSUN -DSVR4 -DSYSV -DANSI -DXTFUNCPROTO +w  -compat 
MACH_CXXFLAGS = -D'NAME="verde"' -DSOLARIS

#MACH_LINK_LIBS = -liostream -lC

# Some controls on echoing of compile commands; for verbose output, comment
# out the following two definitions
PREFIX = @
ECHO_COMMAND = @ echo "Compiling(${DEBUG_FLAG}): $<"


# Some compiler definitions - check your environment to make sure the correct
# compilers are being referenced
#
# CC:
CC = cc
CXX = CC
LINKER = ${CXX}
MAKE = make 
ARCHIVER = ${CXX} -xar -o

# MAKEDEPEND:
MAKEDEPEND = $(CUBITROOT)/bin/makedepend

# location of templates for template classes, helps make clean_all
TEMPLATE_DIR = SunWS_cache

