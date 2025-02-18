

set(TARGET_NAME "tReaderConfig")
project(tLogging LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../kipl/kipl/include
    ../../../algorithms/ImagingAlgorithms/include
    ../../ReaderConfig
)

set(SRC_FILES,
    tst_treaderconfigtest.cpp)

add_executable(${TARGET_NAME} tst_treaderconfigtest.cpp)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Test
        kipl
        ReaderConfig
        ImagingAlgorithms
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
