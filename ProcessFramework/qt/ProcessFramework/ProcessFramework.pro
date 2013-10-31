#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T07:19:32
#
#-------------------------------------------------

QT       -= core gui

TARGET = ProcessFramework
TEMPLATE = lib

DEFINES += PROCESSFRAMEWORK_LIBRARY

SOURCES += \
    ../../src/stdafx.cpp \
    ../../src/KiplProcessModuleBase.cpp \
    ../../src/KiplProcessConfig.cpp \
    ../../src/KiplModuleItem.cpp \
    ../../src/KiplFrameworkException.cpp \
    ../../src/KiplFactory.cpp \
    ../../src/KiplEngine.cpp \
    ../../src/dllmain.cpp

HEADERS += \
    ../../include/KiplProcessModuleBase.h \
    ../../include/KiplProcessConfig.h \
    ../../include/KiplModuleItem.h \
    ../../include/KiplFrameworkException.h \
    ../../include/KiplFactory.h \
    ../../include/KiplEngine.h \
    ../../src/targetver.h \
    ../../src/stdafx.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2AB701B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ProcessFramework.dll
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../../src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../src/libs/kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/include

LIBS += -L/usr/lib -lxml2 -ltiff
INCLUDEPATH += /usr/include/libxml2
