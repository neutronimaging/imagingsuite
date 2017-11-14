#-------------------------------------------------
#
# Project created by QtCreator 2013-12-05T10:46:39
#
#-------------------------------------------------

QT       += core

TARGET = ImagingModules
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

DEFINES += IMAGINGMODULES_LIBRARY

SOURCES +=  ../../src/imagingmodules.cpp \
            ../../src/translateprojectionmodule.cpp \
            ../../src/stripefiltermodule.cpp \
            ../../src/ringcleanmodule.cpp \
            ../../src/referenceimagecorrectionmodule.cpp

HEADERS +=  ../../src/ImagingModules_global.h \
            ../../src/imagingmodules.h\
            ../../src/ImagingModules_global.h \
            ../../src/translateprojectionmodule.h \
            ../../src/stripefiltermodule.h \
            ../../src/ringcleanmodule.h \
            ../../src/referenceimagecorrectionmodule.h

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -fPIC -O2
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}

unix:macx {
    QMAKE_MAC_SDK = macosx10.12
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
}

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2A168C3
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ImagingModules.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../../external/src/linalg $$PWD/../../../../../external/include $$PWD/../../../../../external/include/cfitsio $$PWD/../../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2 /DNOMINMAX
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lProcessFramework -lImagingAlgorithms -lUnpublishedImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../../../../unpublished/core/algorithms/UnpublishedImagingAlgorithms
DEPENDPATH += $$PWD/../../../../../../../unpublished/core/algorithms/UnpublishedImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/src

INCLUDEPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include
DEPENDPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/include
