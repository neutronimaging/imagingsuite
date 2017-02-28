#-------------------------------------------------
#
# Project created by QtCreator 2017-01-17T16:23:18
#
#-------------------------------------------------

QT       -= gui

CONFIG += c++11

TARGET = UnpublishedModules
TEMPLATE = lib

DEFINES += UNPUBLISHEDMODULES_LIBRARY

SOURCES += unpublishedmodules.cpp

HEADERS += unpublishedmodules.h\
        unpublishedmodules_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
