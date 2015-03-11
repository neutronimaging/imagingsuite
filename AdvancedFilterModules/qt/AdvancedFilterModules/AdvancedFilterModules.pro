#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T07:55:35
#
#-------------------------------------------------
QT       += core

TARGET = AdvancedFilterModules
TEMPLATE = lib

DEFINES += ADVANCEDFILTERMODULES_LIBRARY

SOURCES += \
    ../../src/stdafx.cpp \
    ../../src/ISSfilterModule.cpp \
    ../../src/dllmain.cpp \
    ../../src/AdvancedFilterModules.cpp \
    ../../src/NonLinDiffusion.cpp

HEADERS += \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../src/ISSfilterModule.h \
    ../../src/AdvancedFilterModules.h \
    ../../src/NonLinDiffusion.h \
    ../../src/AdvancedFilterModules_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE9202092
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = AdvancedFilterModules.dll
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
        INCLUDEPATH += /opt/local/include
        QMAKE_LIBDIR += /opt/local/lib
        QMAKE_INFO_PLIST = Info.plist
        ICON = muhrec3.icns
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
    QMAKE_CXXFLAGS += /openmp /O2 /DNOMINMAX
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Debug/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Release/ -lProcessFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Debug/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kiplQt5-Release/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl


INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include
