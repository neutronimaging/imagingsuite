cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "Segmentation")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    )

set(INC_FILES
    include/kmeans.h
    include/kmeans.hpp
    )

set(SRC_FILES
    src/kmeans.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE ADVANCEDFILTERS_LIBRARY)

add_definitions(-DHAVE_NEXUS)

if (APPLE)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
        kipl
    )
endif()

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    ../../../../ExternalDependencies/windows/lib
    ../../../../imagingsuite/external/lib64
    ../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
        kipl
    )
endif()

if (LINUX)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
    )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
