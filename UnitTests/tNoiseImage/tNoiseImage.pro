#-------------------------------------------------
#
# Project created by QtCreator 2015-04-12T15:42:45
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tnoiseimagetest
CONFIG   += console
#CONFIG   += c++11
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -O2

SOURCES += tst_tnoiseimagetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug,   debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/include
