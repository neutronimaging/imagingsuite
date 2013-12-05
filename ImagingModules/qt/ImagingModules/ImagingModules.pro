#-------------------------------------------------
#
# Project created by QtCreator 2013-12-05T10:46:39
#
#-------------------------------------------------

QT       -= core gui

TARGET = ImagingModules
TEMPLATE = lib

DEFINES += IMAGINGMODULES_LIBRARY

SOURCES += ../../src/imagingmodules.cpp \
    ../../src/translateprojectionmodule.cpp

HEADERS += ../../src/imagingmodules.h\
        ../../src/ImagingModules_global.h \
    ../../src/translateprojectionmodule.h

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


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../../src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../src/libs/kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix: LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lImagingAlgorithms
else:symbian: LIBS += -lImagingAlgorithms
else:unix: LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/src
