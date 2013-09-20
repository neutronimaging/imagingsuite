#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T19:19:07
#
#-------------------------------------------------

QT       += core gui

TARGET = WidgetTests
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -fopenmp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/libs/kipl/trunk/kipl/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtAddons-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtAddons-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/debug/ -lQtAddons
else:symbian: LIBS += -L$$PWD/../QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../QtAddons
DEPENDPATH += $$PWD/../QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtModuleConfigure-4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtModuleConfigure-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtModuleConfigure
else:symbian: LIBS += -lQtModuleConfigure
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtModuleConfigure
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtModuleConfigure
INCLUDEPATH += $$PWD/../QtModuleConfigure
DEPENDPATH += $$PWD/../QtModuleConfigure

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/modules/trunk/ModuleConfig/qt/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lModuleConfig

LIBS += -lgomp

INCLUDEPATH += $$PWD/../../../../src/libs/modules/trunk/ModuleConfig/include
INCLUDEPATH += /usr/include/libxml2
DEPENDPATH += $$PWD/../../../../src/libs/modules/trunk/ModuleConfig/include
