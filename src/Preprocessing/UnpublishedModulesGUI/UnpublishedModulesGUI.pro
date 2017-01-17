#-------------------------------------------------
#
# Project created by QtCreator 2017-01-17T16:34:53
#
#-------------------------------------------------

QT       += widgets

TARGET = UnpublishedModulesGUI
TEMPLATE = lib

DEFINES += UNPUBLISHEDMODULESGUI_LIBRARY

SOURCES += unpublishedmodulesgui.cpp

HEADERS += unpublishedmodulesgui.h\
        unpublishedmodulesgui_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
