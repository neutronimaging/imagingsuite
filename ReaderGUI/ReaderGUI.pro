#-------------------------------------------------
#
# Project created by QtCreator 2015-10-29T21:36:13
#
#-------------------------------------------------

QT       += widgets

CONFIG += c++11
TARGET = ReaderGUI
TEMPLATE = lib

DEFINES += READERGUI_LIBRARY

SOURCES += readergui.cpp \
    readerconfiglistwidget.cpp \
    addloaderdialog.cpp

HEADERS += readergui.h\
        readergui_global.h \
    readerconfiglistwidget.h \
    addloaderdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-ReaderConfig-Qt5-Release/release/ -lReaderConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-ReaderConfig-Qt5-Release/debug/ -lReaderConfig
else:unix: LIBS += -L$$PWD/../build-ReaderConfig-Qt5-Release/ -lReaderConfig

INCLUDEPATH += $$PWD/../ReaderConfig
DEPENDPATH += $$PWD/../ReaderConfig

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix: LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl

INCLUDEPATH += $$PWD/../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../kipl/trunk/kipl/include

FORMS += \
    addloaderdialog.ui
