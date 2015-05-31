#-------------------------------------------------
#
# Project created by QtCreator 2013-10-30T07:42:56
#
#-------------------------------------------------

QT       += core widgets printsupport

TARGET = QtKipTool
TEMPLATE = app

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

    LIBS += -ltiff -lxml2 -lcfitsio
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

SOURCES += ../../src/main.cpp\
        ../../src/kiptoolmainwindow.cpp \
    ../../src/ImageIO.cpp \
    ../../src/stdafx.cpp \
    ../../src/confighistorydialog.cpp

HEADERS  += ../../src/kiptoolmainwindow.h \
    ../../src/ImageIO.h \
    ../../src/targetver.h \
    ../../src/stdafx.h \
    ../../src/confighistorydialog.h

FORMS    += ../../src/kiptoolmainwindow.ui \
    ../../src/confighistorydialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Release/release/ -lProcessFramework
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/qt/build-ProcessFramework-Qt5-Release/debug/ -lProcessFramework
else:symbian: LIBS += -lProcessFramework
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/build-ProcessFramework-Qt5-Release/ -lProcessFramework
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../ProcessFramework/build-ProcessFramework-Qt5-Debug/ -lProcessFramework

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/release/ -lQtAddons
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/debug/ -lQtAddons
else:symbian: LIBS += -lQtAddons
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Release/ -lQtAddons
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtAddons-Qt5-Debug/ -lQtAddons

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/release/ -lQtModuleConfigure
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/QtModuleConfigure-build-Qt5-Debug/debug/ -lQtModuleConfigure
else:symbian: LIBS += -lQtModuleConfigure
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Release/ -lQtModuleConfigure
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../gui/trunk/qt/build-QtModuleConfigure-Qt5-Debug/ -lQtModuleConfigure

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtModuleConfigure
DEPENDPATH += $$PWD/../../../../../gui/trunk/qt/QtModuleConfigure-build-Qt_4_8_1_for_GCC__Qt_SDK__Release

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/release/ -lModuleConfig
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/debug/ -lModuleConfig
else:symbian: LIBS += -lModuleConfig
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Release/ -lModuleConfig
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../modules/trunk/ModuleConfig/build-ModuleConfig-Qt5-Debug/ -lModuleConfig

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/release/ -lkipl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/debug/ -lkipl
else:symbian: LIBS += -lkipl
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Release/ -lkipl
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../kipl/trunk/kipl/build-kipl-Qt5-Debug/ -lkipl

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include
