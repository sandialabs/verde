# Microsoft Developer Studio Project File - Name="verde_gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=verde_gui - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "verde_gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "verde_gui.mak" CFG="verde_gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "verde_gui - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "verde_gui - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
# ADD CPP -MD /W3 /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D UNICODE /D QT_CLEAN_NAMESPACE /D QT_DLL /D QT_THREAD_SUPPORT /FD /c -nologo -Zm200 -GX -O1
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo 

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD CPP -MDd /W3 /Gm /GZ /ZI /Od /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D UNICODE /D QT_CLEAN_NAMESPACE /D QT_DLL /D QT_THREAD_SUPPORT /FD /c -nologo -Zm200 -GX  -Zi
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo 

!ENDIF 

# Begin Target

# Name "verde_gui - Win32 Release"
# Name "verde_gui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=DrawingTool.cpp
# End Source File
# Begin Source File

SOURCE=GUICommandHandler.cpp
# End Source File
# Begin Source File

SOURCE=GLInputWidget.cpp
# End Source File
# Begin Source File

SOURCE=GLWindow.cpp
# End Source File
# Begin Source File

SOURCE=GUImain.cpp
# End Source File
# Begin Source File

SOURCE=GUITextBox.cpp
# End Source File
# Begin Source File

SOURCE=UserEventHandler.cpp
# End Source File
# Begin Source File

SOURCE=OpenMeshFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=PickTool.cpp
# End Source File
# Begin Source File

SOURCE=GLTextTool.cpp
# End Source File
# Begin Source File

SOURCE=VerdeGUIApp.cpp
# End Source File
# Begin Source File

SOURCE=verde.ui.h
# End Source File
# Begin Source File

SOURCE=MetricRangeDialog.ui.h
# End Source File
# Begin Source File

SOURCE=ModelResultsTreeView.ui.h
# End Source File
# Begin Source File

SOURCE=ModelTreeView.ui.h
# End Source File
# Begin Source File

SOURCE=Preferences.ui.h
# End Source File
# Begin Source File

SOURCE=BlockImport.ui.h
# End Source File
# Begin Source File

SOURCE=AboutBox.ui.h
# End Source File
# Begin Source File

SOURCE=MouseMap.ui.h
# End Source File

# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=DrawingTool.hpp

# End Source File
# Begin Source File

SOURCE=GUICommandHandler.hpp

# End Source File
# Begin Source File

SOURCE=GLInputWidget.hpp

# End Source File
# Begin Source File

SOURCE=GLWindow.hpp

# End Source File
# Begin Source File

SOURCE=GUITextBox.hpp

# End Source File
# Begin Source File

SOURCE=UserEventHandler.hpp

# End Source File
# Begin Source File

SOURCE=OpenMeshFileDialog.hpp

# End Source File
# Begin Source File

SOURCE=PickTool.hpp

# End Source File
# Begin Source File

SOURCE=GLTextTool.hpp

# End Source File
# Begin Source File

SOURCE=VerdeGUIApp.hpp

# End Source File

# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group

# Begin Group "Forms"
# Prop Default_Filter "ui"
# Begin Source File

SOURCE=verde.ui
USERDEP_verde.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing verde.ui...
InputPath=.\verde.ui

BuildCmds= \
	$(QTDIR)\bin\uic verde.ui -o verde.h \
	$(QTDIR)\bin\uic verde.ui -i verde.h -o verde.cpp \
	$(QTDIR)\bin\moc verde.h -o moc_verde.cpp \

"verde.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"verde.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_verde.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing verde.ui...
InputPath=.\verde.ui

BuildCmds= \
	$(QTDIR)\bin\uic verde.ui -o verde.h \
	$(QTDIR)\bin\uic verde.ui -i verde.h -o verde.cpp \
	$(QTDIR)\bin\moc verde.h -o moc_verde.cpp \

"verde.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"verde.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_verde.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=MetricRangeDialog.ui
USERDEP_MetricRangeDialog.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing MetricRangeDialog.ui...
InputPath=.\MetricRangeDialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic MetricRangeDialog.ui -o MetricRangeDialog.h \
	$(QTDIR)\bin\uic MetricRangeDialog.ui -i MetricRangeDialog.h -o MetricRangeDialog.cpp \
	$(QTDIR)\bin\moc MetricRangeDialog.h -o moc_MetricRangeDialog.cpp \

