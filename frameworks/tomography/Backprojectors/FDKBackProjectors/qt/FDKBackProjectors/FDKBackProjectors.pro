#-------------------------------------------------
#
# Project created by QtCreator 2015-02-17T07:15:06
#
#-------------------------------------------------

QT       -= gui

TARGET = FDKBackProjectors
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

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
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
        LIBS += -L/usr/local/lib64 -L/usr/lib/x86_64-linux-gnu -lNeXus -lNeXusCPP
    }

    LIBS += -ltiff -lxml2 -lfftw3 -lfftw3f

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }


    QMAKE_LIBDIR += $$PWD/../../../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/cfitsio
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH  += $$PWD/../../../../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio -llibfftw3-3 -llibfftw3f-3
    QMAKE_CXXFLAGS += /openmp /O2
}


DEFINES += FDKBACKPROJ_LIBRARY

SOURCES += ../../src/fdkbackproj.cpp  \
        ../../src/genericbp.cpp \
        ../../src/fdkreconbase.cpp \
        ../../src/fdkbp.cpp \
        ../../src/fdkbp_single.cpp
#        ../../src/ramp_filter.cpp
#        ../../src/fdk.cxx \
#        ../../src/bowtie_correction.cxx \
#        ../../src/delayload.cxx \
#        ../../src/file_util.cxx \
#        ../../src/plm_math.cxx \
#        ../../src/plm_timer.cxx \
#        ../../src/proj_image.cxx \
#        ../../src/proj_matrix.cxx \
#        ../../src/proj_image_filter.cxx \
#        ../../src/proj_matrix.cxx \
#        ../../src/volume.cxx
#understand what to include in a smart way

HEADERS += ../../src/fdkbackproj.h \
	../../src/fdkbackproj_global.h \
	../../src/genericbp.h \
        ../../src/fdkreconbase.h \
         ../../src/fdkbp.h \
        ../../src/fdkbp_single.h
#        ../../src/ramp_filter.h
#        ../../src/fdk.h \
#        ../../src/bowtie_correction.h \
#        ../../src/delayload.h \
#        ../../src/file_util.h \
#        ../../src/plm_math.h \
#        ../../src/plm_timer.h \
#        ../../src/proj_image.h \
#        ../../src/proj_image_filter.h \
#        ../../src/proj_matrix.h \
#        ../../src/volume.h \
#        ../../src/threding.h

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lImagingAlgorithms -lReaderConfig -lReconFramework

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH  += $$PWD/../../../../../../core/kipl/kipl/src

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH  += $$PWD/../../../../../../core/modules/ModuleConfig/src

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
DEPENDPATH  += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH  += $$PWD/../../../../Framework/ReconFramework/src

macx: {
INCLUDEPATH += $$PWD/../../../../../../external/mac/include
DEPENDPATH += $$PWD/../../../../../../external/mac/include
LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}


