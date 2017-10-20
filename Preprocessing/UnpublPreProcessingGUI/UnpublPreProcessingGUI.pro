#-------------------------------------------------
#
# Project created by QtCreator 2017-10-20T14:01:40
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = UnpublPreProcessingGUI
TEMPLATE = lib

DEFINES += UNPUBLPREPROCESSINGGUI_LIBRARY

SOURCES += unpublpreprocessinggui.cpp

HEADERS += unpublpreprocessinggui.h\
        unpublpreprocessinggui_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
