cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME "tKIPL_IO")
project(tKIPL_IO LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 COMPONENTS Test REQUIRED)
find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    ../../kipl/include
    ../../../../external/include
#    ../../../../ExternalDependencies/macos/include
#    ../../../../ExternalDependencies/macos/include/nexus
    )

set(SRC_FILES,
    tst_tkipl_iotest.cpp)

add_executable(${TARGET_NAME} tst_tkipl_iotest.cpp)

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

if (WIN32) 
target_link_directories(${TARGET_NAME} PRIVATE
../../../../../ExternalDependencies/windows/lib
../../../../../imagingsuite/external/lib64
# ../../../../imagingsuite/external/lib64/nexus
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Test
        kipl
        libtiff
    )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/tests
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/applications/${TARGET_NAME}.app/Contents/Frameworks)
