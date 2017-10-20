#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T14:40:51
#
#-------------------------------------------------

QT       -= gui

TARGET = ImagingAlgorithms
TEMPLATE = lib

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../lib/debug

win32 {
    message(Building for Windows)
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += "$$PWD/../../../../../../external/src/linalg" "$$PWD/../../../../../../external/include" "../../../../../../external/include/cfitsio"
    QMAKE_LIBDIR += ../../../../../external/lib64

#    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

unix {
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += "../../../../../../external/src/linalg"

    unix:!macx {

        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        LIBS += -L/usr/lib -lxml2 -ltiff
        INCLUDEPATH += /usr/include/libxml2

    }
    else
    {
  #      QMAKE_MAC_SDK = macosx10.12
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib

        LIBS += -L/opt/local/lib/ -lxml2 -ltiff
    }


}
DEFINES += IMAGINGALGORITHMS_LIBRARY NOMINMAX

SOURCES += \
    ../../src/StripeFilter.cpp \
    ../../src/SpotClean.cpp \
    ../../src/ReferenceImageCorrection.cpp \
    ../../src/PolynomialCorrection.cpp \
    ../../src/ImagingException.cpp \
    ../../src/TranslateProjections.cpp \
    ../../src/MorphSpotClean.cpp \
    ../../src/pixelinfo.cpp \
    ../../src/gammaclean.cpp \
    ../../src/averageimage.cpp \
    ../../src/detectorlagcorrection.cpp \
    ../../src/piercingpointestimator.cpp

HEADERS += \
    ../../include/StripeFilter.h \
    ../../include/SpotClean.h \
    ../../include/ReferenceImageCorrection.h \
    ../../include/PolynomialCorrection.h \
    ../../include/ImagingException.h \
    ../../include/TranslateProjections.h \
    ../../include/MorphSpotClean.h \
    ../../include/pixelinfo.h \
    ../../include/ImagingAlgorithms_global.h \
    ../../include/gammaclean.h \
    ../../include/averageimage.h \
    ../../include/detectorlagcorrection.h \
    ../../include/piercingpointestimator.h


symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE18A5065
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ImagingAlgorithms.dll
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
}



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../lib -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../lib/debug -lkipl
else:unix:CONFIG(release, debug|release) LIBS += -L$$PWD/../../../../../../lib -lkipl
else:unix:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../../../../lib/debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../kipl/trunk/kipl/include

message($$INCLUDEPATH)
