# Microsoft Developer Studio Project File - Name="verde" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=verde - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "verde.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "verde.mak" CFG="verde - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "verde - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "verde - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "verde - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\GUI" /I "..\verdict" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "NT" /D "YY_NEVER_INTERACTIVE" /D NAME=\"verde\" /D "VERDE_GUI" /D "QT_DLL" /D "USE_VERDICT" /YX /FD /c
# SUBTRACT CPP /Z<none>
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 verdict100.lib  netcdfs.lib libexoIIv2c.lib msvcrt.lib msvcprt.lib $(QTDIR)\lib\qt-mt304.lib $(QTDIR)\lib\qtmain.lib oldnames.lib advapi32.lib opengl32.lib glu32.lib kernel32.lib gdi32.lib user32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../verde.exe" /libpath:"..\\"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=copy  ..\verdict\Release\verdict100.lib  ..\ 	copy  ..\verdict\Release\verdict100.dll  ..\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "verde - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /I "..\GUI" /I "..\verdict" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "NT" /D "YY_NEVER_INTERACTIVE" /D NAME=\"verde\" /D "VERDE_GUI" /D "QT_DLL" /D "USE_VERDICT" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 verdict100d.lib netcdfsd.lib libexoIIv2c_db.lib msvcrtd.lib msvcprtd.lib $(QTDIR)\lib\qt-mt304.lib $(QTDIR)\lib\qtmain.lib oldnames.lib advapi32.lib opengl32.lib glu32.lib kernel32.lib gdi32.lib user32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../verde.exe" /pdbtype:sept /libpath:"..\\"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=copy  ..\verdict\Debug\verdict100d.dll  ..\ 	copy  ..\verdict\Debug\verdict100d.lib  ..\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "verde - Win32 Release"
# Name "verde - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\AllocMemManagers.cpp
# End Source File
# Begin Source File

SOURCE=..\aprepro_init.cpp
# End Source File
# Begin Source File

SOURCE=..\ArrayBasedContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\BoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=..\BucketSort.cpp
# End Source File
# Begin Source File

SOURCE=..\builtin.cpp
# End Source File
# Begin Source File

SOURCE=..\CommandHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\CpuTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\EdgeElem.cpp
# End Source File
# Begin Source File

SOURCE=..\EdgeMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\EdgeRef.cpp
# End Source File
# Begin Source File

SOURCE=..\EdgeTool.cpp
# End Source File
# Begin Source File

SOURCE=..\ElementBC.cpp
# End Source File
# Begin Source File

SOURCE=..\ElementBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\ElemRef.cpp
# End Source File
# Begin Source File

SOURCE=..\ErrorTool.cpp
# End Source File
# Begin Source File

SOURCE=..\EulerTool.cpp
# End Source File
# Begin Source File

SOURCE=..\ExodusTool.cpp
# End Source File
# Begin Source File

SOURCE=..\GetLongOpt.cpp
# End Source File
# Begin Source File

SOURCE=..\GridSearch.cpp
# End Source File
# Begin Source File

SOURCE=..\HexMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\HexRef.cpp
# End Source File
# Begin Source File

SOURCE=..\idr.cpp
# End Source File
# Begin Source File

SOURCE=..\idr_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\KnifeMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\KnifeRef.cpp
# End Source File
# Begin Source File

SOURCE=..\MDBClient.cpp
# End Source File
# Begin Source File

SOURCE=..\MemoryBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\MemoryManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=..\MeshContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\Metric.cpp
# End Source File
# Begin Source File

SOURCE=..\NodeBC.cpp
# End Source File
# Begin Source File

SOURCE=..\NodeRef.cpp
# End Source File
# Begin Source File

SOURCE=..\PyramidMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\PyramidRef.cpp
# End Source File
# Begin Source File

SOURCE=..\QuadElem.cpp
# End Source File
# Begin Source File

SOURCE=..\QuadMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\QuadRef.cpp
# End Source File
# Begin Source File

SOURCE=..\QualitySummary.cpp
# End Source File
# Begin Source File

SOURCE=..\SkinTool.cpp
# End Source File
# Begin Source File

SOURCE=..\TetMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\TetRef.cpp
# End Source File
# Begin Source File

SOURCE=..\TriElem.cpp
# End Source File
# Begin Source File

SOURCE=..\TriMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\TriRef.cpp
# End Source File
# Begin Source File

SOURCE=..\UserInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\VerdeApp.cpp
# End Source File
# Begin Source File

SOURCE=..\VerdeMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\VerdeMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\VerdeString.cpp
# End Source File
# Begin Source File

SOURCE=..\VerdeUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\VerdeVector.cpp
# End Source File
# Begin Source File

SOURCE=..\VerdeVersion.h
# End Source File
# Begin Source File

SOURCE=..\WedgeMetric.cpp
# End Source File
# Begin Source File

SOURCE=..\WedgeRef.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\AllocMemManagers.hpp
# End Source File
# Begin Source File

SOURCE=..\allwords.h
# End Source File
# Begin Source File

SOURCE=..\aprepro_init.hpp
# End Source File
# Begin Source File

