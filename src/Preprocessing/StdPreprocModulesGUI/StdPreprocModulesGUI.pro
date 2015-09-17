#-------------------------------------------------
#
# Project created by QtCreator 2013-09-02T06:58:44
#
#-------------------------------------------------

QT       += core widgets

TARGET = StdPreprocModulesGUI
TEMPLATE = lib
CONFIG += c++11

DEFINES += STDPREPROCMODULESGUI_LIBRARY

SOURCES += stdpreprocmodulesgui.cpp \
    stdafx.cpp \
    SpotClean2Dlg.cpp \
    dllmain.cpp \
    FullLogNormDlg.cpp \
    WaveletRingCleanDlg.cpp \
    projectionfilterdlg.cpp \
    morphspotcleandlg.cpp \
    polynomialcorrectiondlg.cpp \
    datascalerdlg.cpp \
    adaptivefilterdlg.cpp

HEADERS += stdpreprocmodulesgui.h\
        StdPreprocModulesGUI_global.h \
    stdafx.h \
    SpotClean2Dlg.h \
    FullLogNormDlg.h \
    WaveletRingCleanDlg.h \
    projectionfilterdlg.h \
    morphspotcleandlg.h \
    polynomialcorrectiondlg.h \
    datascalerdlg.h \
    adaptivefilterdlg.h


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

    unix:macx {
        QMAKE_MAC_SDK = macosx10.11
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
    INCLUDEPATH += $$PWD/../../../../../external/src/linalg
    INCLUDEPATH += $$PWD/../../../../../external/include
    INCLUDEPATH += $$PWD/../../../../../external/include/cfitsio
    INCLUDEPATH += $$PWD/../../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Debug/debug/ -lQtModuleConfigure
else:symbian: LIBS += -lQtModuleConfigure
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/ -lQtModuleConfigure
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Debug/ -lQtModuleConfigure

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../../gui/trunk/qt/QtModuleConfigure

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/debug/ -lImagingAlgorithms
else:symbian: LIBS += -lImagingAlgorithms
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/ -lImagingAlgorithms
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/ -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-ReconFramework-Qt5-Debug/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-ReconFramework-Qt5-Release/ -lReconFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-ReconFramework-Qt5-Debug/ -lReconFramework

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-StdPreprocModules-Qt5-Release/release/ -lStdPreprocModules
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-StdPreprocModules-Qt5-Debug/debug/ -lStdPreprocModules
else:symbian: LIBS += -lStdPreprocModules
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-StdPreprocModules-Qt5-Release/ -lStdPreprocModules
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-StdPreprocModules-Qt5-Debug/ -lStdPreprocModules

INCLUDEPATH += $$PWD/../StdPreprocModules/include
DEPENDPATH += $$PWD/../StdPreprocModules/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

FORMS += \
    polynomialcorrectiondlg.ui \
    datascalerdlg.ui \
    adaptivefilterdlg.ui \
    morphspotcleandlg.ui
