
# this makefile just contains custom build rules, and controls
# what gets built

SHELL = /bin/sh

VERDE_VERSION = 2.6

default : gui_dir verdict_dir help_dir MDBlib verdeexe

gui_dir :
	@ cd GUI && ${QTDIR}/bin/qmake verde.pro
	@ echo && echo "--Building GUI directory--"
	@  echo && cd GUI && ${MAKE}
	@ echo "--Done Building GUI directory--"

verdict_dir :
	@ echo && echo "--Building Verdict directory--"
	@ echo && cd verdict && ${MAKE}
	@ cp verdict/libverdict*.* .
	@ echo "--Done Building Verdict directory--" && echo

help_dir :
	@ cd help && ${QTDIR}/bin/qmake verde_help.pro
	@ echo "--Building help directory--"
	@ echo && cd help && ${MAKE}
	@ echo "--Done Building help directory--" && echo	


verdeexe : custom_files build_VerdeVersion.h gui_dir verdict_dir
	@ ${QTDIR}/bin/qmake -o makefile.verde verde.pro
	@ ${MAKE} -f makefile.verde

MDBlib : 
	@ ${QTDIR}/bin/qmake -o makefile.MDB MDB.pro
	@ echo "--Building MDB library--"
	@ ${MAKE} -f makefile.MDB
	@ echo "--Done Building MDB library--"
	@ echo


custom_files : idr_parser.cpp allwords.h keywords.h idr_lexer.cpp


idr_parser.cpp : idr_parser.y
idr_lexer.cpp : idr_lexer.l

build_VerdeVersion.h :
	@ echo Updating VerdeVersion.h ... 
	@ rm -f VerdeVersion.tmp
	@ date +"#define VERDE_DATE \"%m-%d-%y %r\"" > VerdeVersion.tmp
	@ date +"#define VERDE_VERSION \"$(VERDE_VERSION)\"" >> VerdeVersion.tmp
	@ cp VerdeVersion.tmp VerdeVersion.h

allwords.h : CommandTable.h allwords.sed
	@ echo Updating: allwords.h
	@ sed -n -f allwords.sed CommandTable.h | sort | uniq > allwords.h

keywords.h: CommandTable.h keywords.sed
	@ echo Updating: keywords.h
	@ sed -n -f keywords.sed CommandTable.h | sort > keywords.h


## replace some default suffix rules
.SUFFIXES : .y .l .cpp 

.l.cpp :
	@ echo $<
	@ flex -t $< > $@

.y.cpp :
	@ echo $< 
	@ bison -d -b y $<  
	@ mv y.tab.c $*.cpp
	@ mv y.tab.h $*.h



clean_all : clean cleanGUI cleanHELP
	@- rm -f *.o *_gui.o *~ ./#*  *.a .#* *.sbr *.pch *.idb *.pdb *.bsc *.ilk idr_parser.cpp idr_lexer.cpp idr_parser.h allwords.h keywords.h

clean : cleanMDB cleanEXE
	@- rm -f *.o *_gui.o *~ ./#*  *.a .#* *.sbr *.pch *.idb *.pdb *.bsc *.ilk idr_parser.cpp idr_lexer.cpp idr_parser.h allwords.h keywords.h 

cleanEXE :
	@ ${MAKE} -f makefile.verde clean

cleanMDB :
	@ ${MAKE} -f makefile.MDB clean
	
cleanGUI :
	@ cd GUI && ${MAKE} clean

cleanHELP :
	@ cd help && ${MAKE} clean



tarfile: 
	tar cf ../verde.tar *.cpp *.hpp *.h README VERDE.* makefile 
#manual.*


# DO NOT DELETE THIS LINE -- make depend depends on it.

