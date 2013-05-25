#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T14:40:51
#
#-------------------------------------------------

QT       -= core gui

TARGET = ImagingAlgorithms
TEMPLATE = lib
CMAKE_CXX_FLAGS += "-fPIC -fopenmp"

DEFINES += IMAGINGALGORITHMS_LIBRARY

SOURCES += \
    ../../src/StripeFilter.cpp \
    ../../src/stdafx.cpp \
    ../../src/SpotClean.cpp \
    ../../src/ReferenceImageCorrection.cpp \
    ../../src/PolynomialCorrection.cpp \
    ../../src/ImagingException.cpp

HEADERS += \
    ../../include/targetver.h \
    ../../include/StripeFilter.h \
    ../../include/stdafx.h \
    ../../include/SpotClean.h \
    ../../include/ReferenceImageCorrection.h \
    ../../include/PolynomialCorrection.h \
    ../../include/ImagingException.h \
    ../../src/targetver.h \
    ../../src/stdafx.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lkipl


INCLUDEPATH += $$PWD/../../../../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../src/src/libs/kipl/trunk/kipl/include
