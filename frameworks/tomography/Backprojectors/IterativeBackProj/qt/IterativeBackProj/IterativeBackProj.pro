#-------------------------------------------------
#
# Project created by QtCreator 2015-02-17T07:15:06
#
#-------------------------------------------------

QT       -= gui

TARGET = IterativeBackProj
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug


DEFINES += ITERATIVEBACKPROJ_LIBRARY

SOURCES += ../../src/iterativebackproj.cpp \
    ../../src/genericbp.cpp \
    ../../src/sirtbp.cpp \
    ../../src/iterativereconbase.cpp

HEADERS += ../../src/iterativebackproj.h\
        ../../src/iterativebackproj_global.h \
    ../../src/genericbp.h \
    ../../src/sirtbp.h \
    ../../src/iterativereconbase.h

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
        ICON = muhrec3.icns
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
        LIBS += -L/usr/local/lib64 -L/usr/lib/x86_64-linux-gnu -lNeXus -lNeXusCPP
    }

    LIBS += -ltiff -lxml2

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    QMAKE_LIBDIR += $$PWD/../../../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/cfitsio
    INCLUDEPATH  += $$PWD/../../../../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH +=  $$PWD/../../../../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lImagingAlgorithms -lReaderConfig -lReconFramework -lReconAlgorithms

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH  += $$PWD/../../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH  += $$PWD/../../../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH  += $$PWD/../../../../Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
DEPENDPATH += $$PWD/../../../../Framework/ReconAlgorithms/ReconAlgorithms
