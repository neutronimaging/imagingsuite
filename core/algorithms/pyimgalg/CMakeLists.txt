# ~~~
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2021 ImaginSuite contributors (https://github.com/neutronimaging/imagingsuite)
# ~~~

add_definitions(-DNO_QT)

set(TARGET_NAME "imgalg") 

include_directories(
    ../../kipl/kipl/include 
    ../ImagingAlgorithms/include 
    /opt/local/include .
    ./../../external/pylinalg
    )
    
link_directories(../../../../lib /opt/local/lib )

# create python module
add_library(${TARGET_NAME}
  ${LINK_TYPE}
  MODULE

  pyStripeFilter.cpp  
#  pySpotClean.cpp  
#  pyNormalizeImage.cpp
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

target_link_libraries(${TARGET_NAME}
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    ImagingAlgorithms
    xml2
    tiff
  )

set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

if(DYNAMIC_LIB)
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
endif(DYNAMIC_LIB)
