TARGET	= ../verde_help
PROJECTNAME = VerdeHelp
FORMS = mainwindow.ui topicchooser.ui finddialog.ui helpdialog.ui settingsdialog.ui 
TEMPLATE = app
CONFIG += qt warn_on opengl verde_HELP

include(../platforms.pro.inc)
include(../verde.local)

SOURCES	+= main.cpp helpwindow.cpp topicchooserimpl.cpp helpdialogimpl.cpp settingsdialogimpl.cpp assistant.cpp structureparser.cpp textbrowser.cpp

HEADERS	+= helpwindow.h topicchooserimpl.h helpdialogimpl.h settingsdialogimpl.h assistant.h structureparser.h textbrowser.h

