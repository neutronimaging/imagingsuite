#-------------------------------------------------
#
# Project created by QtCreator 2019-07-16T14:59:10
#
#-------------------------------------------------

QT       -= gui

TARGET = AdvancedFilters
TEMPLATE = lib

CONFIG(release, debug|release): DESTDIR    = $$PWD/../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../lib/debug

DEFINES += ADVANCEDFILTERS_LIBRARY

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
        advancedfilterexception.cpp \
        filterenums.cpp

HEADERS += \
        AnisoDiff.h \
        ISSfilter.h \
        ISSfilter2D.h \
        ISSfilterOrig3D.h \
        ISSfilterQ3D.h \
        ISSfilterQ3Dp.h \
        NonLinDiffAOS.h \
        advancedfilterexception.h \
        advancedfilters.h \
        advancedfilters_global.h  \
        diff_filterbase.h \
        filterenums.h \
        lambdaest.h \
        separabelfilters.h \
        shock_filter.h \
        ISSfilter.hpp \
        ISSfilterQ3D.hpp \
        ISSfilterQ3Dp.hpp


unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32 {
    message(Building for Windows)
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }

    INCLUDEPATH += "$$PWD/../../../external/src/linalg" "$$PWD/../../../external/include" "$$PWD/../../../external/include/cfitsio"
    QMAKE_LIBDIR += $$PWD/../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

unix {
    target.path = /usr/lib
    INSTALLS += target

    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += "../../../external/src/linalg"

    unix:!macx {

        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
        LIBS += -L/usr/lib -lxml2 -ltiff
        INCLUDEPATH += /usr/include/libxml2

    }
    else
    {
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib

        LIBS += -L/opt/local/lib/ -lxml2 -ltiff
    }


}
CONFIG(release, debug|release)      LIBS += -L$$PWD/../../../../lib
else:CONFIG(debug, debug|release)   LIBS += -L$$PWD/../../../../lib/debug/

LIBS += -lkipl

INCLUDEPATH += $$PWD/../../kipl/kipl/include
