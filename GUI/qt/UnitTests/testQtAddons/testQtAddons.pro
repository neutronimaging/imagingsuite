#-------------------------------------------------
#
# Project created by QtCreator 2017-03-10T07:22:35
#
#-------------------------------------------------

QT       += widgets testlib

TARGET = tst_qtaddons
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

SOURCES += tst_qtaddons.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

unix {
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }

    LIBS +=  -lm -lz   -ltiff  -lcfitsio
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include
    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include/cfitsio
    QMAKE_LIBDIR += $$PWD/../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH += $$PWD/../../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../../external/lib64

    LIBS += -llibxml2
    LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi

    QMAKE_CXXFLAGS += /openmp /O2
}


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lQtAddons

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/src

INCLUDEPATH += $$PWD/../../QtAddons
DEPENDPATH += $$PWD/../../QtAddons
