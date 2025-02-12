cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "StdBackProjectors")
project(StdBackProjectors VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../core/modules/ModuleConfig/include
    ../../Framework/ReconFramework/include
    )

set(INC_FILES
    include/StdBackProjectors.h
    include/StdBackProjectorBase.h
    include/NNMultiProjBP.h
    include/MultiProjBPparallel.h
    include/MultiProjBP.h
    include/StdBackProjectors_global.h
)

set(SRC_FILES
src/StdBackProjectors.cpp
    src/StdBackProjectorBase.cpp
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

target_compile_definitions(${TARGET_NAME} PRIVATE STDBACKPROJECTORS_LIBRARY)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})

if (WIN32)
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        LibXml2::LibXml2
        cfitsio::cfitsio
        TIFF::TIFF
        OpenBLAS::OpenBLAS
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

            kipl
            ImagingAlgorithms
            ModuleConfig
            ReconFramework
            LibXml2::LibXml2 
            fftw::fftw
            cfitsio::cfitsio
            TIFF::TIFF
            armadillo::armadillo
            OpenBLAS::OpenBLAS
            )
    else()
        target_link_libraries(${TARGET_NAME}
          PUBLIC
            kipl
            ImagingAlgorithms
            ModuleConfig
            ReconFramework
            LibXml2::LibXml2 
            fftw::fftw
            cfitsio::cfitsio
            TIFF::TIFF
            armadillo::armadillo
            OpenBLAS::OpenBLAS
          )
    endif()
endif()


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
