QT -= gui console

CONFIG += c++11 console
CONFIG -= app_bundle

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

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
        INCLUDEPATH += /opt/local/include

        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
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
    INCLUDEPATH += $$PWD/../../../../ExternalDependencies/windows/include
    INCLUDEPATH += $$PWD/../../../../ExternalDependencies/windows/include/cfitsio
    INCLUDEPATH += $$PWD/../../../../ExternalDependencies/windows/include/libxml2
    QMAKE_LIBDIR += $$PWD/../../../../ExternalDependencies/windows/lib

    INCLUDEPATH += $$PWD/../../../external/src/linalg $$PWD/../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += \
        main.cpp

LIBS += -L$$PWD/../../../../lib/ -lkipl -lReaderConfig -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include $$PWD/../../../core/modules/ReaderConfig
DEPENDPATH += $$PWD/../../../core/kipl/kipl/src


INCLUDEPATH += $$PWD/../../../../
DEPENDPATH += $$PWD/../../../../


INCLUDEPATH += $$PWD/../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../core/algorithms/ImagingAlgorithms/include
