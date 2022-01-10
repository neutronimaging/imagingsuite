#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T17:40:10
#
#-------------------------------------------------

QT       += core widgets

TARGET = InspectorModules
TEMPLATE = lib
CONFIG += c++11

DEFINES += INSPECTORMODULES_LIBRARY

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

SOURCES += \
    CountNANs.cpp \
    InspectorModules.cpp \
    ProjectionInspector.cpp \
    SaveProjections.cpp \
    getimagesize.cpp

HEADERS +=\
        inspectormodules_global.h \
    CountNANs.h \
    InspectorModules.h \
    ProjectionInspector.h \
    SaveProjections.h \
    getimagesize.h

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
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
        INCLUDEPATH += /usr/include/cfitsio
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
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64
    LIBS += -llibxml2
    LIBS += -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug

LIBS += -lkipl -lModuleConfig -lReconFramework -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../core//algorithms/ImagingAlgorithms/src

macx: {

INCLUDEPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../external/mac/include/nexus/ $$PWD/../../../../external/mac/include/hdf5
DEPENDPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../external/mac/include/nexus/ $$PWD/../../../../external/mac/include/hdf5

LIBS += -L$$PWD/../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}
