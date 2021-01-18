#-------------------------------------------------
#
# Project created by QtCreator 2018-05-03T07:23:03
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_treaderconfigtest
CONFIG   += console
CONFIG   += c++11
CONFIG   -= app_bundle

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


SOURCES += \
        tst_treaderconfigtest.cpp 

DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += $$PWD/../../ReaderConfig
DEPENDPATH += $$PWD/../../ReaderConfig

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2

        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2

        QMAKE_LIBDIR += /opt/local/lib
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    LIBS += -ltiff -lxml2
    INCLUDEPATH += /usr/include/libxml2
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../external/include $$PWD/../../../../external/include/cfitsio $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../../lib -lkipl -lReaderConfig
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug -lkipl -lReaderConfig

INCLUDEPATH += $$PWD/../../../kipl/kipl/include
DEPENDPATH  += $$PWD/../../../kipl/kipl/include

INCLUDEPATH += $$PWD/../../../algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../algorithms/ImagingAlgorithms/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lImagingAlgorithms.1.0.0
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lImagingAlgorithms.1.0.0
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lImagingAlgorithms.1.0.0

#INCLUDEPATH += $$PWD/../../../algorithms/ImagingAlgorithms/include
#DEPENDPATH += $$PWD/../../../algorithms/ImagingAlgorithms/include
