#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T17:38:34
#
#-------------------------------------------------

QT       += svg

TARGET = QtAddons
TEMPLATE = lib

DEFINES += QTADDONS_LIBRARY

SOURCES += qtlogviewer.cpp

HEADERS += qtlogviewer.h\
        QtAddons_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE28FF4BD
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = QtAddons.dll
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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/release/kipl.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/debug/kipl.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/Release/libkipl.a
