cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME "tFDKBackProjectors")
project(${TARGET_NAME} LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../Framework/ReconFramework/include/
    ../../Backprojectors/FDKBackProjectors/src/
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/kipl/kipl/include/
    ../../../../core/algorithms/ImagingAlgorithms/include
)

set(SRC_FILES
    tst_FDKBackprojectors.cpp)

add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
    PUBLIC
        Qt6::Test
        kipl
        ModuleConfig
        ImagingAlgorithms
        ReconFramework
        FDKBackProjectors
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
