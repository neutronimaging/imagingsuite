#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T17:53:09
#
#-------------------------------------------------
#QT += core
QT       -= gui
CONFIG += c++11
TARGET = GenericPreProc
TEMPLATE = lib

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

DEFINES += GENERICPREPROC_LIBRARY

SOURCES += genericpreproc.cpp \
    PreprocModule.cpp

HEADERS += genericpreproc.h\
        genericpreproc_global.h \
    PreprocModule.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

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
    }


    QMAKE_LFLAGS += -ltiff -lxml2 -lfftw3

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../external/include
    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
    DEFINES += NOMINMAX
}

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../core/modules/trunk/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/src

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lImagingAlgorithms -lReconFramework

macx: {
    INCLUDEPATH += $$PWD/../../../../external/mac/include
    INCLUDEPATH += $$PWD/../../../../external/mac/include/hdf5
    INCLUDEPATH += $$PWD/../../../../external/mac/include/nexus
    DEPENDPATH  += $$PWD/../../../../external/mac/include
    DEPENDPATH  += $$PWD/../../../../external/mac/include/hdf5
    DEPENDPATH  += $$PWD/../../../../external/mac/include/nexus
    LIBS += -L$$PWD/../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}
