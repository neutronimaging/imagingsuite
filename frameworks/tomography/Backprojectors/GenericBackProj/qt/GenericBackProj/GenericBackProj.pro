#-------------------------------------------------
#
# Project created by QtCreator 2015-02-17T07:15:06
#
#-------------------------------------------------

QT       -= gui

TARGET = GenericBackProj
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

DEFINES += GENERICBACKPROJ_LIBRARY

SOURCES += ../../src/genericbackproj.cpp \
    ../../src/genericbp.cpp

HEADERS += ../../src/genericbackproj.h\
        ../../src/genericbackproj_global.h \
    ../../src/genericbp.h

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
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2

}

unix:mac {
exists($$PWD/../../../../../../external/mac/lib/*NeXus*) {

    message("-lNeXus exists")
    DEFINES += HAVE_NEXUS

    INCLUDEPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/nexus $$PWD/../../../../../../external/mac/include/hdf5
    DEPENDPATH  += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/nexus $$PWD/../../../../../../external/mac/include/hdf5

    LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}
else {
message("-lNeXus does not exists $$HEADERS")
}
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    QMAKE_LIBDIR += $$PWD/../../../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/cfitsio
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH  += $$PWD/../../../../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2

#    exists($$PWD/../../../../../../external/lib64/nexus/*NeXus*) {

#        message("-lNeXus exists")
#        DEFINES += HAVE_NEXUS
#        INCLUDEPATH += $$PWD/../../../../external/include/nexus $$PWD/../../../../external/include/hdf5
#        QMAKE_LIBDIR += $$PWD/../../../../external/lib64/nexus $$PWD/../../../../external/lib64/hdf5

#        LIBS +=  -lNeXus -lNeXusCPP

#        SOURCES += ../src/io/io_nexus.cpp
#        HEADERS += ../include/io/io_nexus.h
#    }

}


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lImagingAlgorithms -lReaderConfig -lReconFramework -lReconAlgorithms

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH  += $$PWD/../../../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/src

INCLUDEPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
DEPENDPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
