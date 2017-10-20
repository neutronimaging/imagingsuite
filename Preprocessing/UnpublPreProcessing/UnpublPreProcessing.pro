#-------------------------------------------------
#
# Project created by QtCreator 2017-10-20T13:58:53
#
#-------------------------------------------------

QT       -= gui

TARGET = UnpublPreProcessing
TEMPLATE = lib

DEFINES += UNPUBLPREPROCESSING_LIBRARY

SOURCES += unpublpreprocessing.cpp

HEADERS += unpublpreprocessing.h\
        unpublpreprocessing_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
