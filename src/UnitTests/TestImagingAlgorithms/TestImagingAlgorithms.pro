#-------------------------------------------------
#
# Project created by QtCreator 2013-10-24T17:17:21
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_testmorphspotclean
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_testmorphspotclean.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lImagingAlgorithms
else:symbian: LIBS += -lImagingAlgorithms
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/ -lImagingAlgorithms
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/ -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../ImagingAlgorithms/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../src/libs/kipl/trunk/kipl/kipl-build_Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/src
