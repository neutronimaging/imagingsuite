#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T19:19:07
#
#-------------------------------------------------

QT       += svg widgets printsupport
#QT       += core gui
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

TARGET = WidgetTests
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

unix {
    INCLUDEPATH += /usr/include/libxml2
}

unix:macx {
    QMAKE_MAC_SDK = macosx10.12
}

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}

win32 {
    INCLUDEPATH += ../../../external/include
    LIBPATH += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib/ -lkipl -lQtAddons -lQtModuleConfigure -lModuleConfig -lReaderConfig -lReaderGUI
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib/debug/ -lkipl -lQtAddons -lQtModuleConfigure -lModuleConfig -lReaderConfig -lReaderGUI

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../QtAddons
DEPENDPATH += $$PWD/../QtAddons

INCLUDEPATH += $$PWD/../QtModuleConfigure
DEPENDPATH += $$PWD/../QtModuleConfigure

INCLUDEPATH += $$PWD/../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../core/modules/ReaderConfig
DEPENDPATH += $$PWD/../../../core/modules/ReaderConfig

INCLUDEPATH += $$PWD/../../../core/modules/ReaderGUI
DEPENDPATH += $$PWD/../../../core/modules/ReaderGUI
