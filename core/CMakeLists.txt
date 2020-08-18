# define minimum cmake version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# project name and supported language
project(kipl-core LANGUAGES CXX)

# require C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for testing we will need the python interpreter
find_package(PythonInterp REQUIRED)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(algorithms/pyimgalg)
add_subdirectory(modules/pyimgio)

# turn on testing
enable_testing()

message(${CMAKE_CURRENT_SOURCE_DIR})
## define test
add_test(
  NAME
    testBindAverageImage
  COMMAND
    ${CMAKE_COMMAND} -E env PYIMGALG_MODULE_PATH=$<TARGET_FILE_DIR:pyimgalg>
#    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindAverageImage.py
    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindAverageImage.py
)

add_test(
  NAME
    testBindPolynomialCorrection
  COMMAND
    ${CMAKE_COMMAND} -E env PYIMGALG_MODULE_PATH=$<TARGET_FILE_DIR:pyimgalg>
    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindPolynomialCorrection.py
)
