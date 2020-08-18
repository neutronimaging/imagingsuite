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

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    ../../../core/kipl/kipl/include
    ../../../external/src/linalg
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../external/mac/include/
    ../../../external/mac/include/nexus
    ../../../external/mac/include/hdf5
    ../Framework/ReconFramework/include
    )
link_directories(../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module
add_library(muhrectomo
  MODULE

  src/ReconEngine.cpp
  src/ReconException.cpp
  src/ReconConfig.cpp
  src/ReconFactory.cpp
  src/pybinder.cpp

  )

target_link_libraries(muhrectomo
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    xml2
    tiff
    ModuleConfig
    ReconFramework
  )

set_target_properties(muhrectomo
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
