cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "InspectorModules")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../../core/kipl/kipl/include
    /opt/local/include
    /opt/local/include/libxml2
    ../../Framework/ReconFramework/include/
    ../../../../core/algorithms/ImagingAlgorithms/include
#    ../../../../external/src/linalg
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../external/mac/include/
    ../../../../external/mac/include/nexus
    ../../../../external/mac/include/hdf5
    )

link_directories(../../../../../lib /opt/local/lib )

set(INC_FILES
  CountNANs.h
  SaveProjections.h
  InspectorModules.h
  getimagesize.h
  ProjectionInspector.h
  inspectormodules_global.h
)

set(SRC_FILES
  CountNANs.cpp
  ProjectionInspector.cpp
  getimagesize.cpp
  InspectorModules.cpp
  SaveProjections.cpp
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")
set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# target_compile_definitions(${TARGET_NAME} PRIVATE INSPECTORMODULES_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})
if (APPLE)
    if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/local/lib
            ../../../../../ExternalDependencies/macos/arm64/lib
            )
    else()
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/local/lib
            ../../../../external/mac/lib/
            )
    endif()

    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        xml2
        fftw3
        fftw3f
        cfitsio
        tiff
        armadillo
        lapack
        blas
        NeXus.1.0.0
        NeXusCPP.1.0.0
    )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
