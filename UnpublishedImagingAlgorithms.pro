#-------------------------------------------------
#
# Project created by QtCreator 2017-10-20T13:51:08
#
#-------------------------------------------------

QT       -= gui

TARGET = UnpublishedImagingAlgorithms
TEMPLATE = lib


CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

DEFINES += UNPUBLISHEDIMAGINGALGORITHMS_LIBRARY

SOURCES += ReferenceImageCorrection.cpp\
            detectorlagcorrection.cpp

HEADERS += ReferenceImageCorrection.h\
            detectorlagcorrection.h\
            unpublishedimagingalgorithms_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib/debug/

LIBS += -lkipl -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../imagingsuite/external/src/linalg
INCLUDEPATH += $$PWD/../../../../imagingsuite/external/include

INCLUDEPATH += $$PWD/../../../../imagingsuite/core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../imagingsuite/core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../imagingsuite/core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../imagingsuite/core/algorithms/ImagingAlgorithms/include

