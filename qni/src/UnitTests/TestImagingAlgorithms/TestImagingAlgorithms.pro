#-------------------------------------------------
#
# Project created by QtCreator 2013-10-24T17:17:21
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_ImagingAlgorithms
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

TEMPLATE = app

unix {
    INCLUDEPATH += ../../../../../external/src/linalg
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
     #   QMAKE_MAC_SDK = macosx10.11
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }

    LIBS +=  -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../external/src/linalg ../../../../external/include ../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += \
    tst_testImagingAlgorithms.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/ -lkipl -lImagingAlgorithms
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lkipl -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/src

