# Microsoft Developer Studio Project File - Name="FlexBison" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=FlexBison - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FlexBison.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FlexBison.mak" CFG="FlexBison - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FlexBison - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "FlexBison - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FlexBison - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FlexBison___Win32_Release"
# PROP BASE Intermediate_Dir "FlexBison___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /subsystem:console /machine:I386 /nodefaultlib /out:"FlexBison"

!ELSEIF  "$(CFG)" == "FlexBison - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FlexBison___Win32_Debug"
# PROP BASE Intermediate_Dir "FlexBison___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /nodefaultlib /out:"FlexBison" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FlexBison - Win32 Release"
# Name "FlexBison - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\CommandTable.h

!IF  "$(CFG)" == "FlexBison - Win32 Release"

# Begin Custom Build
InputPath=..\CommandTable.h

BuildCmds= \
	.\sed -n -f ..\allwords.sed ..\CommandTable.h | .\sort | .\uniq > ..\allwords.h \
	.\sed -n -f ..\keywords.sed ..\CommandTable.h | .\sort > ..\keywords.h \
	

"..\allwords.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\keywords.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FlexBison - Win32 Debug"

# Begin Custom Build
InputPath=..\CommandTable.h

BuildCmds= \
	.\sed -n -f ..\allwords.sed ..\CommandTable.h | .\sort | .\uniq > ..\allwords.h \
	.\sed -n -f ..\keywords.sed ..\CommandTable.h | .\sort > ..\keywords.h \
	

"..\allwords.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\keywords.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\idr_lexer.l

!IF  "$(CFG)" == "FlexBison - Win32 Release"

# Begin Custom Build
InputPath=..\idr_lexer.l

"..\idr_lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\flex -t -L ..\idr_lexer.l > ..\idr_lexer.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FlexBison - Win32 Debug"

# Begin Custom Build
InputPath=..\idr_lexer.l

"..\idr_lexer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\flex -t -L ..\idr_lexer.l > ..\idr_lexer.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\idr_parser.y

!IF  "$(CFG)" == "FlexBison - Win32 Release"

# Begin Custom Build
ProjDir=.
InputPath=..\idr_parser.y

BuildCmds= \
	set BISON_HAIRY=$(ProjDir)\bison.hairy \
	set BISON_SIMPLE=$(ProjDir)\bison.simple \
	.\bison -b y -d --no-lines ..\idr_parser.y \
	move y.tab.c ..\idr_parser.cpp \
	move y.tab.h ..\idr_parser.h \
	

"..\idr_parser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\idr_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FlexBison - Win32 Debug"

# Begin Custom Build
ProjDir=.
InputPath=..\idr_parser.y

BuildCmds= \
	set BISON_HAIRY=$(ProjDir)\bison.hairy \
	set BISON_SIMPLE=$(ProjDir)\bison.simple \
	.\bison -b y -d --no-lines ..\idr_parser.y \
	move y.tab.c ..\idr_parser.cpp \
	move y.tab.h ..\idr_parser.h \
	

"..\idr_parser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\idr_parser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\VerdeVersion.h

!IF  "$(CFG)" == "FlexBison - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\VerdeVersion.h

"..\bugus_filename_to_make_this_every_time" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /Q ..\VerdeVersion.h 
	set date = date /T 
	set time = time /T 
	echo #define VERDE_DATE "%date% %time%" > ..\VerdeVersion.h 
	echo #define VERDE_VERSION "2.6" >> ..\VerdeVersion.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FlexBison - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\VerdeVersion.h

"..\bugus_filename_to_make_this_every_time" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /Q ..\VerdeVersion.h 
	set date = date /T 
	set time = time /T 
	echo #define VERDE_DATE "%date% %time%" > ..\VerdeVersion.h 
	echo #define VERDE_VERSION "2.6" >> ..\VerdeVersion.h 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
