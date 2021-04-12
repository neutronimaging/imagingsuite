#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T14:40:51
#
#-------------------------------------------------

QT       -= gui

TARGET = ImagingAlgorithms
TEMPLATE = lib

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR    = $$PWD/../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../lib/debug

win32 {
    message(Building for Windows)
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += "$$PWD/../../../../../external/include" "$$PWD/../../../../../external/include/cfitsio"
    QMAKE_LIBDIR += $$PWD/../../../../../../ExternalDependencies/windows/lib
    QMAKE_LIBDIR += $$PWD/../../../../../external/lib64


    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    LIBS += -llibopenblas
    QMAKE_CXXFLAGS += /openmp /O2
}

unix {
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {

        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        LIBS += -L/usr/lib -lxml2 -ltiff
        INCLUDEPATH += /usr/include/libxml2

    }
    else
    {
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib

        LIBS += -L/opt/local/lib/ -lxml2 -larmadillo -ltiff
    }


}
DEFINES += IMAGINGALGORITHMS_LIBRARY NOMINMAX

SOURCES += \
    ../../src/StripeFilter.cpp \
    ../../src/SpotClean.cpp \
    ../../src/PolynomialCorrection.cpp \
    ../../src/ImagingException.cpp \
    ../../src/TranslateProjections.cpp \
    ../../src/MorphSpotClean.cpp \
    ../../src/normalizeimage.cpp \
    ../../src/pixelinfo.cpp \
    ../../src/gammaclean.cpp \
    ../../src/averageimage.cpp \
    ../../src/piercingpointestimator.cpp \
    ../../src/ReferenceImageCorrection.cpp \
    ../../src/tomocenter.cpp \
    ../../src/vostripeclean.cpp \
    ../../src/projectionfilter.cpp

HEADERS += \
    ../../include/StripeFilter.h \
    ../../include/SpotClean.h \
    ../../include/PolynomialCorrection.h \
    ../../include/ImagingException.h \
    ../../include/TranslateProjections.h \
    ../../include/MorphSpotClean.h \
    ../../include/normalizeimage.h \
    ../../include/pixelinfo.h \
    ../../include/ImagingAlgorithms_global.h \
    ../../include/gammaclean.h \
    ../../include/averageimage.h \
    ../../include/piercingpointestimator.h \
    ../../include/ReferenceImageCorrection.h \
    ../../include/tomocenter.h \
    ../../include/vostripeclean.h \
    ../../include/projectionfilter.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}



CONFIG(release, debug|release)           LIBS += -L$$PWD/../../../../../../lib -lkipl
else:CONFIG(debug, debug|release)        LIBS += -L$$PWD/../../../../../../lib/debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/kipl/include

message($$INCLUDEPATH)
