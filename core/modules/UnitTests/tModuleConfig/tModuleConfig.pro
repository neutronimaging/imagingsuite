#-------------------------------------------------
#
# Project created by QtCreator 2014-11-17T07:54:02
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tModuleConfig
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

TEMPLATE = app

unix {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2

        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    LIBS += -L/usr/local/lib64

    LIBS += -ltiff -lxml2 #-lNeXus -lNeXusCPP
    INCLUDEPATH += /usr/include/libxml2
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += $$PWD/../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH += $$PWD/../../../../../ExternalDependencies/windows/include/cfitsio
    LIBPATH     += $$PWD/../../../../../ExternalDependencies/windows/lib

    INCLUDEPATH += $$PWD/../../../../external/include
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64


    LIBS += -llibxml2
    LIBS += -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += tst_configbasetest.cpp \
           dummyconfig.cpp

HEADERS += dummyconfig.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug

LIBS += -lkipl -lModuleConfig -lReconFramework -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../kipl/kipl/include
DEPENDPATH += $$PWD/../../../kipl/kipl/src

INCLUDEPATH += $$PWD/../../../../frameworks/tomography/Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../frameworks/tomography/Framework/ReconFramework/include

INCLUDEPATH += $$PWD/../../ModuleConfig/include
DEPENDPATH += $$PWD/../../ModuleConfig/src

INCLUDEPATH += $$PWD/../../../algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../algorithms/ImagingAlgorithms/include
