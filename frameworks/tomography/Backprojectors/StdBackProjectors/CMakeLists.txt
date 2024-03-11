cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "StdBackProjectors")
project(StdBackProjectors VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../core/modules/ModuleConfig/include
    # ../../../../../ExternalDependencies/macos/include
    ../../Framework/ReconFramework/include
    )
endif()


if (WIN32)
include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../Framework/ReconFramework/include
    # ../../../../../ExternalDependencies/windows/include
    # ../../../../../ExternalDependencies/windows/include/nexus
    # ../../../../../ExternalDependencies/windows/include/cfitsio
    # ../../../../../ExternalDependencies/windows/include/libxml2
    # ../../../../external/include/
    )
endif()

if (UNIX)
include_directories(
    /usr/include/libxml2
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../core/modules/ModuleConfig/include
    ../../Framework/ReconFramework/include
    )
endif()

set(INC_FILES
    include/StdBackProjectors.h
    include/StdBackProjectorBase.h
    include/NNMultiProjBP.h
    include/MultiProjBPparallel.h
    include/MultiProjBP.h
    # src/stdafx.h
    include/StdBackProjectors_global.h
)

set(SRC_FILES
src/StdBackProjectors.cpp
    src/StdBackProjectorBase.cpp
    # src/stdafx.cpp
    src/NNMultiProjBP.cpp
    src/MultiProjBPparallel.cpp
    src/MultiProjBP.cpp
    src/dllmain.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

# target_compile_definitions(${TARGET_NAME} PRIVATE STDBACKPROJECTORS_LIBRARY HAVE_NEXUS)
target_compile_definitions(${TARGET_NAME} PRIVATE STDBACKPROJECTORS_LIBRARY)

#add_definitions(-DHAVE_NEXUS)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})

if (WIN32)
    # target_link_directories(${TARGET_NAME} PRIVATE
    # ../../../../../ExternalDependencies/windows/lib
    # ../../../../../imagingsuite/external/lib64
    # ../../../../../imagingsuite/external/lib64/nexus
    # )
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        libxml2
        cfitsio
        TIFF::TIFF
        #libfftw3-3
        #libfftw3f-3
        fftw::fftw
        Iphlpapi

        )
endif()

if (APPLE)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
    )
endif()

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX
            # pybind11::module

            kipl
            ImagingAlgorithms
            ModuleConfig
            ReconFramework
            xml2
            #fftw3
            #fftw3f
            fftw::fftw
            cfitsio
            TIFF::TIFF
            armadillo
            lapack
            blas
            # NeXus
            # NeXusCPP
            )
    else()
        target_link_libraries(${TARGET_NAME}
          PUBLIC
            # pybind11::module

            kipl
            ImagingAlgorithms
            ModuleConfig
            ReconFramework
            xml2
            #fftw3
            #fftw3f
            fftw::fftw
            cfitsio
            TIFF::TIFF
            armadillo
            lapack
            blas
            # NeXus
            # NeXusCPP
          )
    endif()
endif()


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
