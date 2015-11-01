#-------------------------------------------------
#
# Project created by QtCreator 2015-10-27T18:11:30
#
#-------------------------------------------------

QT       -= gui

CONFIG += c++11

TARGET = ReaderConfig
TEMPLATE = lib

DEFINES += READERCONFIG_LIBRARY

SOURCES += readerconfig.cpp \
    datasetbase.cpp \
    buildfilelist.cpp

HEADERS += readerconfig.h\
        readerconfig_global.h \
    datasetbase.h \
    buildfilelist.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug -lkipl
else:unix:CONFIG(release, debug|release) LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix: LIBS += -L$$PWD/../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl

INCLUDEPATH += $$PWD/../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../kipl/trunk/kipl/include
