#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T19:19:07
#
#-------------------------------------------------

QT       += svg widgets printsupport
#QT       += core gui
CONFIG += c++11

TARGET = WidgetTests
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

unix {
    INCLUDEPATH += /usr/include/libxml2
}

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}

win32 {
    INCLUDEPATH += ../../../../external/include
    LIBPATH += ../../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/trunk/kipl/include


win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-QtAddons-Qt5-Debug/debug/ -lQtAddons
else:symbian: LIBS += -L$$PWD/../QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-QtAddons-Qt5-Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-QtAddons-Qt5-Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../QtAddons
DEPENDPATH += $$PWD/../QtAddons

win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../build-QtModuleConfigure-Qt5-Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-QtModuleConfigure-Qt5-Debug/debug/ -lQtModuleConfigure
else:symbian: LIBS += -lQtModuleConfigure
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-QtModuleConfigure-Qt5-Release/ -lQtModuleConfigure
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtModuleConfigure-Qt5-Debug/ -lQtModuleConfigure
INCLUDEPATH += $$PWD/../QtModuleConfigure
DEPENDPATH += $$PWD/../QtModuleConfigure

win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/qt/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/qt/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/build-ReaderConfig-Qt5-Release/release/ -lReaderConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/build-ReaderConfig-Qt5-Release/debug/ -lReaderConfig
else:unix: LIBS += -L$$PWD/../../../../modules/trunk/build-ReaderConfig-Qt5-Release/ -lReaderConfig

INCLUDEPATH += $$PWD/../../../../modules/trunk/ReaderConfig
DEPENDPATH += $$PWD/../../../../modules/trunk/ReaderConfig

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/build-ReaderGUI-Qt5-Release/release/ -lReaderGUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/build-ReaderGUI-Qt5-Release/debug/ -lReaderGUI
else:unix: LIBS += -L$$PWD/../../../../modules/trunk/build-ReaderGUI-Qt5-Release/ -lReaderGUI

INCLUDEPATH += $$PWD/../../../../modules/trunk/ReaderGUI
DEPENDPATH += $$PWD/../../../../modules/trunk/ReaderGUI
