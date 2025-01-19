# define minimum cmake version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# project name and supported language
project(ReconFramework-binder LANGUAGES CXX)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE
        STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE
  )
endif(NOT CMAKE_BUILD_TYPE)

# for testing we will need the python interpreter
find_package(Python3 COMPONENTS Interpreter)

if("${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}" VERSION_LESS "3.5")
  message(
    FATAL_ERROR
      "Python v3 interpreter must be greater than or equal to 3.5. Found ${PYTHON3_VERSION_MAJOR}.${PYTHON3_VERSION_MINOR}"
  )
endif()

# Get version from git-describe
execute_process(
  COMMAND git describe --tag
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  RESULT_VARIABLE HAVE_GIT_VERSION_INFO
  OUTPUT_VARIABLE GIT_VERSION_INFO
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(${HAVE_GIT_VERSION_INFO} EQUAL 0)
  message(STATUS "Got version from Git: ${GIT_VERSION_INFO}")
  add_definitions(-DVERSION="${GIT_VERSION_INFO}")
endif()

add_subdirectory(pyMuhRec)

message(${CMAKE_CURRENT_SOURCE_DIR})
