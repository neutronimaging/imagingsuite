#-------------------------------------------------
#
# Project created by QtCreator 2013-05-04T14:04:46
#
#-------------------------------------------------

QT       += core widgets

TARGET = QtModuleConfigure
TEMPLATE = lib
CONFIG += c++11


CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

unix {

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -O2 -fPIC
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
    INCLUDEPATH += /usr/include/libxml2
}
unix:macx {
#    QMAKE_MAC_SDK = macosx10.12
    QMAKE_CXXFLAGS += -O2 -fPIC
    INCLUDEPATH += /opt/local/include/libxml2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
}


}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../external/include
    LIBPATH += ../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}


DEFINES += QTMODULECONFIGURE_LIBRARY

SOURCES += qtmoduleconfigure.cpp \
    modulechainconfiguratorwidget.cpp \
    singlemoduleconfiguratorwidget.cpp \
    ApplicationBase.cpp \
    ModuleConfigurator.cpp \
    AddModuleDialog.cpp \
    ConfiguratorDialogBase.cpp \
    stdafx.cpp

HEADERS += qtmoduleconfigure.h\
        QtModuleConfigure_global.h \
    modulechainconfiguratorwidget.h \
    singlemoduleconfiguratorwidget.h \
    ApplicationBase.h \
    ModuleConfigurator.h \
    AddModuleDialog.h \
    QListWidgetModuleItem.h \
    ConfiguratorDialogBase.h \
    stdafx.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE961B98C
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = QtModuleConfigure.dll
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

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib/ -lkipl -lModuleConfig -lQtAddons
else:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../lib/debug/ -lkipl -lModuleConfig -lQtAddons

INCLUDEPATH += $$PWD/../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../GUI/qt/QtAddons
