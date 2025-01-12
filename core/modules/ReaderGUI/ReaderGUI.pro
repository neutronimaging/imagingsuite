#-------------------------------------------------
#
# Project created by QtCreator 2015-10-29T21:36:13
#
#-------------------------------------------------

QT       += widgets charts

TARGET = ReaderGUI
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

DEFINES += READERGUI_LIBRARY

SOURCES += readergui.cpp \
    readerconfiglistwidget.cpp \
    addloaderdialog.cpp \
    findskiplistdialog.cpp \
    readerform.cpp \
    singlefileform.cpp

HEADERS += readergui.h\
        readergui_global.h \
    readerconfiglistwidget.h \
    addloaderdialog.h \
    findskiplistdialog.h \
    readerform.h \
    singlefileform.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix {
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp -lxml2
        INCLUDEPATH += /usr/include/libxml2
        INCLUDEPATH += /usr/include/cfitsio
        DEFINES += HAVE_NEXUS
        LIBS += -L/usr/local/lib64 -L/usr/lib/x86_64-linux-gnu
        LIBS += -lNeXus -lNeXusCPP
    }

    unix:macx {
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
        INCLUDEPATH += /opt/local/include/libxml2
    }

    LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff -lfftw3 -lfftw3f -lcfitsio
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH  += $$PWD/../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH  += $$PWD/../../../../ExternalDependencies/windows/include/cfitsio
    QMAKE_LIBDIR += $$PWD/../../../../ExternalDependencies/windows/lib
    INCLUDEPATH += $$PWD/../../../external/include
    QMAKE_LIBDIR += $$PWD/../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2

    LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib/debug

LIBS += -lkipl -lReaderConfig -lQtAddons

INCLUDEPATH += $$PWD/../ReaderConfig
DEPENDPATH += $$PWD/../ReaderConfig

INCLUDEPATH += $$PWD/../../kipl/kipl/include
DEPENDPATH += $$PWD/../../kipl/kipl/include

INCLUDEPATH += $$PWD/../../algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../GUI/qt/QtAddons

FORMS += \
    addloaderdialog.ui \
    findskiplistdialog.ui \
    readerform.ui \
    singlefileform.ui
