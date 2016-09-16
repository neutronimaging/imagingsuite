#-------------------------------------------------
#
# Project created by QtCreator 2013-10-30T07:42:56
#
#-------------------------------------------------

QT       += core widgets printsupport

TARGET = QtKipTool
TEMPLATE = app
CONFIG += c++11

CONFIG(release, debug|release): DESTDIR = $$PWD/../../../../../Applications
else:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../../../Applications/debug

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

SOURCES +=  ../../src/main.cpp\
            ../../src/kiptoolmainwindow.cpp \
            ../../src/ImageIO.cpp \
            ../../src/confighistorydialog.cpp

HEADERS  += ../../src/kiptoolmainwindow.h \
            ../../src/ImageIO.h \
            ../../src/confighistorydialog.h

FORMS    += ../../src/kiptoolmainwindow.ui \
            ../../src/confighistorydialog.ui

CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/

LIBS += -lkipl -lModuleConfig -lProcessFramework -lQtAddons -lQtModuleConfigure

INCLUDEPATH += $$PWD/../../../ProcessFramework/include
DEPENDPATH += $$PWD/../../../ProcessFramework/include

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons
DEPENDPATH += $$PWD/../../../../../gui/trunk/qt/QtAddons

INCLUDEPATH += $$PWD/../../../../../gui/trunk/qt/QtModuleConfigure

INCLUDEPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include
DEPENDPATH += $$PWD/../../../../../modules/trunk/ModuleConfig/include

INCLUDEPATH += $$PWD/../../../../../kipl/trunk/kipl/include
DEPENDPATH += $$PWD/../../../../../kipl/trunk/kipl/include
