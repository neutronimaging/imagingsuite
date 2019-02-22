#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T18:05:46
#
#-------------------------------------------------

QT       += widgets

TARGET = InspectorModulesGUI
TEMPLATE = lib
DEFINES += INSPECTORMODULESGUI_LIBRARY
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

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
    INCLUDEPATH += $$PWD/../../../../external/src/linalg
    INCLUDEPATH += $$PWD/../../../../external/include
    INCLUDEPATH += $$PWD/../../../../external/include/cfitsio
    INLCUDEPATH += $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lQtModuleConfigure -lQtAddons -lReconFramework

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/src

macx: {

INCLUDEPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../external/mac/include/nexus/ $$PWD/../../../../external/mac/include/hdf5
DEPENDPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../external/mac/include/nexus/ $$PWD/../../../../external/mac/include/hdf5

LIBS += -L$$PWD/../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}
