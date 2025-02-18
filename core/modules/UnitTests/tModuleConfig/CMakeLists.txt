

set(TARGET_NAME "tModuleConfig")
project(tModuleConfig LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../kipl/kipl/include
    ../../ModuleConfig/include
    ../../../../frameworks/tomography/Framework/ReconFramework/include
    ../../../algorithms/ImagingAlgorithms/include
)

set(SRC_FILES,
    tst_configbasetest.cpp)

add_executable(${TARGET_NAME} tst_configbasetest.cpp  dummyconfig.cpp)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Test
        kipl
        ModuleConfig
        ReconFramework
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
