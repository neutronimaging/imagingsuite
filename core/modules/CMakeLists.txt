# define minimum cmake version
(VERSION 3.30 FATAL_ERROR)

# project name and supported language
project(ImagingModules-binder LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for testing we will need the python interpreter
find_package(Python3 COMPONENTS Interpreter)

add_subdirectory(pyimgio)

message(${CMAKE_CURRENT_SOURCE_DIR})

