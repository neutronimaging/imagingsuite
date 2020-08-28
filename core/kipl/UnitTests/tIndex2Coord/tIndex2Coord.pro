#-------------------------------------------------
#
# Project created by QtCreator 2018-05-01T21:56:17
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tindex2coordtest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        tst_tindex2coordtest.cpp 

DEFINES += SRCDIR=\\\"$$PWD/\\\"


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/
LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/include
