# define minimum cmake version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# project name and supported language
project(kipl-core LANGUAGES CXX)

# require C++11

set(CMAKE_CXX_EXTENSIONS OFF)
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

# # turn on testing
# enable_testing()

# message(${CMAKE_CURRENT_SOURCE_DIR})
# ## define test
# add_test(
#   NAME
#     testBindAverageImage
#   COMMAND
#     ${CMAKE_COMMAND} -E env PYIMGALG_MODULE_PATH=$<TARGET_FILE_DIR:pyimgalg>
# #    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindAverageImage.py
#     ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindAverageImage.py
# )

# add_test(
#   NAME
#     testBindPolynomialCorrection
#   COMMAND
#     ${CMAKE_COMMAND} -E env PYIMGALG_MODULE_PATH=$<TARGET_FILE_DIR:pyimgalg>
#     ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindPolynomialCorrection.py
# )
