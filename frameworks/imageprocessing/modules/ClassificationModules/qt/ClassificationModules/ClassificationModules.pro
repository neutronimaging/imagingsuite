#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T18:22:19
#
#-------------------------------------------------

QT       += core

TARGET = ClassificationModules
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

unix:!symbian {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
        QMAKE_MAC_SDK = macosx10.12
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }

    LIBS += -ltiff
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

DEFINES += CLASSIFICATIONMODULES_LIBRARY

SOURCES += \
    ../../src/RemoveBackground.cpp \
    # ../../src/KernelFuzzyCMeans.cpp \
    #../../src/FuzzyCMeans.cpp \
    ../../src/DoubleThreshold.cpp \
    ../../src/ClassificationModules.cpp \
    ../../src/BasicThreshold.cpp

HEADERS += \
    ../../src/RemoveBackground.h \
   # ../../src/KernelFuzzyCMeans.h \
   # ../../src/FuzzyCMeans.h \
    ../../src/DoubleThreshold.h \
    ../../src/ClassificationModules.h \
    ../../src/BasicThreshold.h \
    ../../src/ClassificationModules_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA26CA15
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ClassificationModules.dll
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

    LIBS += -ltiff -lxml2
    INCLUDEPATH += /usr/include/libxml2
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../../../external/src/linalg $$PWD/../../../../../../external/include $$PWD/../../../../../../external/include/cfitsio $$PWD/../../../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2 /DNOMINMAX
}


macx: {
INCLUDEPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
DEPENDPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lProcessFramework

INCLUDEPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include
DEPENDPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/include


