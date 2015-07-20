#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T14:40:51
#
#-------------------------------------------------

QT       -= gui

TARGET = ImagingAlgorithms
TEMPLATE = lib

CONFIG += c++11

unix {
    QMAKE_CXXFLAGS += -fPIC -O2
    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        LIBS += -L/usr/lib -lxml2
        INCLUDEPATH += /usr/include/libxml2
    }
    else
    {
        INCLUDEPATH += /usr/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
        LIBS += -L/opt/local/lib/ -lxml2
    }


}
DEFINES += IMAGINGALGORITHMS_LIBRARY NOMINMAX

SOURCES += \
    ../../src/StripeFilter.cpp \
#    ../../src/stdafx.cpp \
    ../../src/SpotClean.cpp \
    ../../src/ReferenceImageCorrection.cpp \
    ../../src/PolynomialCorrection.cpp \
    ../../src/ImagingException.cpp \
    ../../src/TranslateProjections.cpp \
    ../../src/MorphSpotClean.cpp \
    ../../src/pixelinfo.cpp \
    ../../src/gammaclean.cpp

HEADERS += \
 #   ../../include/targetver.h \
    ../../include/StripeFilter.h \
 #   ../../include/stdafx.h \
    ../../include/SpotClean.h \
    ../../include/ReferenceImageCorrection.h \
    ../../include/PolynomialCorrection.h \
    ../../include/ImagingException.h \
 #   ../../src/targetver.h \
 #   ../../src/stdafx.h \
    ../../include/TranslateProjections.h \
    ../../include/MorphSpotClean.h \
    ../../include/pixelinfo.h \
    ../../include/ImagingAlgorithms_global.h \
    ../../include/gammaclean.h \

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

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../../external/src/linalg ../../../../../external/include ../../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../../external/lib64

#    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}
else {
    LIBS += -L/usr/lib -lxml2 -ltiff
    INCLUDEPATH += /usr/include/libxml2
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug -lkipl
else:unix:CONFIG(release, debug|release) LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release -lkipl
else:unix:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
