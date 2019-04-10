QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle


TEMPLATE = app


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

SOURCES +=  tst_tedgefunction.cpp



INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/src

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../../../external/src/linalg $$PWD/../../../../external/src/Fadeeva_erf
