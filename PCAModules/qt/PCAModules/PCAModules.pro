#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T19:34:12
#
#-------------------------------------------------

QT       += core

TARGET = PCAModules
TEMPLATE = lib

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -fPIC -O2
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}

unix:macx {
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
}


DEFINES += PCAMODULES_LIBRARY

SOURCES += ../../src/pcamodules.cpp \
    ../../src/PCAFilterModule.cpp \
    ../../src/stdafx.cpp

HEADERS += \
    ../../src/PCAFilterModule.h \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../src/PCAModules_global.h \
    ../../src/pcamodules.h

unix:INCLUDEPATH += $$PWD/../../../../src/external/src/linalg

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEFEEBACE
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = PCAModules.dll
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../src/libs/modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../..//kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../ProcessFramework/build-ProcessFramework-Qt5-Release/ -lProcessFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../ProcessFramework/build-ProcessFramework-Qt5-Debug/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include
