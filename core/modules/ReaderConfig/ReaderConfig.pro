#-------------------------------------------------
#
# Project created by QtCreator 2015-10-27T18:11:30
#
#-------------------------------------------------

QT       -= gui

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

TARGET = ReaderConfig
TEMPLATE = lib

DEFINES += READERCONFIG_LIBRARY

SOURCES += readerconfig.cpp \
    datasetbase.cpp \
    buildfilelist.cpp \
    imagereader.cpp \
    readerexception.cpp \
    imagewriter.cpp \
    analyzefileext.cpp

HEADERS += readerconfig.h\
        readerconfig_global.h \
    datasetbase.h \
    buildfilelist.h \
    imagereader.h \
    readerexception.h \
    imagewriter.h \
    analyzefileext.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix {
    INCLUDEPATH += "../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
        exists(/usr/local/lib/*NeXus*) {

            message("-lNeXus exists")
            DEFINES += HAVE_NEXUS

            LIBS += -L$$PWD/../../../../../../../../usr/local/lib/ -lNeXusCPP.1.0.0 -lNeXus

            INCLUDEPATH += $$PWD/../../../../../../../../usr/local/include
            DEPENDPATH += $$PWD/../../../../../../../../usr/local/include

            SOURCES += ../../kipl/kipl/src/io/io_nexus.cpp
            HEADERS += ../../kipl/kipl/include/io/io_nexus.h
        }
        else {
            message("-lNeXus does not exists $$HEADERS")
        }
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

    exists($$PWD/../../../../external/lib64/nexus/*NeXus*) {

        message("-lNeXus exists")
        DEFINES += HAVE_NEXUS
        INCLUDEPATH += $$PWD/../../../../external/include/nexus $$PWD/../../../../external/include/hdf5
        QMAKE_LIBDIR += $$PWD/../../../../external/lib64/nexus $$PWD/../../../../external/lib64/hdf5

        LIBS +=  -lNeXus -lNeXusCPP

        SOURCES += ../src/io/io_nexus.cpp
        HEADERS += ../include/io/io_nexus.h
    }
}

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:unix: LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff -lcfitsio

CONFIG(release, debug|release) LIBS += -L$$PWD/../../../../lib/
else:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/kipl/include
DEPENDPATH += $$PWD/../../kipl/kipl/include

