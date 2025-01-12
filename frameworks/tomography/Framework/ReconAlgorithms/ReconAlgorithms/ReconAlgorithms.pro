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

    LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff -lfftw3 -lfftw3f -lcfitsio
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }

    QMAKE_LIBDIR += $$PWD/../../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH  += $$PWD/../../../../../../ExternalDependencies/windows/include/cfitsio
    INCLUDEPATH  += $$PWD/../../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH +=  $$PWD/../../../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2

    LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
}

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


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../core/kipl/kipl/include
