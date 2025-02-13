# define minimum cmake version
cmake_minimum_required(VERSION 3.30)

# project name and supported language
project(kipl-core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
# for testing we will need the python interpreter
find_package(Python3 COMPONENTS Interpreter)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(algorithms/ImagingAlgorithms)
add_subdirectory(algorithms/pyimgalg)
add_subdirectory(modules/pyimgio)
add_subdirectory(kipl/kipl)
add_subdirectory(algorithms/AdvancedFilters)
add_subdirectory(algorithms/pyadvancedfilters)

if(NOT APPLE)
  set(CMAKE_INSTALL_RPATH $ORIGIN)
else()
  set(CMAKE_INSTALL_RPATH @loader_path)
endif()

