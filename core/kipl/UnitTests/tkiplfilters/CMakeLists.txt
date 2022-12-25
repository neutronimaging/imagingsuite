cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME "tkiplfilters")
project(tLogging LANGUAGES CXX)

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
    ../../kipl/include
    ../../../../../ExternalDependencies/macos/include
    )

set(SRC_FILES,
    tst_kiplfilters.cpp)

add_executable(${TARGET_NAME} tst_kiplfilters.cpp)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Test
        kipl
    )

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

#     if (WIN32)
#     target_link_directories(${TARGET_NAME} PRIVATE
#     ../../../../../ExternalDependencies/windows/lib
#     ../../../../../imagingsuite/external/lib64
#     # ../../../../imagingsuite/external/lib64/nexus
#     )
    
#     target_link_libraries(${TARGET_NAME}
#     PRIVATE
#         Qt6::Test
#         kipl
#         # libxml2
#         # cfitsio
#         # libtiff
#         # armadillo
#         liblapack
#         libopenblas
#         # libfftw3-3
#         # libfftw3f-3
#         # NeXus
#         # NeXusCPP
#         # Iphlpapi
#     )
# endif()

# install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/tests
#         LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/applications/${TARGET_NAME}.app/Contents/Frameworks)
