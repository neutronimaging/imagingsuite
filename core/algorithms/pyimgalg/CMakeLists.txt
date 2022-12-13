# ~~~
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2021 ImaginSuite contributors (https://github.com/neutronimaging/imagingsuite)
# ~~~

set(TARGET_NAME "imgalg") 

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ../../kipl/kipl/include 
    ../ImagingAlgorithms/include 
    /opt/local/include .
    ./../../external/pylinalg
    ../../../../ExternalDependencies/macos/include
    )
    
    if (WIN32)
    include_directories(
        ../../../core/kipl/kipl/include
        ../../../core/kipl/kipl/include
        ../../../core/modules/ModuleConfig/include
        ../../../external/include/
        ../../../../ExternalDependencies/windows/include
        ../../../../ExternalDependencies/windows/include/nexus
        ../../../../ExternalDependencies/windows/include/cfitsio
        )
endif()

link_directories(../../../../lib /opt/local/lib ../../../external/mac/lib)

# create python module
add_library(${TARGET_NAME}
  ${LINK_TYPE}
  MODULE

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
  pyBinder.h
  )

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

if (APPLE)
target_link_libraries(${TARGET_NAME}
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    ImagingAlgorithms
    xml2
    tiff
    armadillo
    lapack
    blas
    fftw3
    fftw3f
    cfitsio
    NeXus.1.0.0
    NeXusCPP.1.0.0
  )
endif(APPLE)

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    ../../../../ExternalDependencies/windows/lib
    ../../../../imagingsuite/external/lib64
    ../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
      pybind11::module

    PRIVATE
        kipl
        ImagingAlgorithms
        libxml2
        cfitsio
        libtiff
        # armadillo
        liblapack
        libopenblas
        libfftw3-3
        libfftw3f-3
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
            pybind11::module

          PRIVATE
              kipl
              ImagingAlgorithms
              xml2
              tiff
              armadillo
              lapack
              blas
              fftw3
              fftw3f
              cfitsio
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
              xml2
              tiff
              armadillo
              lapack
              blas
              fftw3
              fftw3f
              cfitsio
              NeXus
              NeXusCPP
          )
    endif()
endif()

set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

#if(DYNAMIC_LIB)
  install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
  # install(FILES 
  #     ../../../../lib/libkipl.1.0.0.dylib 
  #     ../../../../lib/libModuleConfig.1.0.0.dylib 
  #     ../../../../lib/libReconFramework.1.0.0.dylib
  #     ../../../../lib/libStdBackProjectors.1.0.0.dylib
  #     ../../../../lib/libFDKBackProjectors.1.0.0.dylib
  #     ../../../../lib/libImagingAlgorithms.1.0.0.dylib
  #     ../../../../lib/libReaderConfig.1.0.0.dylib
  #     ../../../../lib/libNeXus.1.0.0.dylib	
  #     ../../../../lib/libhdf5_cpp.11.dylib
  #     ../../../../lib/libNeXusCPP.1.0.0.dylib
	#     ../../../../lib/libhdf5_hl.10.dylib
  #     ../../../../lib/libhdf5.10.dylib	
  #     ../../../../lib/libsz.2.dylib

  #     /opt/local/lib/libxml2.2.dylib 
  #     /opt/local/lib/libtiff.5.dylib

  #     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

  #     execute_process(
  #       COMMAND for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done 
  #       WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
  #     )
#endif(DYNAMIC_LIB)
