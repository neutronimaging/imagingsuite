cmake_minimum_required(VERSION 3.30)

set(TARGET_NAME "tAdvancedFilters")
project(tLogging LANGUAGES CXX)

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
    ../../AdvancedFilters
)

set(SRC_FILES,
    tst_tadvancedfilterstest.cpp)

add_executable(${TARGET_NAME} tst_tadvancedfilterstest.cpp)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Test
        kipl
        AdvancedFilters
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
