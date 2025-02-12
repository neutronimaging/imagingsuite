cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "QtAddons")
project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts PrintSupport)

message("Using Qt${QT_VERSION_MAJOR}")

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    Qt6Widgets_INCLUDE_DIRS/QtCharts
    )

set(SRC_FILES
  plotter.cpp
  plotpainter.cpp
  imagepainter.cpp
  imageviewerwidget.cpp
  qglyphs.cpp
  reportgeneratorbase.cpp
  imageviewerinfodialog.cpp
  qmarker.cpp
  qtlogviewer.cpp
  roiitemdlg.cpp
  roiwidget.cpp
  roimanager.cpp
  uxroiwidget.cpp
  loggingdialog.cpp
  comparevolumeviewer.cpp
  plotwidget.cpp
  setgraylevelsdlg.cpp
  callout.cpp
  facestyles.cpp
  plotcursor.cpp
  roidialog.cpp 
  imgviewerprofiledialog.cpp
) 

set (INC_FILES
  QtAddons_global.h
  plotter.h
  plotpainter.h
  imagepainter.h
  imageviewerwidget.h
  qglyphs.h
  reportgeneratorbase.h
  imageviewerinfodialog.h
  qmarker.h
  qtlogviewer.h
  roiitemdlg.h
  roiwidget.h
  roimanager.h
  uxroiwidget.h
  loggingdialog.h
  comparevolumeviewer.h
  plotwidget.h
  setgraylevelsdlg.h
  callout.h
  facestyles.h
  plotcursor.h
  roidialog.h
  imgviewerprofiledialog.h
)

set (UI_FILES
    imageviewerinfodialog.ui
    qtlogviewer.ui
    roiitemdlg.ui
    roiwidget.ui
    roimanager.ui
    uxroiwidget.ui
    loggingdialog.ui
    comparevolumeviewer.ui
    plotwidget.ui
    setgraylevelsdlg.ui
    roidialog.ui
    imgviewerprofiledialog.ui
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  ${UI_FILES}
)

target_link_libraries(${TARGET_NAME} 
  PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::PrintSupport
    kipl
    )

target_compile_definitions(${TARGET_NAME} PRIVATE QTADDONS_LIBRARY)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
