
# global project settings for all of the verde project files


# find out which platform we are on
system(echo $MACHINE_TYPE | grep lin > /dev/null) : CONFIG += lin_vplatform
system(echo $MACHINE_TYPE | grep hp  > /dev/null) : CONFIG += hp_vplatform
system(echo $MACHINE_TYPE | grep da  > /dev/null) : CONFIG += da_vplatform
system(echo $MACHINE_TYPE | grep sg  > /dev/null) : CONFIG += sg_vplatform
system(echo $MACHINE_TYPE | grep ss  > /dev/null) : CONFIG += ss5_vplatform

OBJECTS_DIR = obj

# platform specfic configs
da_vplatform {
   # make cxx quiet
   QMAKE_CXX = @ echo $< && cxx
   QMAKE_CXXFLAGS += -std strict_ansi -using_std -tlocal
#   verde_EXE:QMAKE_CXXFLAGS += -ptr cxx_repository
#   verde_GUI:QMAKE_CXXFLAGS += -ptr ../cxx_repository
#   QMAKE_LFLAGS += -ptr cxx_repository
   DEFINES += DA VERDE_64_BIT_BUILD
}

hp_vplatform {
   # make aCC quiet
   QMAKE_CXX = @ echo $< && aCC
   DEFINES += HP CANT_USE_STD_IO std=
   QMAKE_CXXFLAGS += +inst_implicit_include
   # tell the HP to use the lib*.so naming for libraries 
   # set HPUX_SHLIB to nothing
   QMAKE_HPUX_SHLIB = 
   QMAKE_LFLAGS += -Wl,+s
}

lin_vplatform {
   DEFINES += LINUX
   # make g++ quiet
   QMAKE_CXX = @ echo $< && $$QMAKE_CXX
   # for using static qt library
   # QMAKE_LIBS_X11 = -lXext -lXmu -lXft -lXinerama -lX11 -lm
}

sg_vplatform {
   DEFINES += SGI

   ABI_FORMAT = $$(QMAKESPEC)
   contains(ABI_FORMAT, irix-cc-64):LIBS += -L$(CUBIT)/exodus/exodus3.07/lib/irix65_n64
   contains(ABI_FORMAT, irix-cc):LIBS += -L$(CUBIT)/exodus/exodus3.07/lib/irix65_n32
   contains(ABI_FORMAT, irix-o32):LIBS += -L$(CUBIT)/exodus/exodus3.07/lib/irix65_o32
   
   contains(ABI_FORMAT, irix-cc-64):DEFINES += VERDE_64_BIT_BUILD

   QMAKE_CXXFLAGS += -LANG:std -woff 1552,3201,1424
   QMAKE_LFLAGS += -LANG:std
   QMAKE_CXX = @echo $< && CC
}

ss5_vplatform {
   # make 
   QMAKE_CXX = @ echo $< && CC
   DEFINES += SOLARIS
}


