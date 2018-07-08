#-------------------------------------------------
#
# Project created by QtCreator 2016-11-11T17:53:54
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_framework
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_frameworktest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

unix {
    INCLUDEPATH += "../../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        QMAKE_LIBDIR += -L/opt/usr/lib
    }

    unix:macx {
    #    QMAKE_MAC_SDK = macosx10.12
        INCLUDEPATH  += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
        INCLUDEPATH  += /opt/local/include/libxml2
    }
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += $$PWD/../../../../external/src/linalg
    INCLUDEPATH += $$PWD/../../../../external/include
    INCLUDEPATH += $$PWD/../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS +=  -llibxml2_dll
    QMAKE_CXXFLAGS += /openmp /O2
}

win32 {
exists($$PWD/../../../../../../external/lib64/nexus/*NeXus*) {

    message("-lNeXus exists")
    DEFINES *= HAVE_NEXUS
    INCLUDEPATH += $$PWD/../../../../../../external/include/nexus $$PWD/../../../../../../external/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../../../../external/lib64/nexus $$PWD/../../../../../../external/lib64/hdf5

    LIBS += -lNeXus -lNeXusCPP

#    SOURCES += $$PWD/../../../../../../core/kipl/kipl/src/io/io_nexus.cpp
#    HEADERS += $$PWD/../../../../../../core/kipl/kipl/include/io/io_nexus.h
}
}

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:symbian: LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
else:unix: LIBS +=  -lm -lz   -ltiff  -lcfitsio

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lReconFramework

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/src
