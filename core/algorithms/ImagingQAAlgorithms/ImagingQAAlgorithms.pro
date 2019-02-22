#-------------------------------------------------
#
# Project created by QtCreator 2017-10-17T16:14:26
#
#-------------------------------------------------

QT       -= gui

TARGET = ImagingQAAlgorithms
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR    = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

DEFINES += IMAGINGQAALGORITHMS_LIBRARY

SOURCES += imagingqaalgorithms.cpp \
    ballanalysis.cpp \
    collimationestimator.cpp \
    resolutionestimators.cpp \
    contrastsampleanalysis.cpp \
    ballassemblyanalysis.cpp \
    profileextractor.cpp

HEADERS += imagingqaalgorithms.h\
        imagingqaalgorithms_global.h \
    ballanalysis.h \
    collimationestimator.h \
    resolutionestimators.h \
    contrastsampleanalysis.h \
    ballassemblyanalysis.h \
    profileextractor.h

win32 {
    message(Building for Windows)
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += "$$PWD/../../../external/src/linalg" "$$PWD/../../../external/include" "$$PWD/../../../external/include/cfitsio"
    QMAKE_LIBDIR += $$PWD/../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

unix {
    target.path = /usr/lib
    INSTALLS += target

    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += "../../../external/src/linalg"

    unix:!macx {

        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        LIBS += -L/usr/lib -lxml2 -ltiff
        INCLUDEPATH += /usr/include/libxml2

    }
    else
    {
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib

        LIBS += -L/opt/local/lib/ -lxml2 -ltiff
    }


}
CONFIG(release, debug|release)      LIBS += -L$$PWD/../../../../lib
else:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/kipl/include