"MetricRangeDialog.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"MetricRangeDialog.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_MetricRangeDialog.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing MetricRangeDialog.ui...
InputPath=.\MetricRangeDialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic MetricRangeDialog.ui -o MetricRangeDialog.h \
	$(QTDIR)\bin\uic MetricRangeDialog.ui -i MetricRangeDialog.h -o MetricRangeDialog.cpp \
	$(QTDIR)\bin\moc MetricRangeDialog.h -o moc_MetricRangeDialog.cpp \

"MetricRangeDialog.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"MetricRangeDialog.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_MetricRangeDialog.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=ModelResultsTreeView.ui
USERDEP_ModelResultsTreeView.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing ModelResultsTreeView.ui...
InputPath=.\ModelResultsTreeView.ui

BuildCmds= \
	$(QTDIR)\bin\uic ModelResultsTreeView.ui -o ModelResultsTreeView.h \
	$(QTDIR)\bin\uic ModelResultsTreeView.ui -i ModelResultsTreeView.h -o ModelResultsTreeView.cpp \
	$(QTDIR)\bin\moc ModelResultsTreeView.h -o moc_ModelResultsTreeView.cpp \

"ModelResultsTreeView.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"ModelResultsTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_ModelResultsTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing ModelResultsTreeView.ui...
InputPath=.\ModelResultsTreeView.ui

BuildCmds= \
	$(QTDIR)\bin\uic ModelResultsTreeView.ui -o ModelResultsTreeView.h \
	$(QTDIR)\bin\uic ModelResultsTreeView.ui -i ModelResultsTreeView.h -o ModelResultsTreeView.cpp \
	$(QTDIR)\bin\moc ModelResultsTreeView.h -o moc_ModelResultsTreeView.cpp \

"ModelResultsTreeView.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"ModelResultsTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_ModelResultsTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=ModelTreeView.ui
USERDEP_ModelTreeView.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing ModelTreeView.ui...
InputPath=.\ModelTreeView.ui

BuildCmds= \
	$(QTDIR)\bin\uic ModelTreeView.ui -o ModelTreeView.h \
	$(QTDIR)\bin\uic ModelTreeView.ui -i ModelTreeView.h -o ModelTreeView.cpp \
	$(QTDIR)\bin\moc ModelTreeView.h -o moc_ModelTreeView.cpp \

"ModelTreeView.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"ModelTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_ModelTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing ModelTreeView.ui...
InputPath=.\ModelTreeView.ui

BuildCmds= \
	$(QTDIR)\bin\uic ModelTreeView.ui -o ModelTreeView.h \
	$(QTDIR)\bin\uic ModelTreeView.ui -i ModelTreeView.h -o ModelTreeView.cpp \
	$(QTDIR)\bin\moc ModelTreeView.h -o moc_ModelTreeView.cpp \

"ModelTreeView.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"ModelTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_ModelTreeView.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=Preferences.ui
USERDEP_Preferences.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing Preferences.ui...
InputPath=.\Preferences.ui

BuildCmds= \
	$(QTDIR)\bin\uic Preferences.ui -o Preferences.h \
	$(QTDIR)\bin\uic Preferences.ui -i Preferences.h -o Preferences.cpp \
	$(QTDIR)\bin\moc Preferences.h -o moc_Preferences.cpp \

"Preferences.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"Preferences.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_Preferences.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing Preferences.ui...
InputPath=.\Preferences.ui

BuildCmds= \
	$(QTDIR)\bin\uic Preferences.ui -o Preferences.h \
	$(QTDIR)\bin\uic Preferences.ui -i Preferences.h -o Preferences.cpp \
	$(QTDIR)\bin\moc Preferences.h -o moc_Preferences.cpp \

"Preferences.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"Preferences.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_Preferences.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=BlockImport.ui
USERDEP_BlockImport.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing BlockImport.ui...
InputPath=.\BlockImport.ui

BuildCmds= \
	$(QTDIR)\bin\uic BlockImport.ui -o BlockImport.h \
	$(QTDIR)\bin\uic BlockImport.ui -i BlockImport.h -o BlockImport.cpp \
	$(QTDIR)\bin\moc BlockImport.h -o moc_BlockImport.cpp \

"BlockImport.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"BlockImport.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_BlockImport.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing BlockImport.ui...
InputPath=.\BlockImport.ui

BuildCmds= \
	$(QTDIR)\bin\uic BlockImport.ui -o BlockImport.h \
	$(QTDIR)\bin\uic BlockImport.ui -i BlockImport.h -o BlockImport.cpp \
	$(QTDIR)\bin\moc BlockImport.h -o moc_BlockImport.cpp \

