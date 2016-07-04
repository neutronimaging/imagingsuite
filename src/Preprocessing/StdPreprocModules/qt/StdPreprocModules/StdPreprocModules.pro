#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T14:30:11
#
#-------------------------------------------------

QT       -= gui

TARGET = StdPreprocModules
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

DEFINES += STDPREPROCMODULES_LIBRARY

SOURCES += \
    ../../src/WaveletRingClean.cpp \
    ../../src/TranslatedProjectionWeighting.cpp \
    ../../src/StdPreprocModules.cpp \
    ../../src/stdafx.cpp \
    ../../src/SpotRingClean.cpp \
    ../../src/SpotClean2.cpp \
    ../../src/SpotClean.cpp \
    ../../src/ProjectionFilter.cpp \
    ../../src/PreprocEnums.cpp \
    ../../src/PolynomialCorrectionModule.cpp \
    ../../src/NormPlugins.cpp \
    ../../src/MedianMixRingClean.cpp \
    ../../src/ISSfilter.cpp \
    ../../src/GeneralFilter.cpp \
    ../../src/dllmain.cpp \
    ../../src/DataScaler.cpp \
    ../../src/BasicRingClean.cpp \
    ../../src/SinoSpotClean.cpp \
    ../../src/AdaptiveFilter.cpp \
    ../../src/TranslateProjectionsModule.cpp \
    ../../src/MorphSpotCleanModule.cpp \
    ../../src/GammaSpotCleanModule.cpp \
    ../../src/CameraStripeClean.cpp \
    ../../src/robustlognorm.cpp

HEADERS += \
    ../../include/WaveletRingClean.h \
    ../../include/TranslatedProjectionWeighting.h \
    ../../include/StdPreprocModules.h \
    ../../include/SpotRingClean.h \
    ../../include/SpotClean2.h \
    ../../include/SpotClean.h \
    ../../include/ProjectionFilter.h \
    ../../include/PreprocEnums.h \
    ../../include/PolynomialCorrectionModule.h \
    ../../include/NormPlugins.h \
    ../../include/MedianMixRingClean.h \
    ../../include/ISSfilter.h \
    ../../include/GeneralFilter.h \
    ../../include/DataScaler.h \
    ../../include/BasicRingClean.h \
    ../../include/AdaptiveFilter.h \
    ../../include/SinoSpotClean.h \
    ../../src/stdafx.h \
    ../../include/TranslateProjectionsModule.h \
    ../../include/MorphSpotCleanModule.h \
    ../../include/GammaSpotCleanModule.h \
    ../../include/StdPreprocModules_global.h \
    ../../include/CameraStripeClean.h \
    ../../include/robustlognorm.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2EC73DA
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = StdPreprocModules.dll
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


    QMAKE_LFLAGS += -ltiff -lxml2 -lfftw3

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../../../../external/src/linalg
    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../../../../external/include
    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
    DEFINES += NOMINMAX
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../build-ReconFramework-Qt5-Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../build-ReconFramework-Qt5-Debug/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib/ -lkipl -lModuleConfig -lReconFramework -lImagingAlgorithms
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/ -lkipl -lModuleConfig -lReconFramework -lImagingAlgorithms


win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/debug/ -lImagingAlgorithms

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig

win32:CONFIG(release, debug|release):     LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release):  LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include

INCLUDEPATH += $$PWD/../../../../Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../../Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../../qni/trunk/src/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../../../qni/trunk/src/ImagingAlgorithms/include
