# define minimum cmake version
cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

# execute_process(
#   COMMAND git describe --tags --abbrev=0 | cut -d'v' -f 2
#   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
#   OUTPUT_VARIABLE IMGSUITE_VERSION
#   OUTPUT_STRIP_TRAILING_WHITESPACE
# )

project(imagingsuite VERSION 4.3 LANGUAGES CXX)

set(CMAKE_FIND_DEBUG_MODE OFF)
# require C++17
if(NOT DEFINED CMAKE_CXX_STANDARD)
  
endif(NOT DEFINED CMAKE_CXX_STANDARD)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE
        STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE
  )
endif(NOT CMAKE_BUILD_TYPE)

file(TO_CMAKE_PATH "$ENV{QTPATH}/lib/cmake/;${CMAKE_BINARY_DIR}/generators" PREFIX_PATH)
file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}/cmake/;${CMAKE_BINARY_DIR}/generators" MODULE_PATH)

# Set environment variables so that CMAKE can find the packages
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PREFIX_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_PATH})
message("CMAKE_PREFIX_PATH is: ${CMAKE_PREFIX_PATH}")
message("CMAKE_MODULE_PATH is: ${CMAKE_MODULE_PATH}")
message("QTPATH is: $ENV{QTPATH}")
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Python3 COMPONENTS Interpreter Development)

find_package(Armadillo REQUIRED)
find_package(pybind11 REQUIRED)
find_package(libxml2 REQUIRED)
find_package(OpenBLAS REQUIRED)
find_package(TIFF REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(cfitsio REQUIRED)
find_package(ZLIB REQUIRED)
find_package(HDF5 REQUIRED)

if (WIN32)
  find_package(dirent)
endif()

if (WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/Release)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
elseif (LINUX)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
elseif (APPLE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  include_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif()

if("${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}" VERSION_LESS "3.5")
  message(
    FATAL_ERROR
      "Python v3 interpreter must be greater than or equal to 3.5. Found ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}"
  )
endif()

# Get version from git-describe
execute_process(
  COMMAND git describe --tag
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  RESULT_VARIABLE HAVE_GIT_VERSION_INFO
  OUTPUT_VARIABLE GIT_VERSION_INFO
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY
)
if(${HAVE_GIT_VERSION_INFO} EQUAL 0)
  message(STATUS "Got version from Git: ${GIT_VERSION_INFO}")
  add_definitions(-DVERSION="${GIT_VERSION_INFO}")
endif()

include(CTest)

execute_process(
  COMMAND git describe --tag
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  RESULT_VARIABLE HAVE_GIT_VERSION_INFO
  OUTPUT_VARIABLE GIT_VERSION_INFO OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(${HAVE_GIT_VERSION_INFO} EQUAL 0)
  message(STATUS "Got version from Git: ${GIT_VERSION_INFO}")
  add_definitions(-DIMGSUITE_VERSION="${GIT_VERSION_INFO}")
endif()

if(WIN32)
add_compile_options(
  /openmp 
  /O2
  )
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
  add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wunused
    -Wshadow
    -Winit-self
    -Wpointer-arith
    -Woverloaded-virtual
    -Wold-style-cast
    -Wcast-qual
    -Wcast-align
  )
  add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-operator-names>
  )
endif()

set(PYTHONDIR kipl)
set(INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})

set(EXPORT_NAME ${PROJECT_NAME}-targets)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

if(NOT APPLE)
  set(CMAKE_INSTALL_RPATH $ORIGIN/../lib)
else()
  set(CMAKE_INSTALL_RPATH @loader_path/../Frameworks @loader_path/../../../lib @loader_path/lib @loader_path)
endif()
# turn on testing
enable_testing()

# Core libraries
add_subdirectory(core/kipl/kipl)

