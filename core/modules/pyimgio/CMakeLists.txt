cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "pyimgio")
project(kipl LANGUAGES CXX)

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
