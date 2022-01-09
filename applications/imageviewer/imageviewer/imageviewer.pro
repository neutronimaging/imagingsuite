#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T15:42:46
#
#-------------------------------------------------

QT       += core widgets printsupport charts

TARGET = ImageViewer
TEMPLATE = app

CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../Applications
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../Applications/debug

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
        INCLUDEPATH += $$PWD/../../../external/mac/include
        DEPENDPATH += $$PWD/../../../external/mac/include
        LIBS += -L$$PWD/../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
        INCLUDEPATH += /usr/include/cfitsio
        LIBS += -L/usr/local/lib64 -L/usr/lib/x86_64-linux-gnu -lNeXus -lNeXusCPP
        DEFINES += HAVE_NEXUS
    }

    LIBS += -ltiff -lxml2 -lcfitsio

}


win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH  += $$PWD/../../../../ExternalDependencies/windows/include
    INCLUDEPATH  += $$PWD/../../../../ExternalDependencies/windows/include/libxml2
    INCLUDEPATH  += $$PWD/../../../../ExternalDependencies/windows/include/cfitsio
    QMAKE_LIBDIR += $$PWD/../../../../ExternalDependencies/windows/lib
    INCLUDEPATH  += $$PWD/../../../external/include
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

ICON = viewer_icon.icns
RC_ICONS = viewer_icon.ico

SOURCES += main.cpp\
        viewermainwindow.cpp \
    saveasdialog.cpp

HEADERS  += viewermainwindow.h \
    saveasdialog.h

FORMS    += viewermainwindow.ui \
    saveasdialog.ui

CONFIG(release, debug|release):      LIBS += -L$$PWD/../../../../lib
else:CONFIG(debug, debug|release):   LIBS += -L$$PWD/../../../../lib/debug

LIBS += -lkipl -lImagingAlgorithms -lQtAddons -lReaderConfig

INCLUDEPATH += $$PWD/../../../core/modules/ReaderConfig
DEPENDPATH += $$PWD/../../../core/modules/ReaderConfig

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../core/kipl/kipl/src

INCLUDEPATH += $$PWD/../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../GUI/qt/QtAddons

unix:!mac {
    message("-lNeXus exists")
    DEFINES += HAVE_NEXUS
    LIBS += -L/usr/local/lib64 -L/usr/lib/x86_64-linux-gnu
    LIBS += -lNeXus -lNeXusCPP
}

unix:mac {
exists($$PWD/../../../external/mac/lib/*NeXus*) {
    DEFINES += HAVE_NEXUS

    INCLUDEPATH += $$PWD/../../../external/mac/include $$PWD/../../../external/mac/include/nexus $$PWD/../../../../external/mac/include/hdf5
    DEPENDPATH += $$PWD/../../../external/mac/include $$PWD/../../../external/mac/include/nexus $$PWD/../../../../external/mac/include/hdf5

    LIBS += -L$$PWD/../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0


}
else {
message("-lNeXus does not exists $$HEADERS")
}

}

win32 {

exists($$PWD/../../../external/lib64/nexus/*NeXus*) {

    message("-lNeXus exists")
    DEFINES += HAVE_NEXUS
    INCLUDEPATH += $$PWD/../../../external/include/nexus $$PWD/../../../external/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../external/lib64/nexus $$PWD/../../../external/lib64/hdf5

    LIBS +=  -lNeXus -lNeXusCPP
}
}

