#-------------------------------------------------
#
# Project created by QtCreator 2015-10-27T18:11:30
#
#-------------------------------------------------

QT       -= gui

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../lib/debug

TARGET = ReaderConfig
TEMPLATE = lib

DEFINES += READERCONFIG_LIBRARY

SOURCES += readerconfig.cpp \
    datasetbase.cpp \
    buildfilelist.cpp \
    imagereader.cpp \
    readerexception.cpp \
    imagewriter.cpp

HEADERS += readerconfig.h\
        readerconfig_global.h \
    datasetbase.h \
    buildfilelist.h \
    imagereader.h \
    readerexception.h \
    imagewriter.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
        QMAKE_MAC_SDK = macosx10.12
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../external/src/linalg ../../../external/include ../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:unix: LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff -lcfitsio

win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug -lkipl
else:unix:CONFIG(release, debug|release) LIBS += -L$$PWD/../../../lib/ -lkipl
else:unix:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../lib/debug/ -lkipl

INCLUDEPATH += $$PWD/../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/debug/ -lkipl

