cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "QtImaging")
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
    /opt/local/include
    /opt/local/include/libxml2
    /usr/include/libxml2
    ../QtAddons
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../external/mac/include/
    ../../../../ExternalDependencies/macos/include
    ../../../../ExternalDependencies/macos/include/nexus
    ../../../external/mac/include/hdf5
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
    ../../../../ExternalDependencies/windows/include/libxml2
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
target_link_directories(${TARGET_NAME} PRIVATE
    /opt/local/lib
    ../../../external/mac/lib/
    )

target_link_libraries(${TARGET_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
#    Qt${QT_VERSION_MAJOR}::Charts
    kipl
    ReaderConfig
    QtAddons
    armadillo
    lapack
    blas
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
 #   armadillo
 #   lapack
 #   blas
    )
endif()



target_compile_definitions(${TARGET_NAME} PRIVATE QTIMAGING_LIBRARY)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
