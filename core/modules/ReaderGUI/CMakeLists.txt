cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ReaderGUI")
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

find_package(QT NAMES Qt6 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts )

#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts PrintSupport REQUIRED)
message("Using Qt${QT_VERSION_MAJOR}")

if (APPLE)
include_directories(
    ../QtAddons
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
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
    ../../../GUI/qt/QtAddons
    ../../../../ExternalDependencies/windows/include
    ../../../../ExternalDependencies/windows/include/nexus
    ../../../../ExternalDependencies/windows/include/libxml2
    ../../../../ExternalDependencies/windows/include/cfitsio
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )
endif()

if (UNIX)
include_directories(
    ../../../GUI/qt/QtAddons
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )
endif()

set(SRC_FILES
    readergui.cpp
    readerconfiglistwidget.cpp
    addloaderdialog.cpp
    findskiplistdialog.cpp
    readerform.cpp
    singlefileform.cpp
) 

set (INC_FILES
    readergui.h
    readergui_global.h
    readerconfiglistwidget.h
    addloaderdialog.h
    findskiplistdialog.h
    readerform.h
    singlefileform.h
)

set (UI_FILES
    addloaderdialog.ui
    findskiplistdialog.ui
    readerform.ui
    singlefileform.ui
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  ${UI_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_link_libraries(${TARGET_NAME} 
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
#    Qt${QT_VERSION_MAJOR}::Charts
    PUBLIC
    kipl
    ReaderConfig
    QtAddons
    )

target_compile_definitions(${TARGET_NAME} PRIVATE READERGUI_LIBRARY)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
