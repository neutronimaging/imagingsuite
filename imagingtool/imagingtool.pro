#-------------------------------------------------
#
# Project created by QtCreator 2013-09-06T16:32:02
#
#-------------------------------------------------

QT       += core gui

TARGET = imagingtool
TEMPLATE = app


SOURCES += main.cpp\
        imagingtoolmain.cpp \
    findskiplistdialog.cpp \
    ImagingToolConfig.cpp \
    stdafx.cpp

HEADERS  += imagingtoolmain.h \
    findskiplistdialog.h \
    ImagingToolConfig.h \
    targetver.h \
    stdafx.h

FORMS    += imagingtoolmain.ui \
    findskiplistdialog.ui

LIBS += -ltiff -lxml2
INCLUDEPATH += /usr/include/libxml2

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix: LIBS += -L$$PWD/../../../gui/trunk/qt/QtAddons-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lQtAddons

INCLUDEPATH += $$PWD/../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../src/src/libs/kipl/trunk/kipl/qt/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/ -lkipl

INCLUDEPATH += $$PWD/../../../src/src/libs/kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../src/src/libs/kipl/trunk/kipl/include
