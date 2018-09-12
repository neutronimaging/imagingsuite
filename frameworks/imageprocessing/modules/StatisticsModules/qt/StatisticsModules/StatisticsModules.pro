#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T16:21:35
#
#-------------------------------------------------

QT       -= gui

TARGET = StatisticsModules
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release):    DESTDIR = $$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../../lib/debug

DEFINES += STATISTICSMODULES_LIBRARY

SOURCES += \
    ../../src/DistanceStatistics.cpp \
    ../../src/ImageHistogram.cpp \
    ../../src/StatisticsModules.cpp

HEADERS += ../../src/statisticsmodules_global.h \
    ../../src/DistanceStatistics.h \
    ../../src/ImageHistogram.h \
    ../../src/StatisticsModules.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp -fPIC -O2
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
    INCLUDEPATH += /usr/include/libxml2
}

unix:macx {
    QMAKE_MAC_SDK = macosx10.12
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
    LIBS += -ltiff -lxml2
    INCLUDEPATH += /usr/include/libxml2
}

macx: {
INCLUDEPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
DEPENDPATH += $$PWD/../../../../../../external/mac/include $$PWD/../../../../../../external/mac/include/hdf5 $$PWD/../../../../../../external/mac/include/nexus
LIBS += -L$$PWD/../../../../../../external/mac/lib/ -lNeXus.1.0.0 -lNeXusCPP.1.0.0
}


CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../lib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lProcessFramework

INCLUDEPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include
DEPENDPATH += $$PWD/../../../../../../frameworks/imageprocessing/ProcessFramework/include

INCLUDEPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../core/modules/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../../core/kipl/kipl/include
