#-------------------------------------------------
#
# Project created by QtCreator 2013-10-30T07:42:56
#
#-------------------------------------------------

QT       += core gui

TARGET = QtKipTool
TEMPLATE = app


SOURCES += ../../src/main.cpp\
        ../../src/kiptoolmainwindow.cpp \
    ../../src/ImageIO.cpp \
    ../../src/stdafx.cpp \
    ../../src/confighistorydialog.cpp

HEADERS  += ../../src/kiptoolmainwindow.h \
    ../../src/ImageIO.h \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../src/confighistorydialog.h

FORMS    += ../../src/kiptoolmainwindow.ui \
    ../../src/confighistorydialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../../src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../src/libs/kipl/trunk/kipl/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix: LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/ModuleConfig-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../src/libs/modules/trunk/ModuleConfig/include

LIBS += -L/usr/lib -lxml2 -ltiff -lcfitsio -lgomp
INCLUDEPATH += /usr/include/libxml2

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix: LIBS += -L$$PWD/../../../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtAddons

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtModuleConfigure
else:symbian: LIBS += -lQtModuleConfigure
else:unix: LIBS += -L$$PWD/../../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtModuleConfigure

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release
