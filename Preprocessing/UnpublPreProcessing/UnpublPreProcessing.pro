#-------------------------------------------------
#
# Project created by QtCreator 2017-10-20T13:58:53
#
#-------------------------------------------------

QT       -= gui

TARGET = UnpublPreProcessing
TEMPLATE = lib

CONFIG += c++11


CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug
DEFINES += UNPUBLPREPROCESSING_LIBRARY

SOURCES += unpublpreprocessing.cpp \
           robustlognorm.cpp
HEADERS += unpublpreprocessing.h\
           robustlognorm.h \
           unpublpreprocessing_global.h


unix {
    target.path = /usr/lib
    INSTALLS += target

    unix:macx {
#        QMAKE_MAC_SDK = macosx10.12
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

    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../../imagingsuite/external/src/linalg
    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../../imagingsuite/external/include
    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../../imagingsuite/external/include/cfitsio
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../imagingsuite/external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
    DEFINES += NOMINMAX
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lReconFramework -lImagingAlgorithms -lUnpublishedImagingAlgorithms

#INCLUDEPATH += $$PWD/../../../../../imagingsuite/external/src/linalg
#INCLUDEPATH += $$PWD/../../../../../imagingsuite/external/include

INCLUDEPATH += $$PWD/../../../../../imagingsuite/core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../imagingsuite/core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../imagingsuite/core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../imagingsuite/core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../core/algorithms/UnpublishedImagingAlgorithms/
DEPENDPATH += $$PWD/../../../../core/algorithms/UnpublishedImagingAlgorithms/

INCLUDEPATH += $$PWD/../../../../../imagingsuite/core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../imagingsuite/core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../../imagingsuite/frameworks/tomography/Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../../imagingsuite/frameworks/tomography/Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../../imagingsuite/frameworks/tomography/Preprocessing/StdPreprocModules/include
DEPENDPATH += $$PWD/../../../../../imagingsuite/frameworks/tomography/Preprocessing/StdPreprocModules/src


macx: {
LIBS += -L$$PWD/../../../../../imagingsuite/external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
INCLUDEPATH += $$PWD/../../../../../imagingsuite/external/mac/include
DEPENDPATH += $$PWD/../../../../../imagingsuite/external/mac/include
}
