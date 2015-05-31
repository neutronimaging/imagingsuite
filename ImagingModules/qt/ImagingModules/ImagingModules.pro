#-------------------------------------------------
#
# Project created by QtCreator 2013-12-05T10:46:39
#
#-------------------------------------------------

QT       += core

TARGET = ImagingModules
TEMPLATE = lib

DEFINES += IMAGINGMODULES_LIBRARY

SOURCES += ../../src/imagingmodules.cpp \
    ../../src/translateprojectionmodule.cpp

HEADERS += ../../src/imagingmodules.h\
        ../../src/ImagingModules_global.h \
    ../../src/translateprojectionmodule.h \
    ../../src/ImagingModules_global.h

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

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2A168C3
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ImagingModules.dll
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


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/ProcessFramework-build_Qt_4_8_1_Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/build-ProcessFramework-Qt5-Release/ -lProcessFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/build-ProcessFramework-Qt5-Debug/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/release/ -lImagingAlgorithms
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/qt/ImagingAlgorithms-build-Qt_4_8_1_for_GCC__Qt_SDK__Release/debug/ -lImagingAlgorithms
else:symbian: LIBS += -lImagingAlgorithms
else:unix:CONFIG(release, debug|release):  LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Release/ -lImagingAlgorithms
else:unix:CONFIG(debug, debug|release):    LIBS += -L$$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/build-ImagingAlgorithms-Qt5-Debug/ -lImagingAlgorithms

INCLUDEPATH += $$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/include
DEPENDPATH += $$PWD/../../../../../qni/trunk/src/ImagingAlgorithms/src


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
