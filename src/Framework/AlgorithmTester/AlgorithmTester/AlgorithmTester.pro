#-------------------------------------------------
#
# Project created by QtCreator 2015-09-16T15:25:31
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_algorithmtestertest
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
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../external/src/linalg ../../../../external/include ../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}


win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:symbian: LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
else:unix: LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff -lfftw3 -lfftw3f -lcfitsio

SOURCES += tst_algorithmtestertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../build-ReconAlgorithms-Qt5-Debug/release/ -lReconAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../build-ReconAlgorithms-Qt5-Debug/debug/ -lReconAlgorithms
else:unix:CONFIG(release, debug|release):  LIBS += -L$$PWD/../../../build-ReconAlgorithms-Qt5-Release/ -lReconAlgorithms
else:unix:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../build-ReconAlgorithms-Qt5-Debug/ -lReconAlgorithms

INCLUDEPATH += $$PWD/../../../Framework/ReconAlgorithms/ReconAlgorithms
DEPENDPATH += $$PWD/../../../Framework/ReconAlgorithms/ReconAlgorithms

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl
else:unix:CONFIG(release, debug|release):  LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../kipl/trunk/kipl/src
