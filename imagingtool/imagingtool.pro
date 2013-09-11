#-------------------------------------------------
#
# Project created by QtCreator 2013-09-06T16:32:02
#
#-------------------------------------------------

QT       += core gui

TARGET = imagingtool
TEMPLATE = app

QMAKE_CXXFLAGS +=  -fPIC -fopenmp
QMAKE_LFLAGS += -fopenmp


SOURCES += main.cpp\
        imagingtoolmain.cpp \
    findskiplistdialog.cpp \
    ImagingToolConfig.cpp \
    stdafx.cpp \
    Fits2Tif.cpp

HEADERS  += imagingtoolmain.h \
    findskiplistdialog.h \
    ImagingToolConfig.h \
    targetver.h \
    stdafx.h \
    Fits2Tif.h

FORMS    += imagingtoolmain.ui \
    findskiplistdialog.ui



LIBS += -ltiff -lxml2 -lcfitsio -lgomp
INCLUDEPATH += /usr/include/libxml2

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
#else:symbian: LIBS += -lkipl
#else:unix: LIBS += -L$$PWD/../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl

#INCLUDEPATH += $$PWD/../../../src/src/libs/kipl/trunk/kipl/include
#DEPENDPATH += $$PWD/../../../src/src/libs/kipl/trunk/kipl/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../src/libs/kipl/trunk/kipl/kipl-build-desktop-Qt_4_8_1_in_PATH__System__Release/ -lkipl


INCLUDEPATH += $$PWD/../../../src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../src/libs/kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix: LIBS += -L$$PWD/../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtAddons

INCLUDEPATH += $$PWD/../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix: LIBS += -L$$PWD/../../../src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lReconFramework

INCLUDEPATH += $$PWD/../../../src/libs/recon2/trunk/ReconFramework/include
DEPENDPATH += $$PWD/../../../src/libs/recon2/trunk/ReconFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/libs/modules/trunk/ModuleConfig/qt/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/libs/modules/trunk/ModuleConfig/qt/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../src/libs/modules/trunk/ModuleConfig/qt/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../src/libs/modules/trunk/ModuleConfig/include





win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix: LIBS += -L$$PWD/../../../src/src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lReconFramework

INCLUDEPATH += $$PWD/../../../src/src/libs/recon2/trunk/ReconFramework/include
DEPENDPATH += $$PWD/../../../src/src/libs/recon2/trunk/ReconFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../src/src/libs/modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../src/src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../src/src/libs/modules/trunk/ModuleConfig/include
