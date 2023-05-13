cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ImagingAlgorithms")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../../ExternalDependencies/macos/include
    )
endif()

if (WIN32)
    include_directories(
        ../../../core/kipl/kipl/include
        ../../../core/modules/ModuleConfig/include
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
    ../../../core/kipl/kipl/include
    ../../../external/src/linalg
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../external/mac/include/
    ../../../../ExternalDependencies/macos/include
    ../../../../ExternalDependencies/macos/include/nexus
    ../../../external/mac/include/hdf5
    )
endif()

set(INC_FILES
  include/StripeFilter.h  
  include/SpotClean.h  
  include/PolynomialCorrection.h  
  include/ImagingException.h  
  include/TranslateProjections.h  
  include/MorphSpotClean.h  
  include/pixelinfo.h  
  include/projectionfilter.h
  include/ImagingAlgorithms_global.h  
  include/gammaclean.h  
  include/averageimage.h  
  include/piercingpointestimator.h  
  include/ReferenceImageCorrection.h  
  include/ReplaceUnderexposed.h
  include/tomocenter.h
  include/normalizeimage.h
  include/detectorlag.h
)

set(SRC_FILES
  src/StripeFilter.cpp  
  src/SpotClean.cpp  
  src/PolynomialCorrection.cpp  
  src/ImagingException.cpp  
  src/TranslateProjections.cpp  
  src/MorphSpotClean.cpp  
  src/pixelinfo.cpp
  src/projectionfilter.cpp
  src/gammaclean.cpp  
  src/averageimage.cpp  
  src/piercingpointestimator.cpp  
  src/ReferenceImageCorrection.cpp  
  src/ReplaceUnderexposed.cpp
  src/tomocenter.cpp  
  src/normalizeimage.cpp
  src/detectorlag.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE IMAGINGALGORITHMS_LIBRARY HAVE_NEXUS)

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
        # libxml2
        # cfitsio
        # libtiff
        # liblapack
        # libblas
        # libfftw3-3
        # libfftw3f-3
        # NeXus
        # NeXusCPP
        # Iphlpapi
    )
endif()

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX

          PUBLIC
            kipl
            # xml2
            # fftw3
            # fftw3f
            # cfitsio
            # tiff
            # armadillo
            # lapack
            # blas
            # NeXus
            # NeXusCPP
        )
    else()
    target_link_libraries(${TARGET_NAME} 
          PUBLIC
            kipl
            # xml2
            # fftw3
            # fftw3f
            # cfitsio
            # tiff
            # armadillo
            # lapack
            # blas
            # NeXus
            # NeXusCPP
        )
    endif()
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
