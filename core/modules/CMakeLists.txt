# define minimum cmake version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# project name and supported language
project(ImagingModules-binder LANGUAGES CXX)

# for testing we will need the python interpreter
find_package(Python3 COMPONENTS Interpreter)

add_subdirectory(pyimgio)

message(${CMAKE_CURRENT_SOURCE_DIR})

