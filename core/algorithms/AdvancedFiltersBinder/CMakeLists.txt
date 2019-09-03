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
add_definitions(-DHAVEPYBIND11 )
add_definitions(-DNO_QT)

include_directories(../../kipl/kipl/include /opt/local/include ../../../external/src/linalg)
link_directories(../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module
add_library(advancedfilters
  MODULE

  advancedfilters.cpp
  advancedfilterexception.cpp
  filterenums.cpp
  pybinder.cpp
  )

target_link_libraries(advancedfilters
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    xml2
    tiff
    AdvancedFilters
  )

set_target_properties(advancedfilters
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
