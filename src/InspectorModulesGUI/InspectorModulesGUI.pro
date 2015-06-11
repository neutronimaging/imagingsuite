#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T18:05:46
#
#-------------------------------------------------

QT       += widgets

TARGET = InspectorModulesGUI
TEMPLATE = lib

DEFINES += INSPECTORMODULESGUI_LIBRARY

SOURCES += inspectormodulesgui.cpp \
    saveprojectionsdlg.cpp

HEADERS += inspectormodulesgui.h\
        inspectormodulesgui_global.h \
    saveprojectionsdlg.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    saveprojectionsdlg.ui
