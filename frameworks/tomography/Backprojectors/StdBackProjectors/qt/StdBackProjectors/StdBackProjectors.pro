#-------------------------------------------------
#
# Project created by QtCreator 2013-08-16T17:21:41
#
#-------------------------------------------------

QT       -= gui

TARGET = StdBackProjectors
TEMPLATE = lib
CONFIG += c++17

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../../../lib
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
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    QMAKE_LIBDIR += $$PWD/../../../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/cfitsio

    INCLUDEPATH  += $$PWD/../../../../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

DEFINES += STDBACKPROJECTORS_LIBRARY

SOURCES += ../../src/StdBackProjectors.cpp \
    ../../src/StdBackProjectorBase.cpp \
    ../../src/stdafx.cpp \
    ../../src/NNMultiProjBP.cpp \
    ../../src/MultiProjBPparallel.cpp \
    ../../src/MultiProjBP.cpp \
    ../../src/dllmain.cpp

HEADERS +=\
    ../../include/StdBackProjectors.h \
    ../../include/StdBackProjectorBase.h \
    ../../include/NNMultiProjBP.h \
    ../../include/MultiProjBPparallel.h \
    ../../include/MultiProjBP.h \
    ../../src/stdafx.h \
    ../../include/StdBackProjectors_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE3E2CB4A
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = StdBackProjectors.dll
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



CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/


#INCLUDEPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include
#DEPENDPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include
#INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
#DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/src


#INCLUDEPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
#DEPENDPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include

LIBS += -lkipl -lModuleConfig -lReconFramework -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/src

macx: {
INCLUDEPATH += $$PWD/../../../../../../external/mac/include
DEPENDPATH += $$PWD/../../../../../../external/mac/include
LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}
