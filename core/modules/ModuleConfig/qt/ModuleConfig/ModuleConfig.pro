#-------------------------------------------------
#
# Project created by QtCreator 2013-05-04T12:48:45
#
#-------------------------------------------------

QT       -= gui

TARGET = ModuleConfig
TEMPLATE = lib

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../lib/debug

unix {
    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp -fPIC -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        LIBS += -L/usr/lib -lxml2
        INCLUDEPATH += /usr/include/libxml2
    }

    unix:macx {
    #    QMAKE_MAC_SDK = macosx10.11
        QMAKE_CXXFLAGS += -fPIC -O2
        QMAKE_LIBDIR += /opt/local/lib
        LIBS += -L/opt/local/lib -lxml2
        INCLUDEPATH += /opt/local/include /opt/local/include/libxml2
    }
}

windows {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../../external/include
    LIBPATH += $$PWD/../../../../../external/lib64
    LIBS += -llibxml2_dll
    QMAKE_CXXFLAGS += /openmp /O2
}

DEFINES += MODULECONFIG_LIBRARY

SOURCES += \
    ../../src/stdafx.cpp \
    ../../src/ProcessModuleBase.cpp \
    ../../src/ParameterHandling.cpp \
    ../../src/ModuleItemBase.cpp \
    ../../src/ModuleException.cpp \
    ../../src/ModuleConfig.cpp \
    ../../src/dllmain.cpp \
    ../../src/ConfigBase.cpp \
    ../../src/datamodulebase.cpp

HEADERS +=\
     ../../include/ModuleConfig_global.h \
    ../../include/targetver.h \
    ../../include/ProcessModuleBase.h \
    ../../include/ParameterHandling.h \
    ../../include/ModuleItemBase.h \
    ../../include/ModuleException.h \
    ../../include/ModuleConfig.h \
    ../../include/ConfigBase.h \
    ../../include/datamodulebase.h
    ../../src/stdafx.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xECD31538
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ModuleConfig.dll
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

CONFIG(release, debug|release) LIBS += -L$$PWD/../../../../../../lib/ -lkipl
else:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../../../../lib/debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/kipl/include/
DEPENDPATH += $$PWD/../../../../kipl/kipl/include/
