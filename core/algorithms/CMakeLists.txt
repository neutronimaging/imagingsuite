# define minimum cmake version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# project name and supported language
project(nxs-binder LANGUAGES CXX)

# require C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for testing we will need the python interpreter
find_package(PythonInterp REQUIRED)

add_subdirectory(ImagingAlgorithms)

# turn on testing
enable_testing()

## define test
#add_test(
#  NAME
#    python_bindingtest
#  COMMAND
#    ${CMAKE_COMMAND} -E env NXS_MODULE_PATH=$<TARGET_FILE_DIR:imglib>
#  //  ${CMAKE_COMMAND} -E env NXS_MODULE_PATH=/Users/kaestner/git/build-nxs-Desktop_Qt_5_12_2_clang_64bit-Default
#    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/testPyNXSlib/bindingtest.py
#  )

