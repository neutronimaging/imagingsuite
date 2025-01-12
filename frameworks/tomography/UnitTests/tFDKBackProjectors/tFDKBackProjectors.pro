QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

CONFIG += c++11

TEMPLATE = app

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../lib/debug

SOURCES +=  tst_tfdkbackprojectors.cpp

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

        INCLUDEPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
        DEPENDPATH += $$PWD/../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
        LIBS += -L$$PWD/../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
        LIBS += -L/usr/local/lib64 -L/usr/lib/x86_64-linux-gnu -lNeXus -lNeXusCPP
    }

    LIBS += -ltiff -lxml2

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    QMAKE_LIBDIR += $$PWD/../../../../../ExternalDependencies/windows/lib
    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include/cfitsio
    INCLUDEPATH  += $$PWD/../../../../../ExternalDependencies/windows/include/libxml2

    INCLUDEPATH += $$PWD/../../../../external/include
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../external/lib64

    LIBS += -llibxml2 -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lImagingAlgorithms -lReconFramework -lModuleConfig -lFDKBackProjectors

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtModuleConfigure
DEPENDPATH  += $$PWD/../../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../GUI/qt/QtAddons
DEPENDPATH  += $$PWD/../../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include
DEPENDPATH  += $$PWD/../../../../core/algorithms/ImagingAlgorithms/include

INCLUDEPATH += $$PWD/../../Framework/ReconFramework/include
DEPENDPATH  += $$PWD/../../Framework/ReconFramework/include

INCLUDEPATH += $$PWD/../../Backprojectors/FDKBackProjectors/src
DEPENDPATH  += $$PWD/../../Backprojectors/FDKBackProjectors/src

INCLUDEPATH += $$PWD/../../../../core/kipl/kipl/include
DEPENDPATH  += $$PWD/../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../core/modules/ModuleConfig/include
DEPENDPATH  += $$PWD/../../../../core/modules/ModuleConfig/include
