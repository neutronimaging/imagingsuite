#-------------------------------------------------
#
# Project created by QtCreator 2014-06-02T22:05:38
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tdiranalysistest
CONFIG   += console
CONFIG   -= app_bundle

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

TEMPLATE = app

win32 {
    INCLUDEPATH += ../../../../external/src/linalg ../../../../external/include ../../../../external/include/cfitsio
    LIBPATH += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += tst_tdiranalysistest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release): LIBS += -L../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/include
