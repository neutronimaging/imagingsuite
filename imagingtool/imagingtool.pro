#-------------------------------------------------
#
# Project created by QtCreator 2013-09-06T16:32:02
#
#-------------------------------------------------

QT       += core widgets

TARGET = imagingtool
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
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
        QMAKE_INFO_PLIST = Info.plist
        ICON = muhrec3.icns
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2

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
    findskiplistdialog.cpp \
    ImagingToolConfig.cpp \
    stdafx.cpp \
    Fits2Tif.cpp \
    Reslicer.cpp

HEADERS  += imagingtoolmain.h \
    findskiplistdialog.h \
    ImagingToolConfig.h \
    targetver.h \
    stdafx.h \
    Fits2Tif.h \
    Reslicer.h

FORMS    += imagingtoolmain.ui \
    findskiplistdialog.ui


win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release -lkipl
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../kipl/trunk/kipl/include

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Debug/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Release/ -lReconFramework
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../src/libs/recon2/trunk/ReconFramework/build-ReconFramework-Qt5-Debug/ -lReconFramework

INCLUDEPATH += $$PWD/../../../src/libs/recon2/trunk/ReconFramework/include
DEPENDPATH += $$PWD/../../../src/libs/recon2/trunk/ReconFramework/include

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release -lModuleConfig
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug -lModuleConfig

INCLUDEPATH += $$PWD/../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../src/libs/modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/debug/ -lImagingAlgorithms
else:symbian: LIBS += -lImagingAlgorithms
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/ -lImagingAlgorithms
else:unix:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/ -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../qni/trunk/src/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../qni/trunk/src/ImagingAlgorithms/include
