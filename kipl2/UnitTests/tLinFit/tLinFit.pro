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

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib/debug/


LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/src
