#-------------------------------------------------
#
# Project created by QtCreator 2014-11-17T07:54:02
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_configbasetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /usr/local/include
        QMAKE_LIBDIR += /usr/local/lib
        QMAKE_INFO_PLIST = Info.plist
        ICON = muhrec3.icns
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
    INCLUDEPATH += $$PWD/../../../../external/src/linalg $$PWD/../../../../external/include $$PWD/../../../../external/include/cfitsio $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += tst_configbasetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/trunk/kipl/src

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/debug/ -lReconFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/ -lReconFramework
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Debug/ -lReconFramework

INCLUDEPATH += $$PWD/../../../../src/libs/recon2/trunk/ReconFramework/include
DEPENDPATH += $$PWD/../../../../src/libs/recon2/trunk/ReconFramework/src

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../build-ModuleConfig-Qt5-Release/debug/ -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../build-ModuleConfig-Qt5-Debug/ -lModuleConfig
INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../src
