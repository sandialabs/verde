# Microsoft Developer Studio Project File - Name="verde_help" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=verde_help - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "verde_help.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "verde_help.mak" CFG="verde_help - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "verde_help - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "verde_help - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "verde_help - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /I "$(QTDIR)\include" /I "E:\Qt\3.0.0\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 $(QTDIR)\lib\qt-mt304.lib $(QTDIR)\lib\qtmain.lib advapi32.lib /nologo /subsystem:windows /machine:IX86
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy     Release\verde_help.exe     ..\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /I "E:\Qt\3.0.0\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "UNICODE" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 $(QTDIR)\lib\qt-mt304.lib $(QTDIR)\lib\qtmain.lib advapi32.lib /nologo /subsystem:windows /debug /machine:IX86 /nodefaultlib:"msvcrt.lib" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy     Debug\verde_help.exe     ..\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "verde_help - Win32 Release"
# Name "verde_help - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\assistant.cpp
# End Source File
# Begin Source File

SOURCE=.\finddialog.ui.h
# End Source File
# Begin Source File

SOURCE=.\helpdialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\helpwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\mainwindow.ui.h
# End Source File
# Begin Source File

SOURCE=.\moc_textbrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\settingsdialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\structureparser.cpp
# End Source File
# Begin Source File

SOURCE=.\textbrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\topicchooserimpl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\assistant.h

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Moc'ing assistant.h...
InputPath=.\assistant.h

"moc_assistant.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc assistant.h -o moc_assistant.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Moc'ing assistant.h...
InputPath=.\assistant.h

"moc_assistant.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc assistant.h -o moc_assistant.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\helpdialogimpl.h

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Moc'ing helpdialogimpl.h...
InputPath=.\helpdialogimpl.h

"moc_helpdialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc helpdialogimpl.h -o moc_helpdialogimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Moc'ing helpdialogimpl.h...
InputPath=.\helpdialogimpl.h

"moc_helpdialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc helpdialogimpl.h -o moc_helpdialogimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\helpwindow.h

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Moc'ing helpwindow.h...
InputPath=.\helpwindow.h

"moc_helpwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc helpwindow.h -o moc_helpwindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Moc'ing helpwindow.h...
InputPath=.\helpwindow.h

"moc_helpwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc helpwindow.h -o moc_helpwindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\settingsdialogimpl.h

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Moc'ing settingsdialogimpl.h...
InputPath=.\settingsdialogimpl.h

"moc_settingsdialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc settingsdialogimpl.h -o moc_settingsdialogimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Moc'ing settingsdialogimpl.h...
InputPath=.\settingsdialogimpl.h

"moc_settingsdialogimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc settingsdialogimpl.h -o moc_settingsdialogimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\structureparser.h
# End Source File
# Begin Source File

SOURCE=.\textbrowser.h

!IF  "$(CFG)" == "verde_help - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing textbrowser.h...
InputDir=.
InputPath=.\textbrowser.h
InputName=textbrowser

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOCing textbrowser.h...
InputDir=.
InputPath=.\textbrowser.h
InputName=textbrowser

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\topicchooserimpl.h

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Moc'ing topicchooserimpl.h...
InputPath=.\topicchooserimpl.h

"moc_topicchooserimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc topicchooserimpl.h -o moc_topicchooserimpl.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Moc'ing topicchooserimpl.h...
InputPath=.\topicchooserimpl.h

"moc_topicchooserimpl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc topicchooserimpl.h -o moc_topicchooserimpl.cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Forms"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\finddialog.ui

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Uic'ing finddialog.ui...
InputPath=.\finddialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic finddialog.ui -o finddialog.h \
	$(QTDIR)\bin\uic finddialog.ui -i finddialog.h -o finddialog.cpp \
	$(QTDIR)\bin\moc finddialog.h -o moc_finddialog.cpp \
	

"finddialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Uic'ing finddialog.ui...
InputPath=.\finddialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic finddialog.ui -o finddialog.h \
	$(QTDIR)\bin\uic finddialog.ui -i finddialog.h -o finddialog.cpp \
	$(QTDIR)\bin\moc finddialog.h -o moc_finddialog.cpp \
	

"finddialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_finddialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\helpdialog.ui

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Uic'ing helpdialog.ui...
InputPath=.\helpdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic helpdialog.ui -o helpdialog.h \
	$(QTDIR)\bin\uic helpdialog.ui -i helpdialog.h -o helpdialog.cpp \
	$(QTDIR)\bin\moc helpdialog.h -o moc_helpdialog.cpp \
	

"helpdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"helpdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_helpdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Uic'ing helpdialog.ui...
InputPath=.\helpdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic helpdialog.ui -o helpdialog.h \
	$(QTDIR)\bin\uic helpdialog.ui -i helpdialog.h -o helpdialog.cpp \
	$(QTDIR)\bin\moc helpdialog.h -o moc_helpdialog.cpp \
	

"helpdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"helpdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_helpdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mainwindow.ui

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Uic'ing mainwindow.ui...
InputPath=.\mainwindow.ui

BuildCmds= \
	$(QTDIR)\bin\uic mainwindow.ui -o mainwindow.h \
	$(QTDIR)\bin\uic mainwindow.ui -i mainwindow.h -o mainwindow.cpp \
	$(QTDIR)\bin\moc mainwindow.h -o moc_mainwindow.cpp \
	

"mainwindow.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"mainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_mainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Uic'ing mainwindow.ui...
InputPath=.\mainwindow.ui

BuildCmds= \
	$(QTDIR)\bin\uic mainwindow.ui -o mainwindow.h \
	$(QTDIR)\bin\uic mainwindow.ui -i mainwindow.h -o mainwindow.cpp \
	$(QTDIR)\bin\moc mainwindow.h -o moc_mainwindow.cpp \
	

"mainwindow.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"mainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_mainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\settingsdialog.ui

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Uic'ing settingsdialog.ui...
InputPath=.\settingsdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic settingsdialog.ui -o settingsdialog.h \
	$(QTDIR)\bin\uic settingsdialog.ui -i settingsdialog.h -o settingsdialog.cpp \
	$(QTDIR)\bin\moc settingsdialog.h -o moc_settingsdialog.cpp \
	

"settingsdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"settingsdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_settingsdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Uic'ing settingsdialog.ui...
InputPath=.\settingsdialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic settingsdialog.ui -o settingsdialog.h \
	$(QTDIR)\bin\uic settingsdialog.ui -i settingsdialog.h -o settingsdialog.cpp \
	$(QTDIR)\bin\moc settingsdialog.h -o moc_settingsdialog.cpp \
	

"settingsdialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"settingsdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_settingsdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\topicchooser.ui

!IF  "$(CFG)" == "verde_help - Win32 Release"

# Begin Custom Build - Uic'ing topicchooser.ui...
InputPath=.\topicchooser.ui

BuildCmds= \
	$(QTDIR)\bin\uic topicchooser.ui -o topicchooser.h \
	$(QTDIR)\bin\uic topicchooser.ui -i topicchooser.h -o topicchooser.cpp \
	$(QTDIR)\bin\moc topicchooser.h -o moc_topicchooser.cpp \
	

"topicchooser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"topicchooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_topicchooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "verde_help - Win32 Debug"

# Begin Custom Build - Uic'ing topicchooser.ui...
InputPath=.\topicchooser.ui

BuildCmds= \
	$(QTDIR)\bin\uic topicchooser.ui -o topicchooser.h \
	$(QTDIR)\bin\uic topicchooser.ui -i topicchooser.h -o topicchooser.cpp \
	$(QTDIR)\bin\moc topicchooser.h -o moc_topicchooser.cpp \
	

"topicchooser.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"topicchooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"moc_topicchooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
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

SOURCE=.\finddialog.cpp
# End Source File
# Begin Source File

SOURCE=.\finddialog.h
# End Source File
# Begin Source File

SOURCE=.\helpdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\helpdialog.h
# End Source File
# Begin Source File

SOURCE=.\mainwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\mainwindow.h
# End Source File
# Begin Source File

SOURCE=.\moc_assistant.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_finddialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_helpdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_helpdialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_helpwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_mainwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_settingsdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_settingsdialogimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_topicchooser.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_topicchooserimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\settingsdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\settingsdialog.h
# End Source File
# Begin Source File

SOURCE=.\topicchooser.cpp
# End Source File
# Begin Source File

SOURCE=.\topicchooser.h
# End Source File
# End Group
# End Target
# End Project
