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

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

TEMPLATE = app

unix:!symbian {
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
        QMAKE_LIBDIR += /usr/local/lib
        QMAKE_INFO_PLIST = Info.plist
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    LIBS += -ltiff -lxml2
    INCLUDEPATH += /usr/include/libxml2
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../external/src/linalg $$PWD/../../../../external/include $$PWD/../../../../external/include/cfitsio $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += tst_configbasetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../../lib -lkipl -lModuleConfig -lReconFramework
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug -lkipl -lModuleConfig -lReconFramework


INCLUDEPATH += $$PWD/../../../kipl/kipl/include
DEPENDPATH += $$PWD/../../../kipl/kipl/src

INCLUDEPATH += $$PWD/../../../../frameworks/tomography/Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../frameworks/tomography/Framework/ReconFramework/include

INCLUDEPATH += $$PWD/../../ModuleConfig/include
DEPENDPATH += $$PWD/../../ModuleConfig/src
