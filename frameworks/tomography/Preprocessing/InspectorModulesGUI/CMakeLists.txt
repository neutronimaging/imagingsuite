cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "InspectorModulesGUI")
project(InspectorModulesGUI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(QT NAMES Qt6 COMPONENTS Widgets Charts PrintSupport REQUIRED)
# find_package(Qt6 REQUIRED COMPONENTS Widgets Charts PrintSupport)

find_package(QT NAMES Qt6 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts )

message("Using Qt${QT_VERSION_MAJOR}")
if (APPLE)
include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../Framework/ReconFramework/include/
    ../../../../GUI/qt/QtAddons/
    ../../../../GUI/qt/QtModuleConfigure/
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    # ../../../../../ExternalDependencies/macos/include
    )
endif()

if (WIN32)
include_directories(
    ../../../../core/kipl/kipl/include
    ../../Framework/ReconFramework/include/
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../GUI/qt/QtAddons/
    ../../../../GUI/qt/QtModuleConfigure/
    ../../../../core/modules/ModuleConfig/include
    # ../../../../../ExternalDependencies/windows/include
    # ../../../../../ExternalDependencies/windows/include/libxml2
    )
endif()

if (UNIX)
include_directories(
    ../../Framework/ReconFramework/include/
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../GUI/qt/QtAddons/
    ../../../../GUI/qt/QtModuleConfigure/
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    )
endif()

set(INC_FILES
inspectormodulesgui.h
inspectormodulesgui_global.h
saveprojectionsdlg.h
)

set(SRC_FILES
  inspectormodulesgui.cpp
  saveprojectionsdlg.cpp
)

set(UI_FILES
  saveprojectionsdlg.ui
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  ${UI_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# target_compile_definitions(${TARGET_NAME} PRIVATE INSPECTORMODULESGUI_LIBRARY HAVE_NEXUS)
target_compile_definitions(${TARGET_NAME} PRIVATE INSPECTORMODULESGUI_LIBRARY )

# add_definitions(-DHAVE_NEXUS)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})

if (WIN32)

    # target_link_directories(${TARGET_NAME} PRIVATE
    #     ../../../../../ExternalDependencies/windows/lib
    #     ../../../../../imagingsuite/external/lib64
    #     ../../../../../imagingsuite/external/lib64/nexus
    #     )
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Charts
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        QtAddons
        QtModuleConfigure
        InspectorModules
        # libxml2
        # cfitsio
        # libtiff
        # NeXus
        # NeXusCPP
    )
endif()

if (APPLE)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Charts
    #    Qt${QT_VERSION_MAJOR}::PrintSupport
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        QtAddons
        QtModuleConfigure
        InspectorModules
    )
endif()

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(${TARGET_NAME} PUBLIC
      Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Widgets
      Qt${QT_VERSION_MAJOR}::Charts
  #    Qt${QT_VERSION_MAJOR}::PrintSupport
      kipl
      ImagingAlgorithms
      ModuleConfig
      ReconFramework
      QtAddons
      QtModuleConfigure
      InspectorModules
  )
endif()


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
