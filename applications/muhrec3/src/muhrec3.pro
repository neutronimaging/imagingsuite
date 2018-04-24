#-------------------------------------------------
#
# Project created by QtCreator 2013-04-07T13:45:32
#
#-------------------------------------------------

QT       += core widgets printsupport concurrent

TARGET = muhrec3
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
#        QMAKE_MAC_SDK = macosx10.12
        QMAKE_CXXFLAGS += -fPIC -O2
#        INCLUDEPATH += $$PWD/../../../gui/qt/QtAddons # due to strange ordering during ui build...
        INCLUDEPATH += /opt/local/include

        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib

#        QMAKE_INFO_PLIST = Info.plist

    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2

    INCLUDEPATH += $$PWD/../../../external/src/linalg
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../external/src/linalg $$PWD/../../../external/include $$PWD/../../../external/include/cfitsio $$PWD/../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

ICON = muh4_icon.icns

SOURCES += main.cpp\
        muhrecmainwindow.cpp \
    MuhrecInteractor.cpp \
    configuregeometrydialog.cpp \
    findskiplistdialog.cpp \
    recondialog.cpp \
    PreProcModuleConfigurator.cpp \
    stdafx.cpp \
    viewgeometrylistdialog.cpp \
    preferencesdialog.cpp \
    dialogtoobig.cpp \
    piercingpointdialog.cpp \
    referencefiledlg.cpp

HEADERS  += muhrecmainwindow.h \
    MuhrecInteractor.h \
    configuregeometrydialog.h \
    findskiplistdialog.h \
    recondialog.h \
    PreProcModuleConfigurator.h \
    stdafx.h \
    viewgeometrylistdialog.h \
    preferencesdialog.h \
    dialogtoobig.h \
    piercingpointdialog.h \
    referencefiledlg.h

FORMS    += muhrecmainwindow.ui \
    configuregeometrydialog.ui \
    findskiplistdialog.ui \
    recondialog.ui \
    viewgeometrylistdialog.ui \
    preferencesdialog.ui \
    dialogtoobig.ui \
    piercingpointdialog.ui \
    referencefiledlg.ui

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../lib/ -lkipl -lModuleConfig -lReconFramework -lQtAddons -lQtModuleConfigure -lImagingAlgorithms
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../lib/debug -lkipl -lModuleConfig -lReconFramework -lQtAddons -lQtModuleConfigure -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../GUI/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../frameworks/tomography/Framework/ReconFramework/include
DEPENDPATH += $$PWD/../../../frameworks/tomography/Framework/ReconFramework/src

INCLUDEPATH += $$PWD/../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../core/algorithms/ImagingAlgorithms/src
