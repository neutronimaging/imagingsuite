#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T18:27:31
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_fillholetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

SOURCES += tst_fillholetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release): LIBS += -L../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L../../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/include