# Core library tests
add_subdirectory(core/kipl/UnitTests/tDirAnalysis)
add_subdirectory(core/kipl/UnitTests/tLogging)
add_subdirectory(core/kipl/UnitTests/tSTLVectorOperations)
add_subdirectory(core/kipl/UnitTests/tkiplfilters)
add_subdirectory(core/kipl/UnitTests/tFillHole)
add_subdirectory(core/kipl/UnitTests/tImageOperators)
add_subdirectory(core/kipl/UnitTests/tIndex2Coord)
add_subdirectory(core/kipl/UnitTests/tKIPL_IO)
add_subdirectory(core/kipl/UnitTests/tNoiseImage)
add_subdirectory(core/kipl/UnitTests/tedgefunction)
add_subdirectory(core/kipl/UnitTests/tkiplbasetest)
add_subdirectory(core/kipl/UnitTests/tKIPLTimers)
add_subdirectory(core/kipl/UnitTests/tKIPLbaseTImage)
add_subdirectory(core/kipl/UnitTests/tThreadPool)
add_subdirectory(core/kipl/UnitTests/tKiplMath)
add_subdirectory(core/kipl/UnitTests/tKIPLMathFunctions)
add_subdirectory(core/kipl/UnitTests/tLinFit)
add_subdirectory(core/kipl/UnitTests/tKIPLHistogram)


add_subdirectory(core/algorithms/ImagingAlgorithms)
add_subdirectory(core/algorithms/UnitTests/tImagingAlgorithms/)

add_subdirectory(core/algorithms/AdvancedFilters)
# add_subdirectory(core/algorithms/UnitTests/tKiplAdvFilters) # development stuff
# add_subdirectory(core/algorithms/UnitTests/tAdvancedFilters) # development stuff

add_subdirectory(core/algorithms/ImagingQAAlgorithms)
add_subdirectory(core/algorithms/UnitTests/tImagingQAAlgorithms)


add_subdirectory(core/modules/ModuleConfig)

add_subdirectory(core/modules/ReaderConfig)
add_subdirectory(core/modules/UnitTests/tReaderConfig)

add_subdirectory(GUI/qt/QtAddons)
add_subdirectory(GUI/qt/QtImaging)
add_subdirectory(core/modules/ReaderGUI)
add_subdirectory(GUI/qt/QtModuleConfigure)
#add_subdirectory(GUI/qt/WidgetTests)

# Tomography
add_subdirectory(frameworks/tomography/Framework/ReconFramework)
add_subdirectory(frameworks/tomography/Framework/ReconAlgorithms/ReconAlgorithms)
add_subdirectory(frameworks/tomography/Backprojectors/StdBackProjectors)
add_subdirectory(frameworks/tomography/Backprojectors/FDKBackProjectors)
add_subdirectory(frameworks/tomography/Preprocessing/StdPreprocModules)
add_subdirectory(frameworks/tomography/Preprocessing/InspectorModules)

add_subdirectory(core/modules/UnitTests/tModuleConfig) # Delayed compile due to dependency of ReconFramework
add_subdirectory(frameworks/tomography/UnitTests/tReconConfig)
if (NOT WIN32)
  add_subdirectory(frameworks/tomography/UnitTests/tFrameWork)
endif()
add_subdirectory(frameworks/tomography/UnitTests/tBBLogNorm)
add_subdirectory(frameworks/tomography/UnitTests/tFDKBackProjectors)
add_subdirectory(frameworks/tomography/UnitTests/tStdPreprocModules)
# add_subdirectory(frameworks/tomography/UnitTests/tIterativeBackProj) # Not relevant
add_subdirectory(frameworks/tomography/UnitTests/tReconAlgorithms)
add_subdirectory(frameworks/tomography/UnitTests/tStdBackProjectors)

# Tomography GUI
add_subdirectory(frameworks/tomography/Preprocessing/InspectorModulesGUI)
add_subdirectory(frameworks/tomography/Preprocessing/StdPreprocModulesGUI)

# applications
add_subdirectory(applications/muhrec)
if (WIN32)
   add_subdirectory(applications/CLI/muhrecCLI)
#   add_subdirectory(applications/verticalslicer)
#   add_subdirectory(applications/CLI/multiframesplitter)
endif()
add_subdirectory(applications/CLI/verticalslicer)

add_subdirectory(applications/imageviewer)

# Binder modules
add_subdirectory(frameworks/tomography/pyMuhRec)
add_subdirectory(core/algorithms/pyimgalg)
add_subdirectory(core/algorithms/pyadvancedfilters)
