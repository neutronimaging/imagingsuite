#-------------------------------------------------
#
# Project created by QtCreator 2018-10-18T07:36:15
#
#-------------------------------------------------

QT       += core widgets charts

TARGET = ClassificationModulesGUI
TEMPLATE = lib

CONFIG += c++11

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug
DEFINES += CLASSIFICATIONMODULESGUI_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../src/classificationmodulesgui.cpp \
        ../../src/basicthresholddlg.cpp \
    ../../src/doublethresholddlg.cpp \
    ../../src/removebackgrounddlg.cpp

HEADERS += \
        ../../src/classificationmodulesgui.h \
        ../../src/classificationmodulesgui_global.h \ 
        ../../src/basicthresholddlg.h \
    ../../src/doublethresholddlg.h \
    ../../src/removebackgrounddlg.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    ../../src/basicthresholddlg.ui \
    ../../src/doublethresholddlg.ui \
    ../../src/removebackgrounddlg.ui

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
