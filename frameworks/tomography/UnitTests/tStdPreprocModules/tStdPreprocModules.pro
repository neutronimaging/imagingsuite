#-------------------------------------------------
#
# Project created by QtCreator 2017-01-19T17:30:32
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tstdpreprocmodulestest
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

TEMPLATE = app

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

SOURCES += tst_tstdpreprocmodulestest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


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
        QMAKE_LIBDIR += /opt/local/lib

        INCLUDEPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
        DEPENDPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
        LIBS += -L$$PWD/../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include
    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include/cfitsio
    QMAKE_LIBDIR += $$PWD/../../../../../ExternalDependencies/windows/lib

    INCLUDEPATH  += $$PWD/../../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lQtModuleConfigure -lQtAddons -lImagingAlgorithms -lReconFramework -lModuleConfig -lStdPreprocModules

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/include

INCLUDEPATH += $$PWD/../../Preprocessing/StdPreprocModules/include
DEPENDPATH += $$PWD/../../Preprocessing/StdPreprocModules/include

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../core/modules/ModuleConfig/include

