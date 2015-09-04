#-------------------------------------------------
#
# Project created by QtCreator 2013-08-16T17:21:41
#
#-------------------------------------------------

QT       -= gui

TARGET = StdBackProjectors
TEMPLATE = lib
CONFIG += c++11

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
    INCLUDEPATH += ../../../../../../external/src/linalg ../../../../../../external/include ../../../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
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
    ../../src/stdafx.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ReconFramework/build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ReconFramework/build-ReconFramework-Qt5-Debug/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix: CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ReconFramework/build-ReconFramework-Qt5-Release/ -lReconFramework
else:unix: CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ReconFramework/build-ReconFramework-Qt5-Debug/ -lReconFramework

INCLUDEPATH += $$PWD/../../../ReconFramework/include
DEPENDPATH += $$PWD/../../../ReconFramework/src




win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
