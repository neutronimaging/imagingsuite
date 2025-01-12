cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "QtImaging")
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    # ../../../../ExternalDependencies/macos/include
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
    # ../../../../ExternalDependencies/windows/include
    # ../../../../ExternalDependencies/windows/include/libxml2
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )
endif()

if (UNIX)
include_directories(
    /usr/include/libxml2
    ../QtAddons
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )
endif()

set(SRC_FILES
    qtimaging.cpp
    pixelsizedlg.cpp
) 

set (INC_FILES
    qtimaging.h
    qtimaging_global.h
    pixelsizedlg.h
)

set (UI_FILES
    pixelsizedlg.ui
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  ${UI_FILES}
)

if (APPLE)


target_link_libraries(${TARGET_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
#    Qt${QT_VERSION_MAJOR}::Charts
    PUBLIC
    kipl
    ReaderConfig
    QtAddons
    ImagingAlgorithms
    )
endif()

if (WIN32)

target_link_libraries(${TARGET_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
#    Qt${QT_VERSION_MAJOR}::Charts
    kipl
    ReaderConfig
    QtAddons
    ImagingAlgorithms
    )
endif()

if (UNIX)

target_link_libraries(${TARGET_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
#    Qt${QT_VERSION_MAJOR}::Charts
    kipl
    ReaderConfig
    QtAddons
    ImagingAlgorithms
    )
endif()

target_compile_definitions(${TARGET_NAME} PRIVATE QTIMAGING_LIBRARY)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
