#-------------------------------------------------
#
# Project created by QtCreator 2013-05-15T11:43:11
#
#-------------------------------------------------

QT       -= gui

TARGET = ReconFramework
TEMPLATE = lib
CONFIG += c++11

DEFINES += RECONFRAMEWORK_LIBRARY

SOURCES += \
 #   ../../src/stdafx.cpp \
    ../../src/ReconHelpers.cpp \
    ../../src/ReconFramework.cpp \
    ../../src/ReconFactory.cpp \
    ../../src/ReconException.cpp \
    ../../src/ReconEngine.cpp \
    ../../src/ReconConfig.cpp \
    ../../src/ProjectionReader.cpp \
    ../../src/PreprocModuleBase.cpp \
    ../../src/ModuleItem.cpp \
    ../../src/InteractionBase.cpp \
#   ../../src/dllmain.cpp \
    ../../src/BackProjectorBase.cpp

HEADERS += \
#    ../../include/targetver.h \
#    ../../include/stdafx.h \
    ../../include/ReconHelpers.h \
    ../../include/ReconFramework.h \
    ../../include/ReconFactory.h \
    ../../include/ReconException.h \
    ../../include/ReconEngine.h \
    ../../include/ReconConfig.h \
    ../../include/ProjectionReader.h \
    ../../include/PreprocModuleBase.h \
    ../../include/ModuleItem.h \
    ../../include/InteractionBase.h \
    ../../include/BackProjectorBase.h \
#    ../../src/stdafx.h \
    ../../include/ReconFramework_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEE6411E8
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ReconFramework.dll
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

    unix:macx {
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include/libxml2
        INCLUDEPATH += /opt/local/include/
        QMAKE_LIBDIR += /opt/local/lib
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        INCLUDEPATH += /usr/include/libxml2
    }

    LIBS += -ltiff -lxml2 -lcfitsio

}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
    QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += ../../../../../../../external/src/linalg
    INCLUDEPATH += ../../../../../../../external/include
    INCLUDEPATH += ../../../../../../../external/include/cfitsio
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../../../external/lib64

    LIBS +=  -llibtiff -lcfitsio -llibxml2_dll
    QMAKE_CXXFLAGS += /openmp /O2
}


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../../../kipl/trunk/kipl/include
