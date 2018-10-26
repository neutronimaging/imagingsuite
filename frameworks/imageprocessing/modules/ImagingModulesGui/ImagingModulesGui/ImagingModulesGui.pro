#-------------------------------------------------
#
# Project created by QtCreator 2018-09-13T10:10:46
#
#-------------------------------------------------

QT       += core widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImagingModulesGUI
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../lib/debug

DEFINES += IMAGINGMODULESGUI_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += imagingmodulesgui.cpp \
           bblognormdlg.cpp \
           WaveletRingCleanDlg.cpp

HEADERS  += imagingmodulesgui.h \
            bblognormdlg.h \
            imagingmodulesgui_global.h \
            WaveletRingCleanDlg.h

FORMS    += bblognormdlg.ui \
             WaveletRingCleanDlg.ui

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -fPIC -O2
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
    INCLUDEPATH += /usr/include/libxml2
}

unix:macx {
    QMAKE_MAC_SDK = macosx10.12
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
    INCLUDEPATH += /opt/local/include/libxml2

}


#symbian {
#    MMP_RULES += EXPORTUNFROZEN
#    TARGET.UID3 = 0xE2A168C3
#    TARGET.CAPABILITY =
#    TARGET.EPOCALLOWDLLDATA = 1
#    addFiles.sources = ImagingModules.dll
#    addFiles.path = !:/sys/bin
#    DEPLOYMENT += addFiles
#}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}


win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../../external/src/linalg
    INCLUDEPATH += $$PWD/../../../../../external/include
    INCLUDEPATH += $$PWD/../../../../../external/include/cfitsio
    INCLUDEPATH += $$PWD/../../../../../external/include/libxml2

    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2 /DNOMINMAX
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../lib/debug/

LIBS += -lkipl -lQtModuleConfigure -lImagingAlgorithms  -lQtAddons -lProcessFramework -lModuleConfig -lReaderConfig -lImagingModules


INCLUDEPATH += $$PWD/../../../../../GUI/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

INCLUDEPATH += $$PWD/../../ImagingModules/src
DEPENDPATH += $$PWD/../../ImagingModules/src

INCLUDEPATH += $$PWD/../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../core/modules/ReaderConfig
DEPENDPATH += $$PWD/../../../../../core/modules/ReaderConfig

macx: {
INCLUDEPATH += $$PWD/../../../../../external/mac/include $$PWD/../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
DEPENDPATH += $$PWD/../../../../../external/mac/include $$PWD/../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
LIBS += -L$$PWD/../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}





