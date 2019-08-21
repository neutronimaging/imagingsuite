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

include_directories(../../../../core/kipl/kipl/include
    /opt/local/include
    /opt/local/include/libxml2
    ../../../../external/src/linalg
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../external/mac/include/
    ../../../../external/mac/include/nexus
    ../../../../external/mac/include/hdf5
    )
link_directories(../../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module
add_library(reconframework
  MODULE

  src/BackProjectorModuleBase.cpp
  src/ProjectionReader.cpp
  src/ReconEngine.cpp
  src/ReconFramework.cpp
#  src/stdafx.cpp
  src/ModuleItem.cpp
  src/ReconException.cpp
  src/ReconHelpers.cpp
# src/stdafx.h
  src/PreprocModuleBase.cpp
  src/ReconConfig.cpp
  src/ReconFactory.cpp
  src/pybinder.cpp
#dllmain.cpp
  include/BackProjectorModuleBase.h
  include/ProjectionReader.h
  include/ReconException.h
  include/ReconFramework_global.h
  include/targetver.h
  include/ModuleItem.h
  include/ReconConfig.h
  include/ReconFactory.h
  include/ReconHelpers.h
  include/PreprocModuleBase.h
  include/ReconEngine.h
  include/ReconFramework.h
  #include/stdafx.h
  )

target_link_libraries(reconframework
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    xml2
    tiff
    ModuleConfig
  )

set_target_properties(reconframework
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
