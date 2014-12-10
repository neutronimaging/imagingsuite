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

TEMPLATE = app

win32 {
    INCLUDEPATH += ../../../../external/src/linalg ../../../../external/include ../../../../external/include/cfitsio
    LIBPATH += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += tst_tdiranalysistest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../kipl/kipl-build-Qt_4_8_1_Release/ -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/include
