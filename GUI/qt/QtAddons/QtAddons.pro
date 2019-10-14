#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T17:38:34
#
#-------------------------------------------------

QT       += core widgets charts printsupport

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
    plotpainter.cpp \
    imagepainter.cpp \
    imageviewerwidget.cpp \
    qglyphs.cpp \
    reportgeneratorbase.cpp \
    imageviewerinfodialog.cpp \
    qmarker.cpp \
    qtlogviewer.cpp \
    roiitemdlg.cpp \
    roiwidget.cpp \
    roimanager.cpp \
    uxroiwidget.cpp \
    loggingdialog.cpp \
    comparevolumeviewer.cpp \
    plotwidget.cpp \
    setgraylevelsdlg.cpp \
    callout.cpp \
    facestyles.cpp \
    plotcursor.cpp \
    roidialog.cpp



HEADERS += QtAddons_global.h \
    plotter.h \
    plotpainter.h \
    imagepainter.h \
    imageviewerwidget.h \
    qglyphs.h \
    reportgeneratorbase.h \
    imageviewerinfodialog.h \
    qmarker.h \
    qtlogviewer.h \
    roiitemdlg.h \
    roiwidget.h \
    roimanager.h \
    uxroiwidget.h \
    loggingdialog.h \
    comparevolumeviewer.h \
    plotwidget.h \
    setgraylevelsdlg.h \
    callout.h \
    facestyles.h \
    plotcursor.h \
    roidialog.h

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
    qtlogviewer.ui \
    roiitemdlg.ui \
    roiwidget.ui \
    roimanager.ui \
    uxroiwidget.ui \
    loggingdialog.ui \
    comparevolumeviewer.ui \
    plotwidget.ui \
    setgraylevelsdlg.ui \
    roidialog.ui
