#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T17:40:10
#
#-------------------------------------------------

QT       += core widgets

TARGET = InspectorModules
TEMPLATE = lib
CONFIG += c++11

DEFINES += INSPECTORMODULES_LIBRARY

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

SOURCES += \
    CountNANs.cpp \
    InspectorModules.cpp \
    ProjectionInspector.cpp \
    SaveProjections.cpp \
    getimagesize.cpp

HEADERS +=\
        inspectormodules_global.h \
    CountNANs.h \
    InspectorModules.h \
    ProjectionInspector.h \
    SaveProjections.h \
    getimagesize.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_MAC_SDK = macosx10.11
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../../external/src/linalg
    INCLUDEPATH += $$PWD/../../../../../external/include
    INCLUDEPATH += $$PWD/../../../../../external/include/cfitsio
    INCLUDEPATH += $$PWD/../../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib -lkipl -lModuleConfig -lReconFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug -lkipl -lModuleConfig -lReconFramework


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-ReconFramework-Qt5-Debug/debug/ -lReconFramework


INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../Framework/ReconFramework/src
