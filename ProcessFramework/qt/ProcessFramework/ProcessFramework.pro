#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T07:19:32
#
#-------------------------------------------------

QT       -= gui

TARGET = ProcessFramework
TEMPLATE = lib

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -lgomp
    LIBS += -lgomp
}


DEFINES += PROCESSFRAMEWORK_LIBRARY

SOURCES += \
    ../../src/stdafx.cpp \
    ../../src/KiplProcessModuleBase.cpp \
    ../../src/KiplProcessConfig.cpp \
    ../../src/KiplModuleItem.cpp \
    ../../src/KiplFrameworkException.cpp \
    ../../src/KiplFactory.cpp \
    ../../src/KiplEngine.cpp \
    ../../src/dllmain.cpp

HEADERS += \
    ../../include/KiplProcessModuleBase.h \
    ../../include/KiplProcessConfig.h \
    ../../include/KiplModuleItem.h \
    ../../include/KiplFrameworkException.h \
    ../../include/KiplFactory.h \
    ../../include/KiplEngine.h \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../include/ProcessFramework_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2AB701B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ProcessFramework.dll
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

LIBS += -L/usr/lib -lxml2 -ltiff
INCLUDEPATH += /usr/include/libxml2


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
