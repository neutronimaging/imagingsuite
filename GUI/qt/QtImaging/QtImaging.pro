#-------------------------------------------------
#
# Project created by QtCreator 2018-06-25T07:52:23
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = QtImaging
TEMPLATE = lib

CONFIG += c++11

DEFINES += QTIMAGING_LIBRARY

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -fPIC -O2
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
    INCLUDEPATH += /usr/include/libxml2
}

unix:macx {
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
}

macx: {
    INCLUDEPATH += $$PWD/../../../external/mac/include
    INCLUDEPATH += /opt/local/include
    INCLUDEPATH += /opt/local/include/libxml2

    DEPENDPATH += $$PWD/../../../external/mac/include
    LIBS += -L$$PWD/../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../external/include .
    LIBPATH += ../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

SOURCES += \
        qtimaging.cpp \
    pixelsizedlg.cpp

HEADERS += \
        qtimaging.h \
        qtimaging_global.h \ 
    pixelsizedlg.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    pixelsizedlg.ui

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib -lkipl -lQtAddons -lReaderConfig
else:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../lib/debug -lkipl -lQtAddons -lReaderConfig

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../core/modules/ReaderConfig/
DEPENDPATH += $$PWD/../../../core/modules/ReaderConfig/

INCLUDEPATH += $$PWD/../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../GUI/qt/QtAddons
