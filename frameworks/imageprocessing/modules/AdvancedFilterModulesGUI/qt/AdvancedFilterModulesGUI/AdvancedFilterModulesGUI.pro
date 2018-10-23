#-------------------------------------------------
#
# Project created by QtCreator 2018-09-21T07:14:35
#
#-------------------------------------------------
QT       += core widgets charts

TARGET = AdvancedFilterModulesGUI
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

DEFINES += ADVANCEDFILTERMODULESGUI_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
        QMAKE_LIBDIR += /opt/local/lib

INCLUDEPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
DEPENDPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
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
   # INCLUDEPATH += $$PWD/../../../../../external/src/linalg $$PWD/../../../../../external/include $$PWD/../../../../../external/include/cfitsio $$PWD/../../../../../external/include/libxml2

    INCLUDEPATH += $$PWD/../../../../../../external/src/linalg
    INCLUDEPATH += $$PWD/../../../../../../external/include
    INCLUDEPATH += $$PWD/../../../../../../external/include/cfitsio
    INCLUDEPATH += $$PWD/../../../../../../external/include/libxml2

    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2 /DNOMINMAX
}

SOURCES += \
        ../../src/advancedfiltermodulesgui.cpp \
    ../../src/issfilterdlg.cpp \
    ../../src/nonlindiffusiondlg.cpp

HEADERS += \
        ../../src/advancedfiltermodulesgui.h \
        ../../src/advancedfiltermodulesgui_global.h \
        ../../src/issfilterdlg.h \
    ../../src/nonlindiffusiondlg.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    ../../src/issfilterdlg.ui \
    ../../src/nonlindiffusiondlg.ui



CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/
LIBS += -lkipl -lModuleConfig -lProcessFramework -lQtModuleConfigure -lQtAddons -lAdvancedFilterModules

INCLUDEPATH += $$PWD/../../../../../../frameworks/imageprocessing/modules/AdvancedFilterModules/src
DEPENDPATH += $$PWD/../../../../../../frameworks/imageprocessing/modules/AdvancedFilterModules/src

INCLUDEPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include
DEPENDPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../GUI/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../../../GUI/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../../../GUI/qt/QtAddons
DEPENDPATH += $$PWD/../../../../../../GUI/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/include
