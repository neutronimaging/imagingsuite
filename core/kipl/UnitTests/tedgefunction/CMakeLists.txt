cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME "tedgefunction")
project(tedgefunction LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include_directories(
    ../../kipl/include
    )

set(SRC_FILES,
    tst_tedgefunction.cpp)

add_executable(${TARGET_NAME} tst_tedgefunction.cpp)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Test
        kipl
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
