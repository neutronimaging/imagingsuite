#-------------------------------------------------
#
# Project created by QtCreator 2015-10-29T21:36:13
#
#-------------------------------------------------

QT       += widgets

TARGET = ReaderGUI
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../lib/debug

DEFINES += READERGUI_LIBRARY

SOURCES += readergui.cpp \
    readerconfiglistwidget.cpp \
    addloaderdialog.cpp \
    findskiplistdialog.cpp

HEADERS += readergui.h\
        readergui_global.h \
    readerconfiglistwidget.h \
    addloaderdialog.h \
    findskiplistdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix {
    INCLUDEPATH += "../../../../external/src/linalg"
    QMAKE_CXXFLAGS += -fPIC -O2

    unix:!macx {
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    unix:macx {
        QMAKE_MAC_SDK = macosx10.12
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
    }
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../external/src/linalg ../../../external/include ../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

win32:CONFIG(release, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:win32:CONFIG(debug, debug|release): LIBS += -llibtiff -lcfitsio -lzlib_a -llibfftw3-3 -llibfftw3f-3 -lIphlpapi
else:unix: LIBS +=  -lm -lz -L/opt/usr/lib  -ltiff -lfftw3 -lfftw3f -lcfitsio

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-ReaderConfig-Qt5-Release/release/ -lReaderConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-ReaderConfig-Qt5-Release/debug/ -lReaderConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../lib -lkipl -lReaderConfig -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../lib/debug -lkipl -lReaderConfig -lQtAddons

INCLUDEPATH += $$PWD/../ReaderConfig
DEPENDPATH += $$PWD/../ReaderConfig

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../lib/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../lib/debug/ -lkipl


INCLUDEPATH += $$PWD/../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../gui/trunk/qt/build-QtAddons-Qt5-Release/debug/ -lQtAddons

INCLUDEPATH += $$PWD/../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../gui/trunk/qt/QtAddons

FORMS += \
    addloaderdialog.ui \
    findskiplistdialog.ui
