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

endif()

add_definitions(-DHAVEPYBIND11 -DNO_QT)

include_directories(../../kipl/kipl/include /opt/local/include ../../../external/src/linalg)
link_directories(../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module
add_library(imagingqa
  MODULE

  ballanalysis.cpp		
  imagingqaalgorithms.cpp
  ballassemblyanalysis.cpp	
  profileextractor.cpp
  collimationestimator.cpp	
  resolutionestimators.cpp
  contrastsampleanalysis.cpp
  ballanalysis.h			
  imagingqaalgorithms.h
  ballassemblyanalysis.h		
  imagingqaalgorithms_global.h
  collimationestimator.h		
  profileextractor.h
  contrastsampleanalysis.h	
  resolutionestimators.h
  pybinder.cpp
  pybinder.h
  )

target_link_libraries(imagingqa
  PUBLIC
    pybind11::module
    kipl
    xml2
    tiff
  )

set_target_properties(imagingqa
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
