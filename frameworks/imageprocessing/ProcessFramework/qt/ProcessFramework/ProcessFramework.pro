#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T07:19:32
#
#-------------------------------------------------

QT       -= gui

TARGET = ProcessFramework
TEMPLATE = lib
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../../lib
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../../lib/debug

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    unix:macx {
        QMAKE_MAC_SDK = macosx10.12
        QMAKE_CXXFLAGS += -fPIC -O2
        INCLUDEPATH += /opt/local/include
        INCLUDEPATH += /opt/local/include/libxml2
        QMAKE_LIBDIR += /opt/local/lib
 #       QMAKE_INFO_PLIST = Info.plist
 #       ICON = muhrec3.icns
    }
    else {
        QMAKE_CXXFLAGS += -fPIC -fopenmp -O2
        QMAKE_LFLAGS += -lgomp
        LIBS += -lgomp
    }

    LIBS += -ltiff -lxml2
    INCLUDEPATH += /usr/include/libxml2
}

win32 {
    contains(QMAKE_HOST.arch, x86_64):{
        QMAKE_LFLAGS += /MACHINE:X64
    }
    INCLUDEPATH += $$PWD/../../../../../external/src/linalg $$PWD/../../../../../external/include $$PWD/../../../../../external/include/cfitsio $$PWD/../../../../../external/include/libxml2
    QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../../external/lib64

    LIBS += -llibxml2_dll -llibtiff -lcfitsio
    QMAKE_CXXFLAGS += /openmp /O2
}

unix:mac {
exists($$PWD/../../../../../external/mac/lib/*NeXus*) {

    message("-lNeXus exists")
    DEFINES *= HAVE_NEXUS

    INCLUDEPATH += $$PWD/../../../../../external/mac/include/ $$PWD/../../../../../external/mac/include/nexus $$PWD/../../../../../external/mac/include/hdf5
    DEPENDPATH += $$PWD/../../../../../external/mac/include/ $$PWD/../../../../../external/mac/include/nexus $$PWD/../../../../../external/mac/include/hdf5
    QMAKE_LIBDIR += $$PWD/../../../../../external/mac/lib/

    LIBS += -lNeXus.1.0.0 -lNeXusCPP.1.0.0


}
else {
message("-lNeXus does not exists $$HEADERS")
}

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

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../lib/debug/

LIBS+= -lkipl -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../core/kipl/kipl/include
DEPENDPATH += $$PWD/../../../../../core/kipl/kipl/include

INCLUDEPATH += $$PWD/../../../../../core/modules/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../core/modules/ModuleConfig/include
