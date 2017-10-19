#-------------------------------------------------
#
# Project created by QtCreator 2013-12-26T15:54:27
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = unittest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

SOURCES += \
    tstcovariance.cpp \
    tstDirAnalyzer.cpp \
    UnitTestMain.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release): LIBS += -L../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L../../../../../lib/debug/

INCLUDEPATH += $$PWD/../../kipl/include $$PWD/../../../../src/external/src/linalg
DEPENDPATH += $$PWD/../../kipl/include

LIBS += -lkipl

HEADERS += \
    tstcovariance.h \
    tstDirAnalyzer.h
