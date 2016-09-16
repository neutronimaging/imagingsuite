#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T07:39:53
#
#-------------------------------------------------

QT       += core

TARGET = BaseModules
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

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

DEFINES += BASEMODULES_LIBRARY

SOURCES += \
    ../../src/VolumeProject.cpp \
    ../../src/stdafx.cpp \
    ../../src/ScaleData.cpp \
    ../../src/DoseCorrection.cpp \
    ../../src/dllmain.cpp \
    ../../src/ClampData.cpp \
    ../../src/BuildScene.cpp \
    ../../src/BaseModules.cpp

HEADERS += \
    ../../src/VolumeProject.h \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../src/ScaleData.h \
    ../../src/DoseCorrection.h \
    ../../src/ClampData.h \
    ../../src/BuildScene.h \
    ../../src/BaseModules.h \
    ../../src/BaseModules_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE6BE8260
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = BaseModules.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Debug/debug/ -lProcessFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib -lkipl -lModuleConfig -lProcessFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lkipl -lModuleConfig -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kiplQt5-Release/debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include
