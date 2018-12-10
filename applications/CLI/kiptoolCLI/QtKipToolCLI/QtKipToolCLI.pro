#-------------------------------------------------
#
# Project created by QtCreator 2018-12-10T10:46:31
#
#-------------------------------------------------

CONFIG += c++11 console
CONFIG -=app_bundle

TARGET = KipToolCLI
#VERSION = 2.7.0
#TEMPLATE = app


DEFINES += VERSION=\\\"$$VERSION\\\"
DEFINES += QT_DEPRECATED_WARNINGS


CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../Applications
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../Applications/debug

unix:!symbian {
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
    DEPENDPATH += $$PWD/../../../../external/mac/include/ $$PWD/../../../external/mac/include/nexus $$PWD/../../../../external/mac/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../../external/mac/lib/

    LIBS += -lNeXus.1.0.0 -lNeXusCPP.1.0.0


}
else {
message("-lNeXus does not exist $$HEADERS")
}

}

#ICON = kiptool_icon.icns
#RC_ICONS = kiptool_icon.ico

SOURCES +=  main.cpp \
            qtkiptoolcli.cpp \
            ImageIO.cpp \
#            genericconversion.cpp \
#            Reslicer.cpp \
#            mergevolume.cpp \
            Fits2Tif.cpp \
            stdafx.cpp \
            ImagingToolConfig.cpp



HEADERS  += qtkiptoolcli.h \
            ImageIO.h \
#            genericconversion.h \
#            Reslicer.h \
#            mergevolume.h \
            Fits2Tif.h  \
            stdafx.h \
            targetver.h \
            ImagingToolConfig.h





INCLUDEPATH += $$PWD/../../../../frameworks/imageprocessing/ProcessFramework/include
DEPENDPATH += $$PWD/../../../../frameworks/imageprocessing/ProcessFramework/include/

#INCLUDEPATH += $$PWD/../../../../GUI/qt/QtAddons
#DEPENDPATH += $$PWD/../../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../core/modules/ReaderConfig
DEPENDPATH += $$PWD/../../../../core/modules/ReaderConfig

#INCLUDEPATH += $$PWD/../../../../core/modules/ReaderGUI
#DEPENDPATH += $$PWD/../../../../core/modules/ReaderGUI

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/src

LIBS += -L$$PWD/../../../../../lib/ -lkipl -lModuleConfig -lProcessFramework -lReaderConfig  -lImagingAlgorithms


