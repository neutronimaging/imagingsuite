#-------------------------------------------------
#
# Project created by QtCreator 2014-09-20T16:36:56
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tlinfittest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_tlinfittest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix: LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/ -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/src
