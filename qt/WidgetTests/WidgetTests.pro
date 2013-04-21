#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T19:19:07
#
#-------------------------------------------------

QT       += core gui

TARGET = WidgetTests
TEMPLATE = app

unix:INCLUDEPATH += "/home/anders/repos/src/src/libs/kipl/trunk/kipl/include"

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtAddons/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtAddons/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix: LIBS += -L$$PWD/../QtAddons/ -lQtAddons

INCLUDEPATH += $$PWD/../QtAddons
DEPENDPATH += $$PWD/../QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/release/kipl.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/debug/kipl.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/libkipl.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/release/kipl.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/debug/kipl.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/libkipl.a
