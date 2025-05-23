

set(TARGET_NAME "pyimgio")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../../kipl/kipl/include 
                    ../../algorithms/ImagingAlgorithms/include 
                    ../ReaderConfig
)

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
  )

set_target_properties(imgio
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
