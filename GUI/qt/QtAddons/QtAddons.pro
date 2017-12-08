#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T17:38:34
#
#-------------------------------------------------

QT       += core widgets printsupport

TARGET = QtAddons
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -fPIC -O2
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}

unix:macx {
    QMAKE_MAC_SDK = macosx10.12
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../external/include .
    LIBPATH += ../../../external/lib64
    QMAKE_CXXFLAGS += /openmp /O2
}

DEFINES += QTADDONS_LIBRARY

SOURCES += plotter.cpp \
    imagepainter.cpp \
    imageviewerwidget.cpp \
    qglyphs.cpp \
    plotpainter.cpp \
    plotwidget.cpp \
    reportgeneratorbase.cpp \
    imageviewerinfodialog.cpp \
    qmarker.cpp \
    qtlogviewer.cpp



HEADERS += QtAddons_global.h \
    plotter.h \
    imagepainter.h \
    imageviewerwidget.h \
    qglyphs.h \
    plotpainter.h \
    plotwidget.h \
    reportgeneratorbase.h \
    imageviewerinfodialog.h \
    qmarker.h \
    qtlogviewer.h

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

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib -lkipl
else:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../lib/debug -lkipl

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../core/kipl/kipl/include

FORMS += \
    imageviewerinfodialog.ui \
    qtlogviewer.ui
