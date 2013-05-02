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

CMAKE_CXX_FLAGS += "-fopenmp"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtAddons-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtAddons-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/debug/ -lQtAddons
else:symbian: LIBS += -L$$PWD/../QtAddons-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../QtAddons
DEPENDPATH += $$PWD/../QtAddons
