#-------------------------------------------------
#
# Project created by QtCreator 2013-09-02T06:58:44
#
#-------------------------------------------------

QT       += core widgets charts

TARGET = StdPreprocModulesGUI
TEMPLATE = lib
CONFIG += c++11

REPOS=$$PWD/../../../../../../..

CONFIG(release, debug|release): DESTDIR = $$REPOS/lib
else:CONFIG(debug, debug|release): DESTDIR = $$REPOS/lib/debug

DEFINES += STDPREPROCMODULESGUI_LIBRARY

SOURCES += ../../src/stdpreprocmodulesgui.cpp \
    ../../src/FullLogNormDlg.cpp \
    ../../src/WaveletRingCleanDlg.cpp \
    ../../src/projectionfilterdlg.cpp \
    ../../src/morphspotcleandlg.cpp \
    ../../src/polynomialcorrectiondlg.cpp \
    ../../src/datascalerdlg.cpp \
    ../../src/adaptivefilterdlg.cpp \
    ../../src/SpotClean2Dlg.cpp \
    ../../src/medianmixringcleandlg.cpp \
    ../../src/generalfilterdlg.cpp \
    ../../src/bblognormdlg.cpp

HEADERS += ../../src/stdpreprocmodulesgui.h\
    ../../src/StdPreprocModulesGUI_global.h \
    ../../src/FullLogNormDlg.h \
    ../../src/WaveletRingCleanDlg.h \
    ../../src/projectionfilterdlg.h \
    ../../src/morphspotcleandlg.h \
    ../../src/polynomialcorrectiondlg.h \
    ../../src/datascalerdlg.h \
    ../../src/adaptivefilterdlg.h \
    ../../src/SpotClean2Dlg.h \
    ../../src/medianmixringcleandlg.h \
    ../../src/generalfilterdlg.h \
    ../../src/bblognormdlg.h


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

    INCLUDEPATH  += $$REPOS/ExternalDependencies/windows/include
    INCLUDEPATH  += $$REPOS/ExternalDependencies/windows/include/libxml2
    INCLUDEPATH  += $$REPOS/ExternalDependencies/windows/include/cfitsio
    QMAKE_LIBDIR += $$REPOS/ExternalDependencies/windows/lib

    INCLUDEPATH  += $$REPOS/imaginsuite/external/include
    QMAKE_LIBDIR += $$REPOS/imaginsuite/external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

CONFIG(release, debug|release): LIBS += -L$$REPOS/lib/
else:CONFIG(debug, debug|release): LIBS += -L$$REPOS/lib/debug/

LIBS += -lkipl -lQtModuleConfigure -lQtAddons -lImagingAlgorithms -lReconFramework -lModuleConfig -lStdPreprocModules

INCLUDEPATH += $$PWD/../../../../../../GUI/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../Framework/ReconFramework/include

INCLUDEPATH += $$PWD/../../../StdPreprocModules/include
DEPENDPATH += $$PWD/../../../StdPreprocModules/include

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include

macx: {
INCLUDEPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
DEPENDPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}

FORMS += \
    ../../src/polynomialcorrectiondlg.ui \
    ../../src/datascalerdlg.ui \
    ../../src/adaptivefilterdlg.ui \
    ../../src/morphspotcleandlg.ui \
    ../../src/SpotClean2Dlg.ui \
    ../../src/medianmixringclean.ui \
    ../../src/projectionfilterdlg.ui \
    ../../src/WaveletRingCleanDlg.ui \
    ../../src/generalfilterdlg.ui \
    ../../src/FullLogNormDlg.ui \
    ../../src/bblognormdlg.ui

