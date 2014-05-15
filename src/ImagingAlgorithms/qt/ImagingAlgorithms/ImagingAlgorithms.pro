#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T14:40:51
#
#-------------------------------------------------

QT       -= core gui

TARGET = ImagingAlgorithms
TEMPLATE = lib
QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
QMAKE_LFLAGS += -lgomp

DEFINES += IMAGINGALGORITHMS_LIBRARY

SOURCES += \
    ../../src/StripeFilter.cpp \
    ../../src/stdafx.cpp \
    ../../src/SpotClean.cpp \
    ../../src/ReferenceImageCorrection.cpp \
    ../../src/PolynomialCorrection.cpp \
    ../../src/ImagingException.cpp \
    ../../src/TranslateProjections.cpp \
    ../../src/MorphSpotClean.cpp \
    ../../src/pixelinfo.cpp

HEADERS += \
    ../../include/targetver.h \
    ../../include/StripeFilter.h \
    ../../include/stdafx.h \
    ../../include/SpotClean.h \
    ../../include/ReferenceImageCorrection.h \
    ../../include/PolynomialCorrection.h \
    ../../include/ImagingException.h \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../include/TranslateProjections.h \
    ../../include/MorphSpotClean.h \
    ../../include/pixelinfo.h

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

LIBS += -lgomp


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../kipl/trunk/kipl/include
