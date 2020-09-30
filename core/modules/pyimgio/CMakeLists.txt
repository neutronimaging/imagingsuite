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
    add_definitions(-DHAVEPYBIND11)
endif()

add_definitions(-DNO_QT)

include_directories(../../kipl/kipl/include ../../algorithms/ImagingAlgorithms/include ../ReaderConfig /opt/local/include /opt/local/include/libxml2 ../../../external/pylinalg)
link_directories(../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module
add_library(imgio
  MODULE

  pyBinder.cpp
  pyMiscIOFunctions.cpp
  pyFileSet.cpp
  pyImageReader.cpp
  pyImageWriter.cpp

  pyBinder.h
  )

target_link_libraries(imgio
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    ReaderConfig
    xml2
    tiff
    cfitsio
  )

set_target_properties(imgio
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
