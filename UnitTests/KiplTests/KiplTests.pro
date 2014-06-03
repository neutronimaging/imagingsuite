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

SOURCES += \
    tstcovariance.cpp \
    tstDirAnalyzer.cpp \
    UnitTestMain.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L../../kipl/kipl-build-Qt_4_8_1_Release/ -lkipl

INCLUDEPATH += $$PWD/../../kipl/include $$PWD/../../../../src/external/src/linalg
DEPENDPATH += $$PWD/../../kipl/include

HEADERS += \
    tstcovariance.h \
    tstDirAnalyzer.h
