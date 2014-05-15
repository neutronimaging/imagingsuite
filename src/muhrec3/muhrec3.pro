#-------------------------------------------------
#
# Project created by QtCreator 2013-04-07T13:45:32
#
#-------------------------------------------------

QT       += core gui

TARGET = muhrec3
TEMPLATE = app

QMAKE_CXXFLAGS += -fopenmp -O2
QMAKE_LFLAGS += -lgomp

SOURCES += main.cpp\
        muhrecmainwindow.cpp \
    MuhrecInteractor.cpp \
    configuregeometrydialog.cpp \
    findskiplistdialog.cpp \
    recondialog.cpp \
    PreProcModuleConfigurator.cpp \
    stdafx.cpp \
    viewgeometrylistdialog.cpp

HEADERS  += muhrecmainwindow.h \
    MuhrecInteractor.h \
    configuregeometrydialog.h \
    findskiplistdialog.h \
    recondialog.h \
    PreProcModuleConfigurator.h \
    stdafx.h \
    viewgeometrylistdialog.h

FORMS    += muhrecmainwindow.ui \
    configuregeometrydialog.ui \
    findskiplistdialog.ui \
    recondialog.ui \
    viewgeometrylistdialog.ui

LIBS += -L/usr/lib -lxml2 -lgomp

INCLUDEPATH += /usr/include/libxml2

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtModuleConfigure
else:symbian: LIBS += -lQtModuleConfigure
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtModuleConfigure
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtModuleConfigure

INCLUDEPATH += $$PWD/../../../../gui/trunk/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../gui/trunk/qt/QtModuleConfigure


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lReconFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/qt/ReconFramework-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lReconFramework
else:symbian: LIBS += -lReconFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/ReconFramework-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lReconFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../src/libs/recon2/trunk/ReconFramework/ReconFramework-build_Qt_4_8_1_for_GCC__Qt_SDK__Debug/ -lReconFramework

INCLUDEPATH += $$PWD/../../../../src/libs/recon2/trunk/ReconFramework/include
DEPENDPATH += $$PWD/../../../../src/libs/recon2/trunk/ReconFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../kipl/trunk/kipl/include
