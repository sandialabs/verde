
CONFIG	+= qt warn_on staticlib opengl verde_GUI
TEMPLATE	=lib
TARGET	    = verde_gui
PROJECTNAME = verde_gui
DEFINES += QT_CLEAN_NAMESPACE
DEPENDPATH += ../

include(../platforms.pro.inc)
include(../verde.local)

SOURCES	+= DrawingTool.cpp GUICommandHandler.cpp GLInputWidget.cpp GLWindow.cpp GUImain.cpp GUITextBox.cpp UserEventHandler.cpp OpenMeshFileDialog.cpp PickTool.cpp GLTextTool.cpp VerdeGUIApp.cpp
HEADERS	+= DrawingTool.hpp GUICommandHandler.hpp GLInputWidget.hpp GLWindow.hpp GUITextBox.hpp UserEventHandler.hpp OpenMeshFileDialog.hpp PickTool.hpp GLTextTool.hpp VerdeGUIApp.hpp
FORMS	= verde.ui MetricRangeDialog.ui ModelResultsTreeView.ui ModelTreeView.ui Preferences.ui BlockImport.ui AboutBox.ui MouseMap.ui
