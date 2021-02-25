#QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../Applications
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../Applications/debug

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

VERSION = 4.3

DEFINES += VERSION=\\\"$$VERSION\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
#        INCLUDEPATH += $$PWD/../../../gui/qt/QtAddons # due to strange ordering during ui build...
        INCLUDEPATH += /opt/local/include

        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib

#        QMAKE_INFO_PLIST = Info.plist

    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2

    INCLUDEPATH += $$PWD/../../../external/src/linalg
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../external/src/linalg $$PWD/../../../external/include $$PWD/../../../external/include/cfitsio $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += \
        main.cpp \
    muhreccli.cpp \
        stdafx.cpp

HEADERS += \
    muhreccli.h \
    stdafx.h

CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib/debug

LIBS += -lkipl -lModuleConfig -lReconFramework -lQtAddons -lQtModuleConfigure -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include
DEPENDPATH  += $$PWD/../../../core/kipl/kipl/include

#INCLUDEPATH += $$PWD/../../../GUI/qt/QtModuleConfigure
#DEPENDPATH += $$PWD/../../../GUI/qt/QtModuleConfigure

#INCLUDEPATH += $$PWD/../../../GUI/qt/QtAddons
#DEPENDPATH += $$PWD/../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../frameworks/tomography/Framework/ReconFramework/include
DEPENDPATH  += $$PWD/../../../frameworks/tomography/Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../core/modules/ModuleConfig/include
DEPENDPATH  += $$PWD/../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../core/algorithms/ImagingAlgorithms/src
