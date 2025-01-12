cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "pyimgio")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../../kipl/kipl/include 
                    ../../algorithms/ImagingAlgorithms/include 
                    ../ReaderConfig
)

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
  )

set_target_properties(imgio
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )
