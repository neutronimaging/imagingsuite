cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME "tKiplMath")
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
    ../../kipl/include
    )

set(SRC_FILES
    tst_tkiplmathtest.cpp
    )

add_executable(${TARGET_NAME} ${SRC_FILES})

if (WIN32)
    # include_directories(
    #     ../../../../../ExternalDependencies/windows/include
    #     )
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qt6::Test
            kipl
        )
else()
    # include_directories(
    #     ../../../../../ExternalDependencies/macos/include
    #     )
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qt6::Test
            kipl
            # armadillo::armadillo
        )
endif()


add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
