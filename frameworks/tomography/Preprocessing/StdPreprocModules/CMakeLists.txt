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
add_library(stdpreprocmodules
  MODULE

  src/AdaptiveFilter.cpp
  src/SinoSpotClean.cpp
  src/BasicRingClean.cpp
  src/SpotClean.cpp
  src/CameraStripeClean.cpp
  src/SpotClean2.cpp
  src/DataScaler.cpp
  src/SpotRingClean.cpp
  src/GammaSpotCleanModule.cpp
  src/StdPreprocModules.cpp
  src/GeneralFilter.cpp
  src/TranslateProjectionsModule.cpp
I src/SSfilter.cpp
  src/TranslatedProjectionWeighting.cpp
  src/MedianMixRingClean.cpp
  src/WaveletRingClean.cpp
  src/MorphSpotCleanModule.cpp
  src/bblognorm.cpp
  src/NormPlugins.cpp
  src/PolynomialCorrectionModule.cpp
  src/PreprocEnums.cpp
  src/ProjectionFilter.cpp


  )

target_link_libraries(stdpreprocmodules
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    xml2
    tiff
    ModuleConfig
    reconframework
  )

set_target_properties(stdpreprocmodules
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
