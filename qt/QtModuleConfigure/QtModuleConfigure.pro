#-------------------------------------------------
#
# Project created by QtCreator 2013-05-04T14:04:46
#
#-------------------------------------------------

QT       += widgets

TARGET = QtModuleConfigure
TEMPLATE = lib

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}

unix {
    INCLUDEPATH += /usr/include/libxml2
}

win32 {
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



win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/qt/ModuleConfig-Qt_5_2_1-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/qt/ModuleConfig-Qt_5_2_1-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt_5_2_1_64bit-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt_5_2_1_64bit-Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/trunk/kipl/include
