#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T07:39:53
#
#-------------------------------------------------

QT       += core

TARGET = BaseModules
TEMPLATE = lib

unix:!symbian {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
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



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Release/ -lProcessFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Debug/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include
