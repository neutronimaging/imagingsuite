#-------------------------------------------------
#
# Project created by QtCreator 2016-09-17T18:13:04
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tkipl_iotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

unix {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
#        QMAKE_MAC_SDK = macosx10.11
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
    LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../external/src/linalg ../../../../external/include ../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2

    LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
}

SOURCES += tst_tkipl_iotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release) LIBS += -L$$PWD/../../../../lib/ -lkipl
else:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../../lib/debug/ -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/include
