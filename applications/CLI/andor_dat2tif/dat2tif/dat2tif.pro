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

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_MAC_SDK = macosx10.11
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /usr/local/include
        QMAKE_LIBDIR += /usr/local/lib
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
    INCLUDEPATH += ../../../../external/src/linalg ../../../../external/include ../../../../external/include/cfitsio
    QMAKE_LIBDIR += ../../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2

    LIBS += -llibtiff
}

SOURCES += main.cpp \
    convertconfig.cpp \
    mainwindow.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/trunk/kipl/include

HEADERS += \
    convertconfig.h \
    mainwindow.h

FORMS += \
    mainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/debug/ -lQtAddons
else:unix: LIBS += -L$$PWD/../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/ -lQtAddons

INCLUDEPATH += $$PWD/../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../gui/trunk/qt/QtAddons
