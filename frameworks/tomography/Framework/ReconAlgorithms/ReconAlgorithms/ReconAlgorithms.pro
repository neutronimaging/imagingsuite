#-------------------------------------------------
#
# Project created by QtCreator 2015-09-16T14:27:53
#
#-------------------------------------------------

QT       -= gui
QT       += core

TARGET = ReconAlgorithms
TEMPLATE = lib

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../lib/debug

DEFINES += RECONALGORITHMS_LIBRARY

unix {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
    #    QMAKE_MAC_SDK = macosx10.11
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../../../external/src/linalg ../../../../../../external/include ../../../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:symbian: LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
else:unix: LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff -lfftw3 -lfftw3f -lcfitsio


SOURCES += reconalgorithms.cpp \
    forwardprojectorbase.cpp \
    basicforwardprojector.cpp \
    backprojectorbase.cpp \
    basicbackprojector.cpp \
    nnforwardprojector.cpp \
    reconalgorithmexception.cpp \
    linearforwardprojector.cpp

HEADERS += reconalgorithms.h\
        reconalgorithms_global.h \
    forwardprojectorbase.h \
    basicforwardprojector.h \
    backprojectorbase.h \
    basicbackprojector.h \
    nnforwardprojector.h \
    reconalgorithmexception.h \
    linearforwardprojector.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../lib/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../lib/debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../lib/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../lib/debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
