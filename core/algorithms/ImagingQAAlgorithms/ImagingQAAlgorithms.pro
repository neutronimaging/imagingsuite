#-------------------------------------------------
#
# Project created by QtCreator 2017-10-17T16:14:26
#
#-------------------------------------------------

QT       -= gui

TARGET = ImagingQAAlgorithms
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../repos/lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../repos/lib/debug

DEFINES += IMAGINGQAALGORITHMS_LIBRARY

SOURCES += imagingqaalgorithms.cpp \
    ballanalysis.cpp \
    collimationestimator.cpp \
    resolutionestimators.cpp

HEADERS += imagingqaalgorithms.h\
        imagingqaalgorithms_global.h \
    ballanalysis.h \
    collimationestimator.h \
    resolutionestimators.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += $$PWD/../../../../../repos/kipl/trunk/kipl/include
