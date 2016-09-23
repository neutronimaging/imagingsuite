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
        QMAKE_MAC_SDK = macosx10.11
        QMAKE_CXXFLAGS += -fPIC -O2
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

    LIBS += -ltiff -lxml2

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build-ReconFramework-Qt5-Release/debug/ -lReconFramework

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../Framework/ReconFramework/src



DEFINES += FDKBACKPROJ_LIBRARY

SOURCES += ../../src/fdkbackproj.cpp  \
        ../../src/genericbp.cpp \
        ../../src/fdkreconbase.cpp \
        ../../src/fdkbp.cpp
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
         ../../src/fdkbp.h
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




win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib/ -lkipl -lModuleConfig -lReconFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/ -lkipl -lModuleConfig -lReconFramework -lReconAlgorithms

INCLUDEPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/debug/ -lModuleConfig


INCLUDEPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../build-ReconAlgorithms-Qt5-Release/release/ -lReconAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../build-ReconAlgorithms-Qt5-Release/debug/ -lReconAlgorithms


INCLUDEPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
DEPENDPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
