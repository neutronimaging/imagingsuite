cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ReconAlgorithms")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    /usr/include/libxml2
    ../../../../../core/kipl/kipl/include
    ../../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../../external/src/linalg
    ../../../../../core/modules/ModuleConfig/include
    ../../../../../../ExternalDependencies/macos/include
    ../../../../../../ExternalDependencies/macos/include/nexus
    ../../../../../external/mac/include/
    ../../../../../external/mac/include/nexus
    ../../../../../external/mac/include/hdf5
    )
endif()


if (WIN32)
include_directories(
    ../../../../../core/kipl/kipl/include
    # ../../../external/src/linalg
    ../../../../../core/kipl/kipl/include
    ../../../../../core/modules/ModuleConfig/include
    ../../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../../../ExternalDependencies/windows/include
    ../../../../../../ExternalDependencies/windows/include/nexus
    ../../../../../../ExternalDependencies/windows/include/cfitsio
    ../../../../../../ExternalDependencies/windows/include/libxml2
    ../../../../../external/include/
    )
endif()

if (UNIX)
include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    /usr/include/libxml2
    ../../../../../core/kipl/kipl/include
    ../../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../../external/src/linalg
    ../../../../../core/modules/ModuleConfig/include
    )
endif()

set(INC_FILES
    backprojectorbase.h
    nnforwardprojector.h
    basicbackprojector.h
    reconalgorithmexception.h
    basicforwardprojector.h
    reconalgorithms.h
    forwardprojectorbase.h
    reconalgorithms_global.h
    linearforwardprojector.h
)

set(SRC_FILES
    backprojectorbase.cpp
    linearforwardprojector.cpp
    basicbackprojector.cpp
    nnforwardprojector.cpp
    basicforwardprojector.cpp
    reconalgorithmexception.cpp
    forwardprojectorbase.cpp
    reconalgorithms.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE RECONALGORITHMS_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    ../../../../../../ExternalDependencies/windows/lib
    ../../../../../../imagingsuite/external/lib64
    ../../../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PRIVATE
        kipl
        # ImagingAlgorithms
        ReconFramework
        libxml2
        cfitsio
        libtiff
        liblapack
        libopenblas
        libfftw3-3
        libfftw3f-3
        NeXus
        NeXusCPP
        Iphlpapi

        )
endif()

if (APPLE)
    if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/local/lib
            ../../../../../../ExternalDependencies/macos/arm64/lib
            )
    else()
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/local/lib
            ../../../../../external/mac/lib/
            )
    endif()
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
        ImagingAlgorithms
        ModuleConfig
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

if (LINUX)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
        ImagingAlgorithms
        ModuleConfig
        xml2
        fftw3
        fftw3f
        cfitsio
        tiff
        armadillo
        lapack
        blas
        NeXus
        NeXusCPP
    )
endif()
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
