#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T20:36:29
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tkipladvfilterstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG += c++11

unix {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
        QMAKE_MAC_SDK = macosx10.11
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
}

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:symbian: LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
else:unix: LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff  -lcfitsio




SOURCES += tst_tkipladvfilterstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/src
