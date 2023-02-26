cmake_minimum_required(VERSION 3.5)
set(TARGET_NAME "verticalslicer")
project(${TARGET_NAME} VERSION 4.3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
#find_package(Qt6 REQUIRED COMPONENTS Core )

#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

if (APPLE)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../../ExternalDependencies/macos/include
    )
endif()

if (WIN32)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../frameworks/tomography/Framework/ReconFramework/include/
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../../ExternalDependencies/windows/include
    ../../../../ExternalDependencies/windows/include/libxml2
    ../../../../ExternalDependencies/windows/include/nexus
    ../../../external/include/
    ../../../external/mac/include/hdf5
    )
endif()

set(SRC_FILES
    main.cpp
)

set(INC_FILES

)

message(CLIApp Qt${QT_VERSION_MAJOR})

add_executable(${TARGET_NAME}
        ${SRC_FILES}
        ${INC_FILES}
    )

target_link_libraries(${TARGET_NAME} PRIVATE
    kipl
    ReaderConfig
)


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${TARGET_NAME})
