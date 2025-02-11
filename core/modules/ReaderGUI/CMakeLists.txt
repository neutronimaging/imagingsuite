cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ReaderGUI")
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

find_package(QT NAMES Qt6 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts )

message("Using Qt${QT_VERSION_MAJOR}")

include_directories(
    ../QtAddons
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../GUI/qt/QtAddons
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )


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

target_link_libraries(${TARGET_NAME} 
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
#    Qt${QT_VERSION_MAJOR}::Charts

    kipl
    ReaderConfig
    QtAddons
    # ${NEXUS_LIBRARIES}
    )

target_compile_definitions(${TARGET_NAME} PRIVATE READERGUI_LIBRARY)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