SOURCE=..\ArrayBasedContainer.hpp
# End Source File
# Begin Source File

SOURCE=..\BoundingBox.hpp
# End Source File
# Begin Source File

SOURCE=..\BucketSort.hpp
# End Source File
# Begin Source File

SOURCE=..\builtin.h
# End Source File
# Begin Source File

SOURCE=..\builtin.hpp
# End Source File
# Begin Source File

SOURCE=..\CommandHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\CpuTimer.hpp
# End Source File
# Begin Source File

SOURCE=..\defines.h
# End Source File
# Begin Source File

SOURCE=..\DLFilePtrList.hpp
# End Source File
# Begin Source File

SOURCE=..\DLIList.hpp
# End Source File
# Begin Source File

SOURCE=..\EdgeElem.hpp
# End Source File
# Begin Source File

SOURCE=..\EdgeMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\EdgeRef.hpp
# End Source File
# Begin Source File

SOURCE=..\EdgeTool.hpp
# End Source File
# Begin Source File

SOURCE=..\ElementBC.hpp
# End Source File
# Begin Source File

SOURCE=..\ElementBlock.hpp
# End Source File
# Begin Source File

SOURCE=..\ElemRef.hpp
# End Source File
# Begin Source File

SOURCE=..\ErrorTool.hpp
# End Source File
# Begin Source File

SOURCE=..\EulerTool.hpp
# End Source File
# Begin Source File

SOURCE=..\ExodusTool.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedEdge.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedElem.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedHex.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedKnife.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedPyramid.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedQuad.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedTet.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedTri.hpp
# End Source File
# Begin Source File

SOURCE=..\FailedWedge.hpp
# End Source File
# Begin Source File

SOURCE=..\FullMeshData.hpp
# End Source File
# Begin Source File

SOURCE=..\getline.hpp
# End Source File
# Begin Source File

SOURCE=..\GetLongOpt.hpp
# End Source File
# Begin Source File

SOURCE=..\GridSearch.hpp
# End Source File
# Begin Source File

SOURCE=..\HexMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\HexRef.hpp
# End Source File
# Begin Source File

SOURCE=..\idr.hpp
# End Source File
# Begin Source File

SOURCE=..\idr_parser.h
# End Source File
# Begin Source File

SOURCE=..\idr_types.h
# End Source File
# Begin Source File

SOURCE=..\keywords.h
# End Source File
# Begin Source File

SOURCE=..\keywordtable.h
# End Source File
# Begin Source File

SOURCE=..\KnifeMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\KnifeRef.hpp
# End Source File
# Begin Source File

SOURCE=..\main.hpp
# End Source File
# Begin Source File

SOURCE=..\MDBClient.hpp
# End Source File
# Begin Source File

SOURCE=..\MDBInterface.hpp
# End Source File
# Begin Source File

SOURCE=..\MDBVerde.hpp
# End Source File
# Begin Source File

SOURCE=..\MemoryBlock.hpp
# End Source File
# Begin Source File

SOURCE=..\MemoryManager.hpp
# End Source File
# Begin Source File

SOURCE=..\Mesh.hpp
# End Source File
# Begin Source File

SOURCE=..\MeshContainer.hpp
# End Source File
# Begin Source File

SOURCE=..\Metric.hpp
# End Source File
# Begin Source File

SOURCE=..\NodeBC.hpp
# End Source File
# Begin Source File

SOURCE=..\NodeRef.hpp
# End Source File
# Begin Source File

SOURCE=..\ParsingDefines.hpp
# End Source File
# Begin Source File

SOURCE=..\PyramidMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\PyramidRef.hpp
# End Source File
# Begin Source File

SOURCE=..\QuadElem.hpp
# End Source File
# Begin Source File

SOURCE=..\QuadMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\QuadRef.hpp
# End Source File
# Begin Source File

SOURCE=..\QualitySummary.hpp
# End Source File
# Begin Source File

SOURCE=..\SkinTool.hpp
# End Source File
# Begin Source File

SOURCE=..\TetMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\TetRef.hpp
# End Source File
# Begin Source File

SOURCE=..\TriElem.hpp
# End Source File
# Begin Source File

SOURCE=..\TriMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\TriRef.hpp
# End Source File
# Begin Source File

SOURCE=..\UserInterface.hpp
# End Source File
# Begin Source File

SOURCE=..\VerdeApp.hpp
# End Source File
# Begin Source File

SOURCE=..\VerdeDefines.hpp
# End Source File
# Begin Source File

SOURCE=..\VerdeMatrix.hpp
# End Source File
# Begin Source File

SOURCE=..\VerdeMessage.hpp
# End Source File
# Begin Source File

SOURCE=..\VerdeString.hpp
# End Source File
# Begin Source File

SOURCE=..\VerdeUtil.hpp
# End Source File
# Begin Source File

SOURCE=..\VerdeVector.hpp
# End Source File
# Begin Source File

SOURCE=..\WedgeMetric.hpp
# End Source File
# Begin Source File

SOURCE=..\WedgeRef.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
