#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T18:05:46
#
#-------------------------------------------------

QT       += widgets

TARGET = InspectorModulesGUI
TEMPLATE = lib
DEFINES += INSPECTORMODULESGUI_LIBRARY

SOURCES += inspectormodulesgui.cpp \
    saveprojectionsdlg.cpp

HEADERS += inspectormodulesgui.h\
        inspectormodulesgui_global.h \
    saveprojectionsdlg.h

FORMS += \
    saveprojectionsdlg.ui

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
    INCLUDEPATH += $$PWD/../../../../external/src/linalg $$PWD/../../../../external/include $$PWD/../../../../external/include/cfitsio $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Debug/debug/ -lQtModuleConfigure
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/ -lQtModuleConfigure
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Debug/ -lQtModuleConfigure

INCLUDEPATH += $$PWD/../../../../gui/trunk/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../gui/trunk/qt/QtModuleConfigure

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/debug/ -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/trunk/kipl/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/debug/ -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/debug/ -lReconFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/ -lReconFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Debug/ -lReconFramework

INCLUDEPATH += $$PWD/../../../../src/libs/recon2/trunk/ReconFramework/include
DEPENDPATH += $$PWD/../../../../src/libs/recon2/trunk/ReconFramework/src
