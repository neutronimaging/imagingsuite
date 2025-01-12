# define minimum cmake version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# project name and supported language
project(ImagingModules-binder LANGUAGES CXX)

# require C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for testing we will need the python interpreter
find_package(Python3 COMPONENTS Interpreter)

add_subdirectory(pyimgio)

# turn on testing
#enable_testing()

message(${CMAKE_CURRENT_SOURCE_DIR})

