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
        INCLUDEPATH  += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
        INCLUDEPATH  += /opt/local/include/libxml2
    }

    LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
}

unix:mac {
exists($$PWD/../../../../external/mac/lib/*NeXus*) {

    message("-lNeXus exists")
    DEFINES *= HAVE_NEXUS

    INCLUDEPATH += $$PWD/../../../../external/mac/include/ $$PWD/../../../../external/mac/include/nexus $$PWD/../../../../external/mac/include/hdf5
    DEPENDPATH += $$PWD/../../../../external/mac/include/ $$PWD/../../../../external/mac/include/nexus $$PWD/../../../../external/mac/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../../external/mac/lib/

    LIBS += -lNeXus.1.0.0 -lNeXusCPP.1.0.0


}
else {
message("-lNeXus does not exist $$HEADERS")
}

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../external/src/linalg $$PWD/../../../../external/include $$PWD/../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$PWD/../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2

    LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
}


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lReconFramework

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/src
