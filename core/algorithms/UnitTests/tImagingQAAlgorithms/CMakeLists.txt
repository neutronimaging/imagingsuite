(VERSION 3.30)

set(TARGET_NAME "tImagingQAAlgorithms")
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
    ../../../kipl/kipl/include
    ../../ImagingQAAlgorithms

)

set(SRC_FILES
    tst_ImagingQAAlgorithms.cpp)

add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Test
        kipl
        ImagingQAAlgorithms
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
