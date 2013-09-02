#-------------------------------------------------
#
# Project created by QtCreator 2013-09-02T06:58:44
#
#-------------------------------------------------

TARGET = StdPreprocModulesGUI
TEMPLATE = lib

DEFINES += STDPREPROCMODULESGUI_LIBRARY

SOURCES += stdpreprocmodulesgui.cpp \
    stdafx.cpp \
    SpotClean2Dlg.cpp \
    dllmain.cpp \
    FullLogNormDlg.cpp

HEADERS += stdpreprocmodulesgui.h\
        StdPreprocModulesGUI_global.h \
    stdafx.h \
    SpotClean2Dlg.h \
    FullLogNormDlg.h

INCLUDEPATH += /usr/include/libxml2

QMAKE_CXXFLAGS += -fPIC -fopenmp

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA388B00
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = StdPreprocModulesGUI.dll
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtModuleConfigure
else:symbian: LIBS += -lQtModuleConfigure
else:unix: LIBS += -L$$PWD/../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtModuleConfigure

INCLUDEPATH += $$PWD/../../../../gui/trunk/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../gui/trunk/qt/QtModuleConfigure

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../../src/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../src/src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../src/src/libs/modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix: LIBS += -L$$PWD/../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtAddons

INCLUDEPATH += $$PWD/../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lImagingAlgorithms
else:symbian: LIBS += -lImagingAlgorithms
else:unix: LIBS += -L$$PWD/../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../qni/trunk/src/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../qni/trunk/src/ImagingAlgorithms/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix: LIBS += -L$$PWD/../../../../src/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lReconFramework

INCLUDEPATH += $$PWD/../../../../src/src/libs/recon2/trunk/ReconFramework/include
DEPENDPATH += $$PWD/../../../../src/src/libs/recon2/trunk/ReconFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/recon2/trunk/StdPreprocModules/qt/StdPreprocModules-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lStdPreprocModules
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/recon2/trunk/StdPreprocModules/qt/StdPreprocModules-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lStdPreprocModules
else:symbian: LIBS += -lStdPreprocModules
else:unix: LIBS += -L$$PWD/../../../../src/src/libs/recon2/trunk/StdPreprocModules/qt/StdPreprocModules-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lStdPreprocModules

INCLUDEPATH += $$PWD/../../../../src/src/libs/recon2/trunk/StdPreprocModules/include
DEPENDPATH += $$PWD/../../../../src/src/libs/recon2/trunk/StdPreprocModules/include
