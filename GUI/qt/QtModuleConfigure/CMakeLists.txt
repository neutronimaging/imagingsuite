cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "QtModuleConfigure")
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

find_package(QT NAMES Qt6 COMPONENTS Widgets Charts PrintSupport REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts PrintSupport)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts PrintSupport REQUIRED)
message("Using Qt${QT_VERSION_MAJOR}")

if (APPLE)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../../ExternalDependencies/macos/include
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )
endif()

if (WIN32)
include_directories(
    ../QtAddons
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../../ExternalDependencies/windows/include
    ../../../../ExternalDependencies/windows/include/nexus
    ../../../../ExternalDependencies/windows/include/libxml2
    ../../../../ExternalDependencies/windows/include/cfitsio
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )
endif()

if (UNIX)
include_directories(
    /usr/include/libxml2
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )
endif()

set(SRC_FILES
    qtmoduleconfigure.cpp
    modulechainconfiguratorwidget.cpp
    singlemoduleconfiguratorwidget.cpp
    ApplicationBase.cpp
    ModuleConfigurator.cpp
    AddModuleDialog.cpp
    ConfiguratorDialogBase.cpp
    singlemodulesettingsdialog.cpp
    stdafx.cpp
) 

set (INC_FILES
    qtmoduleconfigure.h
    QtModuleConfigure_global.h
    modulechainconfiguratorwidget.h
    singlemoduleconfiguratorwidget.h
    ApplicationBase.h
    ModuleConfigurator.h
    AddModuleDialog.h
    QListWidgetModuleItem.h
    ConfiguratorDialogBase.h
    stdafx.h
    singlemodulesettingsdialog.h
)


set (UI_FILES
    singlemodulesettingsdialog.ui
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  ${UI_FILES}
)

target_link_libraries(${TARGET_NAME} 
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    PUBLIC
    kipl
    ModuleConfig
    )

target_compile_definitions(${TARGET_NAME} PRIVATE QTMODULECONFIGURE_LIBRARY)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
