QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

SOURCES +=  tst_tedgefunction.cpp


INCLUDEPATH += $$PWD/../../kipl/include
DEPENDPATH += $$PWD/../../kipl/src

LIBS += -lkipl # -lFaddeeva

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../external/src/Fadeeva_erf/release/ -lFaddeeva
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../external/src/Fadeeva_erf/debug/ -lFaddeeva
#else:unix: LIBS += -L$$PWD/../../../../external/src/Fadeeva_erf/ -lFaddeeva

#INCLUDEPATH += $$PWD/../../../../external/src/Fadeeva_erf
#DEPENDPATH += $$PWD/../../../../external/src/Fadeeva_erf
