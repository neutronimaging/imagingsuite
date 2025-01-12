# ~~~
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2021 ImaginSuite contributors (https://github.com/neutronimaging/imagingsuite)
# ~~~

set(TARGET_NAME "imgalg") 

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)

include_directories(
    ../../kipl/kipl/include 
    ../ImagingAlgorithms/include 
    # ../../../../ExternalDependencies/macos/include
    )
    
if (WIN32)
    include_directories(
        ../../../core/kipl/kipl/include
        ../../../core/kipl/kipl/include
        ../../../core/modules/ModuleConfig/include
        # ../../../external/include/
        # ../../../../ExternalDependencies/windows/include
        # ../../../../ExternalDependencies/windows/include/nexus
        # ../../../../ExternalDependencies/windows/include/cfitsio
        )
endif()

set(INC_FILES
  pyBinder.h
  utilities.h
)

set(SRC_FILES
  pyStripeFilter.cpp  
#  pySpotClean.cpp  
  pyNormalizeImage.cpp
  pyPolynomialCorrection.cpp  
  pyImagingException.cpp  
# pyTranslateProjections.cpp  
  pyMorphSpotClean.cpp  
  pyPixelInfo.cpp
  pyProjectionFilter.cpp
#  pygammaclean.cpp  
  pyAverageImage.cpp  
#  pypiercingpointestimator.cpp  
#  pyReferenceImageCorrection.cpp  
#  pytomocenter.cpp  
  pyCenterEstimation.cpp
  pyBinder.cpp
  utilities.cpp

)

pybind11_add_module(${TARGET_NAME}
  ${LINK_TYPE}
  MODULE
  ${SRC_FILES}  
  ${INC_FILES}
  )

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

set(LINK_TYPE "MODULE")

message("PyImgAlg ${CMAKE_HOST_SYSTEM_PROCESSOR}")
if (APPLE)

target_link_libraries(${TARGET_NAME}
  PUBLIC
    pybind11::module
    kipl
    ImagingAlgorithms
  )
endif(APPLE)

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    # ../../../../ExternalDependencies/windows/lib
    # ../../../../imagingsuite/external/lib64
    # ../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
      pybind11::module
      OpenMP::OpenMP_CXX

      kipl
      ImagingAlgorithms
    )
endif()


if (UNIX AND NOT APPLE)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX
            # pybind11::module

              kipl
              ImagingAlgorithms
              # xml2
              # tiff
              # armadillo
              # lapack
              # blas
              # fftw3
              # fftw3f
              # cfitsio
              # NeXus
              # NeXusCPP
            )
    else()
        target_link_libraries(${TARGET_NAME}
          PUBLIC
            # pybind11::module

          
              kipl
              ImagingAlgorithms
              # xml2
              # tiff
              # armadillo
              # lapack
              # blas
              # fftw3
              # fftw3f
              # cfitsio
              # NeXus
              # NeXusCPP
          )
    endif()
endif()

set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
