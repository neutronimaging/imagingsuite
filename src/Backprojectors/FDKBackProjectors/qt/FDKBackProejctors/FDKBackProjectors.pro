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
#       QMAKE_MAC_SDK = macosx10.12
        QMAKE_CXXFLAGS += -fPIC -O2
#        QMAKE_LFLAGS += -lfftw3 -lfftw3f
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
        QMAKE_INFO_PLIST = Info.plist
        ICON = muhrec3.icns
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2 -lfftw3 -lfftw3f

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH  += ../../../../../../../external/src/linalg ../../../../../../../external/include ../../../../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio -llibfftw3-3 -llibfftw3f-3
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

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib/ -lkipl -lModuleConfig -lReconFramework
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/ -lkipl -lModuleConfig -lReconFramework -lReconAlgorithms

INCLUDEPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../../kipl/trunk/kipl/src

INCLUDEPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/src

INCLUDEPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
DEPENDPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../Framework/ReconFramework/src

