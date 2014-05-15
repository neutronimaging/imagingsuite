#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T18:22:19
#
#-------------------------------------------------

QT       -= core gui

TARGET = ClassificationModules
TEMPLATE = lib

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}


DEFINES += CLASSIFICATIONMODULES_LIBRARY

SOURCES += \
    ../../src/stdafx.cpp \
    ../../src/RemoveBackground.cpp \
    ../../src/KernelFuzzyCMeans.cpp \
    ../../src/FuzzyCMeans.cpp \
    ../../src/DoubleThreshold.cpp \
    ../../src/dllmain.cpp \
    ../../src/ClassificationModules.cpp \
    ../../src/BasicThreshold.cpp

HEADERS += \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../src/RemoveBackground.h \
    ../../src/KernelFuzzyCMeans.h \
    ../../src/FuzzyCMeans.h \
    ../../src/DoubleThreshold.h \
    ../../src/ClassificationModules.h \
    ../../src/BasicThreshold.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA26CA15
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ClassificationModules.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

LIBS += -ltiff

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix: LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix: LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/ModuleConfig-build-Qt_4_8_1_Release/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix: LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/kipl-build-Qt_4_8_1_Release/ -lkipl

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include


