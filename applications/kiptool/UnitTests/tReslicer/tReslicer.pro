QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app


CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

unix {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib

    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2 -lcfitsio

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../external/src/linalg $$PWD/../../../../external/include $$PWD/../../../../external/include/cfitsio $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

unix:mac {
    exists($$PWD/../../../../external/mac/lib/*NeXus*) {

        message("-lNeXus exists")
        DEFINES *= HAVE_NEXUS

        INCLUDEPATH += $$PWD/../../../../external/mac/include/ $$PWD/../../../../external/mac/include/nexus $$PWD/../../../../external/mac/include/hdf5
        DEPENDPATH += $$PWD/../../../../external/mac/include/ $$PWD/../../../../external/mac/include/nexus $$PWD/../../../../external/mac/include/hdf5
        QMAKE_LIBDIR += $$PWD/../../../../external/mac/lib/

        LIBS += -lNeXus.1.0.0 -lNeXusCPP.1.0.0


    }
    else {
        message("-lNeXus does not exist $$HEADERS")
    }
}

SOURCES += \ 
    tst_reslicer.cpp \
    ../../src/Reslicer.cpp

HEADERS += \
    ../../src/Reslicer.h


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/include

