cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "StdBackProjectors")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    /usr/include/libxml2
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../external/src/linalg
    ../../../../core/modules/ModuleConfig/include
    ../../../../external/mac/include/
    ../../../../external/mac/include/nexus
    ../../../../external/mac/include/hdf5
    ../../Framework/ReconFramework/include
    )

# link_directories(../../../../lib /opt/local/lib ../../../../external/mac/lib)
link_directories(/opt/local/lib ../../../../external/mac/lib)

set(INC_FILES
    include/StdBackProjectors.h
    include/StdBackProjectorBase.h
    include/NNMultiProjBP.h
    include/MultiProjBPparallel.h
    include/MultiProjBP.h
    src/stdafx.h
    include/StdBackProjectors_global.h
)

set(SRC_FILES
src/StdBackProjectors.cpp
    src/StdBackProjectorBase.cpp
    src/stdafx.cpp
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
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE STDBACKPROJECTORS_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

target_link_directories(${TARGET_NAME} PRIVATE
    /opt/local/lib
    ../../../../external/mac/lib
    )

if (APPLE)
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

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX
            pybind11::module

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
            NeXus
            NeXusCPP
            )
    else()
        target_link_libraries(${TARGET_NAME}
          PUBLIC
            pybind11::module

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
            NeXus
            NeXusCPP
          )
    endif()
endif(LINUX)


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
