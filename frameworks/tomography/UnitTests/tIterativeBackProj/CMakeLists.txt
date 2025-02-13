cmake_minimum_required(VERSION 3.30)

set(TARGET_NAME "tIterativeBackProj")
project(${TARGET_NAME} LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../Framework/ReconFramework/include/
    ../../Backprojectors/Itera
     ../../../../core/modules/ModuleConfig/include
    ../../../../core/kipl/kipl/include/
    ../../../../core/algorithms/ImagingAlgorithms/include
)

set(SRC_FILES
    tst_IterativeBackProj.cpp)

add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
    PUBLIC
        Qt6::Test
        kipl
        ModuleConfig
        ImagingAlgorithms
        ReconFramework
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
