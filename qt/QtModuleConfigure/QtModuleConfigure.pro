#-------------------------------------------------
#
# Project created by QtCreator 2013-05-04T14:04:46
#
#-------------------------------------------------

TARGET = QtModuleConfigure
TEMPLATE = lib

QMAKE_CXXFLAGS += -fPIC -fopenmp

DEFINES += QTMODULECONFIGURE_LIBRARY

SOURCES += qtmoduleconfigure.cpp \
    modulechainconfiguratorwidget.cpp \
    singlemoduleconfiguratorwidget.cpp \
    ApplicationBase.cpp \
    ModuleConfigurator.cpp \
    AddModuleDialog.cpp \
    ConfiguratorDialogBase.cpp

HEADERS += qtmoduleconfigure.h\
        QtModuleConfigure_global.h \
    modulechainconfiguratorwidget.h \
    singlemoduleconfiguratorwidget.h \
    ApplicationBase.h \
    ModuleConfigurator.h \
    AddModuleDialog.h \
    QListWidgetModuleItem.h \
    ConfiguratorDialogBase.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/libs/kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lModuleConfig

LIBS += -lgomp

INCLUDEPATH += $$PWD/../../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../src/libs/modules/trunk/ModuleConfig/include

INCLUDEPATH += /usr/include/libxml2
