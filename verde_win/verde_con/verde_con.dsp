# Microsoft Developer Studio Project File - Name="verde_con" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=verde_con - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "verde_con.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "verde_con.mak" CFG="verde_con - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "verde_con - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "verde_con - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "verde_con - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /GX /O1 /I "$(QTDIR)\include" /I "f:\qt\3.0.0\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 $(QTDIR)\lib\qt-mt304.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:console /machine:IX86 /out:"..\Release\verde.com"

!ELSEIF  "$(CFG)" == "verde_con - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /I "f:\qt\3.0.0\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 $(QTDIR)\lib\qt-mt304.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:console /incremental:no /debug /machine:IX86 /out:"..\Debug\verde.com" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "verde_con - Win32 Release"
# Name "verde_con - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\StdioHandler.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdioHandler.h

!IF  "$(CFG)" == "verde_con - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing StdioHandler.h...
InputDir=.
InputPath=.\StdioHandler.h
InputName=StdioHandler

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_con - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing StdioHandler.h...
InputDir=.
InputPath=.\StdioHandler.h
InputName=StdioHandler

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Forms"

# PROP Default_Filter "ui"
# End Group
# Begin Group "Lexables"

# PROP Default_Filter "l"
# End Group
# Begin Group "Yaccables"

# PROP Default_Filter "y"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\moc_StdioHandler.cpp
# End Source File
# End Group
# End Target
# End Project
