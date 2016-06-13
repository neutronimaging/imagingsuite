#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T17:53:09
#
#-------------------------------------------------

QT       -= core gui

TARGET = GenericPreProc
TEMPLATE = lib

DEFINES += GENERICPREPROC_LIBRARY

SOURCES += genericpreproc.cpp

HEADERS += genericpreproc.h\
        genericpreproc_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
