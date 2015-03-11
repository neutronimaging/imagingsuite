#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T16:21:35
#
#-------------------------------------------------

QT       -= gui

TARGET = StatisticsModules
TEMPLATE = lib

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
}

unix:macx {
    QMAKE_CXXFLAGS += -fPIC -O2
    INCLUDEPATH += /opt/local/include
    QMAKE_LIBDIR += /opt/local/lib
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/build-ProcessFramework-Qt5-Release/ -lProcessFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/build-ProcessFramework-Qt5-Debug/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include
