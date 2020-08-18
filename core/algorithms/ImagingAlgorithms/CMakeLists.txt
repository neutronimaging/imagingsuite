include(FetchContent)

set(TARGET_NAME "imagalg")

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

include_directories(../../kipl/kipl/include /opt/local/include ../../../external/src/linalg)
link_directories(../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module
add_library(imagalg
  MODULE

  src/StripeFilter.cpp  
#  src/SpotClean.cpp  
  src/PolynomialCorrection.cpp  
  src/ImagingException.cpp  
# src/TranslateProjections.cpp  
  src/MorphSpotClean.cpp  
  src/pixelinfo.cpp
  src/projectionfilter.cpp
#  src/gammaclean.cpp  
  src/averageimage.cpp  
#  src/piercingpointestimator.cpp  
#  src/ReferenceImageCorrection.cpp  
#  src/tomocenter.cpp  
  src/pybinder.cpp
  include/StripeFilter.h  
#  include/SpotClean.h  
  include/PolynomialCorrection.h  
  include/ImagingException.h  
#  include/TranslateProjections.h  
  include/MorphSpotClean.h  
  include/pixelinfo.h  
  include/projectionfilter.h
  include/ImagingAlgorithms_global.h  
 # include/gammaclean.h  
  include/averageimage.h  
 # include/piercingpointestimator.h  
 # include/ReferenceImageCorrection.h  
 # include/tomocenter.h
  include/pybinder.h
  )

target_link_libraries(imagalg
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    xml2
    tiff
  )

set_target_properties(imagalg
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

#install(DIRECTORY "src/scipp" DESTINATION ".")
install(TARGETS pyimagalg DESTINATION ".")