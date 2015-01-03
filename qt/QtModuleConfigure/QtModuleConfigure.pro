#-------------------------------------------------
#
# Project created by QtCreator 2013-05-04T14:04:46
#
#-------------------------------------------------

QT       += core widgets

TARGET = QtModuleConfigure
TEMPLATE = lib
unix {

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -O2 -fPIC
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
    INCLUDEPATH += /usr/include/libxml2
}
unix:macx {
    QMAKE_CXXFLAGS += -O2 -fPIC
    INCLUDEPATH += /opt/local/include/libxml2
}


}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../external/include
    LIBPATH += ../../../../../external/lib64
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

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release -lkipl
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/trunk/kipl/include

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release -lModuleConfig
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include

