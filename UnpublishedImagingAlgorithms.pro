#-------------------------------------------------
#
# Project created by QtCreator 2017-10-20T13:51:08
#
#-------------------------------------------------

QT       -= gui

TARGET = UnpublishedImagingAlgorithms
TEMPLATE = lib

DEFINES += UNPUBLISHEDIMAGINGALGORITHMS_LIBRARY

SOURCES += unpublishedimagingalgorithms.cpp

HEADERS += unpublishedimagingalgorithms.h\
        unpublishedimagingalgorithms_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
