#-------------------------------------------------
#
# Project created by QtCreator 2017-10-26T20:45:55
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_tkiplmathtest
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

TEMPLATE = app

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
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
    LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff  -lcfitsio -larmadillo -llapack -lblas
}


win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../external/include $$PWD/../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$PWD/../../../../external/lib64
    QMAKE_LIBDIR += $$PWD/../../../../../ExternalDependencies/windows/lib
    QMAKE_CXXFLAGS += /openmp /O2

    LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
    LIBS += -llibopenblas
#    LIBS += -lliblapack -llibblas
}



SOURCES += \
        tst_tkiplmathtest.cpp 

DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/src