"BlockImport.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"BlockImport.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_BlockImport.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=AboutBox.ui
USERDEP_AboutBox.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing AboutBox.ui...
InputPath=.\AboutBox.ui

BuildCmds= \
	$(QTDIR)\bin\uic AboutBox.ui -o AboutBox.h \
	$(QTDIR)\bin\uic AboutBox.ui -i AboutBox.h -o AboutBox.cpp \
	$(QTDIR)\bin\moc AboutBox.h -o moc_AboutBox.cpp \

"AboutBox.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"AboutBox.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_AboutBox.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing AboutBox.ui...
InputPath=.\AboutBox.ui

BuildCmds= \
	$(QTDIR)\bin\uic AboutBox.ui -o AboutBox.h \
	$(QTDIR)\bin\uic AboutBox.ui -i AboutBox.h -o AboutBox.cpp \
	$(QTDIR)\bin\moc AboutBox.h -o moc_AboutBox.cpp \

"AboutBox.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"AboutBox.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_AboutBox.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=MouseMap.ui
USERDEP_MouseMap.ui="$(QTDIR)\bin\moc.exe" "$(QTDIR)\bin\uic.exe"

!IF  "$(CFG)" == "verde_gui - Win32 Release"

# Begin Custom Build - Uic'ing MouseMap.ui...
InputPath=.\MouseMap.ui

BuildCmds= \
	$(QTDIR)\bin\uic MouseMap.ui -o MouseMap.h \
	$(QTDIR)\bin\uic MouseMap.ui -i MouseMap.h -o MouseMap.cpp \
	$(QTDIR)\bin\moc MouseMap.h -o moc_MouseMap.cpp \

"MouseMap.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"MouseMap.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_MouseMap.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ELSEIF  "$(CFG)" == "verde_gui - Win32 Debug"

# Begin Custom Build - Uic'ing MouseMap.ui...
InputPath=.\MouseMap.ui

BuildCmds= \
	$(QTDIR)\bin\uic MouseMap.ui -o MouseMap.h \
	$(QTDIR)\bin\uic MouseMap.ui -i MouseMap.h -o MouseMap.cpp \
	$(QTDIR)\bin\moc MouseMap.h -o moc_MouseMap.cpp \

"MouseMap.h" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"MouseMap.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

"moc_MouseMap.cpp" : "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	$(BuildCmds)

# End Custom Build

!ENDIF 

# End Source File

# End Group






# Begin Group "Generated"
# Begin Source File

SOURCE=moc_verde.cpp
# End Source File
# Begin Source File

SOURCE=moc_MetricRangeDialog.cpp
# End Source File
# Begin Source File

SOURCE=moc_ModelResultsTreeView.cpp
# End Source File
# Begin Source File

SOURCE=moc_ModelTreeView.cpp
# End Source File
# Begin Source File

SOURCE=moc_Preferences.cpp
# End Source File
# Begin Source File

SOURCE=moc_BlockImport.cpp
# End Source File
# Begin Source File

SOURCE=moc_AboutBox.cpp
# End Source File
# Begin Source File

SOURCE=moc_MouseMap.cpp
# End Source File

# Begin Source File

SOURCE=verde.cpp
# End Source File
# Begin Source File

SOURCE=MetricRangeDialog.cpp
# End Source File
# Begin Source File

SOURCE=ModelResultsTreeView.cpp
# End Source File
# Begin Source File

SOURCE=ModelTreeView.cpp
# End Source File
# Begin Source File

SOURCE=Preferences.cpp
# End Source File
# Begin Source File

SOURCE=BlockImport.cpp
# End Source File
# Begin Source File

SOURCE=AboutBox.cpp
# End Source File
# Begin Source File

SOURCE=MouseMap.cpp
# End Source File

# Begin Source File

SOURCE=verde.h
# End Source File
# Begin Source File

SOURCE=MetricRangeDialog.h
# End Source File
# Begin Source File

SOURCE=ModelResultsTreeView.h
# End Source File
# Begin Source File

SOURCE=ModelTreeView.h
# End Source File
# Begin Source File

SOURCE=Preferences.h
# End Source File
# Begin Source File

SOURCE=BlockImport.h
# End Source File
# Begin Source File

SOURCE=AboutBox.h
# End Source File
# Begin Source File

SOURCE=MouseMap.h
# End Source File



# Prop Default_Filter "moc"
# End Group
# End Target
# End Project

