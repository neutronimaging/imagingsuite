#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T11:43:11
#
#-------------------------------------------------

QT       -= gui

TARGET = ReconFramework
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

DEFINES += RECONFRAMEWORK_LIBRARY

SOURCES += \
    ../../src/ReconHelpers.cpp \
    ../../src/ReconFramework.cpp \
    ../../src/ReconFactory.cpp \
    ../../src/ReconException.cpp \
    ../../src/ReconEngine.cpp \
    ../../src/ReconConfig.cpp \
    ../../src/ProjectionReader.cpp \
    ../../src/PreprocModuleBase.cpp \
    ../../src/ModuleItem.cpp \
    ../../src/BackProjectorModuleBase.cpp \
    ../../src/processtiminglogger.cpp

HEADERS += \
    ../../include/ReconHelpers.h \
    ../../include/ReconFramework.h \
    ../../include/ReconFactory.h \
    ../../include/ReconException.h \
    ../../include/ReconEngine.h \
    ../../include/ReconConfig.h \
    ../../include/ProjectionReader.h \
    ../../include/PreprocModuleBase.h \
    ../../include/ModuleItem.h \
    ../../include/ReconFramework_global.h \
    ../../include/BackProjectorModuleBase.h \
    ../../include/processtiminglogger.h \
    ../../src/stdafx.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEE6411E8
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ReconFramework.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include/libxml2
        INCLUDEPATH += /opt/local/include/
        QMAKE_LIBDIR += /opt/local/lib
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
        INCLUDEPATH += /usr/include/cfitsio/
    }

    LIBS += -ltiff -lxml2 -lcfitsio

}

unix:!mac {
    exists(/usr/lib/*NeXus*) | exists(/usr/local/lib64/*NeXus* | exists(/usr/lib/x86_64-linux-gnu/*NeXus*)) {
        message("-lNeXus exists")
        DEFINES += HAVE_NEXUS
        LIBS += -L/usr/local/lib64 -L/usr/lib/x86_64-linux-gnu
        LIBS += -lNeXus -lNeXusCPP
    }
    else {
        message("-lNeXus does not exists $$LIBS")
    }
}

#unix:mac {
#exists(/usr/local/lib/*NeXus*) {

#    message("-lNeXus exists")
#    DEFINES *= HAVE_NEXUS

#     LIBS += -L$$PWD/../../../../../../../../../../usr/local/lib/ -lNeXusCPP.1.0.0 -lNeXus

#    INCLUDEPATH += $$PWD/../../../../../../../../../../usr/local/include
#    DEPENDPATH += $$PWD/../../../../../../../../../../usr/local/include
#}
#else {
#message("-lNeXus does not exists $$HEADERS")
#}

unix:mac {
exists($$PWD/../../../../../../external/mac/lib/*NeXus*) {

    message("-lNeXus exists")
    DEFINES *= HAVE_NEXUS

    INCLUDEPATH += $$PWD/../../../../../../external/mac/include/ $$PWD/../../../../../../external/mac/include/nexus $$PWD/../../../../../../external/mac/include/hdf5
    DEPENDPATH += $$PWD/../../../../../../external/mac/include/ $$PWD/../../../../../../external/mac/include/nexus $$PWD/../../../../../../external/mac/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../../../../external/mac/lib/

    LIBS += -lNeXus.1.0.0 -lNeXusCPP.1.0.0


}
else {
message("-lNeXus does not exists $$HEADERS")
}

}



win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += $$PWD/../../../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH += $$PWD/../../../../../../../ExternalDependencies/windows/include/cfitsio
    LIBPATH     += $$PWD/../../../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH += ../../../../../../external/include
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../../external/lib64

    LIBS += -llibxml2
    LIBS += -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

win32 {
exists($$PWD/../../../../../../external/lib64/nexus/*NeXus*) {

    message("-lNeXus exists")
    DEFINES *= HAVE_NEXUS
    INCLUDEPATH += $$PWD/../../../../../../external/include/nexus $$PWD/../../../../../../external/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../../../../external/lib64/nexus $$PWD/../../../../../../external/lib64/hdf5

    LIBS += -lNeXus -lNeXusCPP

#    SOURCES += $$PWD/../../../../../../core/kipl/kipl/src/io/io_nexus.cpp
#    HEADERS += $$PWD/../../../../../../core/kipl/kipl/include/io/io_nexus.h
}
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug

LIBS+=-lkipl  -lImagingAlgorithms -lModuleConfig -lReaderConfig

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
