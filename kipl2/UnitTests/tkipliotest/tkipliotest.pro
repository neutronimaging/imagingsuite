#-------------------------------------------------
#
# Project created by QtCreator 2016-11-11T17:31:26
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_kipliotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_kipliotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

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
        QMAKE_LIBDIR += -L/opt/usr/lib
    }

    unix:macx {
    #    QMAKE_MAC_SDK = macosx10.12
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
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

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:symbian: LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
else:unix: LIBS +=  -lm -lz   -ltiff  -lcfitsio

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/src
