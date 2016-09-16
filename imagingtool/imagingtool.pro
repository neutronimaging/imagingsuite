#-------------------------------------------------
#
# Project created by QtCreator 2013-09-06T16:32:02
#
#-------------------------------------------------

QT       += core widgets

TARGET = imagingtool
TEMPLATE = app
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../Applications
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../Applications/debug

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
#        QMAKE_MAC_SDK = macosx10.11 # there was a version problem in earlier Qt creator adressing the wrong sdk
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
 #       QMAKE_INFO_PLIST = Info.plist
  #      ICON = muhrec3.icns
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
    INCLUDEPATH += $$PWD/../../../external/src/linalg $$PWD/../../../external/include $$PWD/../../../external/include/cfitsio $$PWD/../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += main.cpp\
        imagingtoolmain.cpp \
    ImagingToolConfig.cpp \
    stdafx.cpp \
    Fits2Tif.cpp \
    Reslicer.cpp \
    mergevolumesdialog.cpp \
    mergevolume.cpp \
    reslicerdialog.cpp \
    fileconversiondialog.cpp \
    genericconversion.cpp

HEADERS  += imagingtoolmain.h \
    ImagingToolConfig.h \
    targetver.h \
    stdafx.h \
    Fits2Tif.h \
    Reslicer.h \
    imagingtool_global.h \
    mergevolumesdialog.h \
    mergevolume.h \
    reslicerdialog.h \
    fileconversiondialog.h \
    genericconversion.h

FORMS    += imagingtoolmain.ui \
    mergevolumesdialog.ui \
    reslicerdialog.ui \
    fileconversiondialog.ui \
    genericconversion.ui

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release -lkipl
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../lib/ -lkipl -lQtAddons -lReconFramework -lModuleConfig -lReaderConfig -lReaderGUI -lImagingAlgorithms
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../lib/debug/ -lkipl -lQtAddons -lReconFramework -lModuleConfig -lReaderConfig -lReaderGUI -lImagingAlgorithms

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/debug/ -lQtAddons

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../tomography/trunk/src/build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../tomography/trunk/src/build-ReconFramework-Qt5-Debug/debug/ -lReconFramework

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/debug/ -lImagingAlgorithms

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../modules/trunk/build-ReaderConfig-Qt5-Release/release/ -lReaderConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../modules/trunk/build-ReaderConfig-Qt5-Release/debug/ -lReaderConfig

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../modules/trunk/build-ReaderGUI-Qt5-Release/release/ -lReaderGUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../modules/trunk/build-ReaderGUI-Qt5-Release/debug/ -lReaderGUI

INCLUDEPATH += $$PWD/../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../kipl/trunk/kipl/src

INCLUDEPATH += $$PWD/../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../gui/trunk/qt/QtAddons

INCLUDEPATH += $$PWD/../../../modules/trunk/ReaderConfig
DEPENDPATH += $$PWD/../../../modules/trunk/ReaderConfig

INCLUDEPATH += $$PWD/../../../modules/trunk/ReaderGUI
DEPENDPATH += $$PWD/../../../modules/trunk/ReaderGUI

INCLUDEPATH += $$PWD/../../../tomography/trunk/src/Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../tomography/trunk/src/Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../src/libs/modules/trunk/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../qni/trunk/src/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../qni/trunk/src/ImagingAlgorithms/include

