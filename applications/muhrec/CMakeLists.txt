cmake_minimum_required(VERSION 3.5)
set(TARGET_NAME "MuhRec")
project(${TARGET_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../GUI/qt/QtAddons
    ../../../GUI/qt/QtModuleConfigure
    ../../../core/modules/ReaderGUI
    ../../../external/mac/include/
    ../../../../ExternalDependencies/macos/include
    ../../../../ExternalDependencies/macos/include/nexus
    ../../../external/mac/include/hdf5
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )

set(SRC_FILES
    src/MuhrecInteractor.cpp
    src/moduleconfigprogressdialog.cpp
    src/PreProcModuleConfigurator.cpp
    src/muhrecmainwindow.cpp
    src/ReconReport.cpp
    src/piercingpointdialog.cpp
    src/configuregeometrydialog.cpp
    src/preferencesdialog.cpp
    src/dialogtoobig.cpp
    src/recondialog.cpp
    src/fileconversiondialog.cpp
    src/referencefiledlg.cpp
    src/findskiplistdialog.cpp
    src/stdafx.cpp
    src/globalsettingsdialog.cpp
    src/viewgeometrylistdialog.cpp
    src/main.cpp
)

set(INC_FILES
    src/MuhrecInteractor.h
    src/moduleconfigprogressdialog.h
    src/PreProcModuleConfigurator.h
    src/muhrecmainwindow.h
    src/ReconReport.h
    src/piercingpointdialog.h
    src/configuregeometrydialog.h
    src/preferencesdialog.h
    src/dialogtoobig.h
    src/recondialog.h
    src/fileconversiondialog.h
    src/referencefiledlg.h
    src/findskiplistdialog.h
    src/stdafx.h
    src/globalsettingsdialog.h
    src/viewgeometrylistdialog.h
)

set(UI_FILES
    src/configuregeometrydialog.ui
    src/muhrecmainwindow.ui
    src/dialogtoobig.ui
    src/piercingpointdialog.ui
    src/fileconversiondialog.ui
    src/preferencesdialog.ui
    src/findskiplistdialog.ui
    src/recondialog.ui
    src/globalsettingsdialog.ui
    src/referencefiledlg.ui
    src/moduleconfigprogressdialog.ui
    src/viewgeometrylistdialog.ui
)

message(App Qt${QT_VERSION_MAJOR})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${TARGET_NAME}
        MANUAL_FINALIZATION
        ${SRC_FILES}
        ${INC_FILES}
        ${UI_FILES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET untitled4 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${TARGET_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${TARGET_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${TARGET_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    kipl
    ModuleConfig
    ImagingAlgorithms
    ReaderConfig
    QtAddons
    ReaderGUI
    QtModuleConfigure
)

set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER muhrec.psi.ch
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE

)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${TARGET_NAME})
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/applications
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/applications/${TARGET_NAME}.app/Contents/Frameworks)
