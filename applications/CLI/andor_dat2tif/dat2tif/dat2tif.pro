#-------------------------------------------------
#
# Project created by QtCreator 2014-11-26T13:25:11
#
#-------------------------------------------------

QT       += core widgets

#QT       -= gui
CONFIG += c++11
TARGET = dat2tif

TEMPLATE = app


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
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib

        LIBS += -lm -lz -ltiff -lfftw3 -lfftw3f -lcfitsio
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    LIBS += -ltiff -lxml2
    INCLUDEPATH += /usr/include/libxml2
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include
    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include/cfitsio
    QMAKE_LIBDIR += $$PWD/../../../../../ExternalDependencies/windows/lib

    INCLUDEPATH += ../../../../external/src/linalg ../../../../external/include
    QMAKE_LIBDIR += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2

    LIBS += -llibtiff
}

SOURCES += main.cpp \
    convertconfig.cpp \
    mainwindow.cpp

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../core/kipl/kipl/include

HEADERS += \
    convertconfig.h \
    mainwindow.h

FORMS += \
    mainwindow.ui


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lQtAddons

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../../GUI/qt/QtAddons
