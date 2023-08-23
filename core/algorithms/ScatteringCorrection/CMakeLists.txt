cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ScatteringCorrection")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/algorithms/ImagingAlgorithms/include
)

if (APPLE)
include_directories(
    ../../../../ExternalDependencies/macos/include
    )
endif()

if (WIN32)
    include_directories(
        ../../../external/include/
        ../../../../ExternalDependencies/windows/include
        ../../../../ExternalDependencies/windows/include/nexus
        ../../../../ExternalDependencies/windows/include/cfitsio
        )
endif()

if (UNIX)
include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    ../../../external/src/linalg
    ../../../external/mac/include/
    ../../../../ExternalDependencies/macos/include
    ../../../../ExternalDependencies/macos/include/nexus
    ../../../external/mac/include/hdf5
    )
endif()

set(INC_FILES
  src/ImageNormalization.h
  src/ScatterEstimation.h
  src/ReferenceLoader.h	
  src/SegmentBB.h
  src/ScatteringCorrection.h
  src/ScatteringCorrection_global.h
)

set(SRC_FILES
  src/ImageNormalization.cpp
  src/ScatterEstimation.cpp
  src/ReferenceLoader.cpp
  src/SegmentBB.cpp
  src/ScatteringCorrection.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE SCATTERINGCORRECTION_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

    
if (APPLE)
target_link_libraries(${TARGET_NAME}
    PUBLIC
    kipl
    ImagingAlgorithms
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
        ImagingAlgorithms
    )
endif()

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX

          PUBLIC
            kipl
            ImagingAlgorithms
        )
    else()
    target_link_libraries(${TARGET_NAME} 
          PUBLIC
            kipl
            ImagingAlgorithms

        )
    endif()
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
