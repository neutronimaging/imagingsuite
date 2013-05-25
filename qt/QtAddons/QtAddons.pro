#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T17:38:34
#
#-------------------------------------------------

QT       += svg

TARGET = QtAddons
TEMPLATE = lib

CMAKE_CXX_FLAGS += "-fPIC -fopenmp"

DEFINES += QTADDONS_LIBRARY

SOURCES += qtlogviewer.cpp \
    plotter.cpp \
    imagepainter.cpp \
    imageviewerwidget.cpp \
    qglyphs.cpp



HEADERS += qtlogviewer.h\
        QtAddons_global.h \
    plotter.h \
    imagepainter.h \
    imageviewerwidget.h \
    qglyphs.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build-desktop-Desktop_Qt_4_8_1_for_GCC__Qt_SDK__Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../src/src/libs/kipl/trunk/kipl/include
