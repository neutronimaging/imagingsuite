include(FetchContent)

FetchContent_Declare(
  pybind11_sources
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.2
)

FetchContent_GetProperties(pybind11_sources)

if(NOT pybind11_sources_POPULATED)
  FetchContent_Populate(pybind11_sources)

  add_subdirectory(
    ${pybind11_sources_SOURCE_DIR}
    ${pybind11_sources_BINARY_DIR}
    )
    add_definitions(-DHAVEPYBIND11 -DNO_QT)
endif()

include_directories(../../kipl/kipl/include ../ImagingAlgorithms/include /opt/local/include ../../../external/pylinalg)
link_directories(../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module
add_library(pyimgalg
  MODULE

  pyStripeFilter.cpp  
#  pySpotClean.cpp  
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
  pyBinder.cpp
# pyStripeFilter.h  
# pySpotClean.h  
#  pyPolynomialCorrection.h  
#  pyImagingException.h  
 # pyTranslateProjections.h  
 # pyMorphSpotClean.h  
 # pyPixelInfo.h  
 # pyProjectionFilter.h
# pyImagingAlgorithms_global.h  
 # pygammaclean.h  
 # pyAverageImage.h  
 # pypiercingpointestimator.h  
 # pyReferenceImageCorrection.h  
 # pytomocenter.h
  pyBinder.h
  )

target_link_libraries(pyimgalg
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    ImagingAlgorithms
    xml2
    tiff
  )

set_target_properties(pyimgalg
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
