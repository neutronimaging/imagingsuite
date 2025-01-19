cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME "tStdPreprocModules")
project(${TARGET_NAME} LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(
    ../../Framework/ReconFramework/include/
    ../../Preprocessing/StdPreprocModules/include
     ../../../../core/modules/ModuleConfig/include
    ../../../../core/kipl/kipl/include/
    ../../../../core/algorithms/ImagingAlgorithms/include
)

set(SRC_FILES
    tst_StdPreprocModules.cpp)

add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
    PUBLIC
        Qt6::Test
        kipl
        ModuleConfig
        ImagingAlgorithms
        ReconFramework
        StdPreprocModules
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
