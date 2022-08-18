# define minimum cmake version
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

#if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
#message(FATAL_ERROR “In-source_build_detected!_Create_a_build_folder_and_call_CMake_there”)
#endif()

execute_process(
  COMMAND git describe --tags --abbrev=0 | cut -d'v' -f 2
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE IMGSUITE_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

project(imagingsuite VERSION 4.2.1 LANGUAGES CXX)

# require C++17
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
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

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )

set(CMAKE_MODULE_PATH "lib" )
# for testing we will need the python interpreter
find_package(PythonInterp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if("${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}" VERSION_LESS "3.5")
  message(
    FATAL_ERROR
      "Python v3 interpreter must be greater than or equal to 3.5. Found ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}"
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

include(CTest)
include(FetchContent)

FetchContent_Declare(
  pybind11_sources
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.2
)

FetchContent_GetProperties(pybind11_sources)

if(NOT pybind11_sources_POPULATED)
  FetchContent_Populate(pybind11_sources)

  add_subdirectory(
    ${pybind11_sources_SOURCE_DIR}
    ${pybind11_sources_BINARY_DIR}
    )
    #add_definitions(-DHAVEPYBIND11 -DNO_QT)
    add_definitions(-DHAVEPYBIND11)
endif()

#include(pybind11)

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
endif(NOT WIN32)

set(PYTHONDIR kipl)
set(INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})

set(EXPORT_NAME ${PROJECT_NAME}-targets)

if(NOT APPLE)
  set(CMAKE_INSTALL_RPATH $ORIGIN)
else()
  set(CMAKE_INSTALL_RPATH @loader_path)
 # SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH YES)
endif()

# Core libraries
add_subdirectory(core/kipl/kipl)
# add_subdirectory(core/kipl/UnitTests/tSTLVectorOperations/)

add_subdirectory(core/algorithms/ImagingAlgorithms)
# add_subdirectory(core/algorithms/UnitTests/tImagingAlgorithms/)

add_subdirectory(core/algorithms/AdvancedFilters)
add_subdirectory(core/modules/ModuleConfig)
add_subdirectory(core/modules/ReaderConfig)
add_subdirectory(GUI/qt/QtAddons)
add_subdirectory(GUI/qt/QtImaging)
add_subdirectory(core/modules/ReaderGUI)
add_subdirectory(GUI/qt/QtModuleConfigure)
# #add_subdirectory(GUI/qt/WidgetTests)

# # Tomography
add_subdirectory(frameworks/tomography/Framework/ReconFramework)
add_subdirectory(frameworks/tomography/Framework/ReconAlgorithms/ReconAlgorithms)
add_subdirectory(frameworks/tomography/Backprojectors/StdBackProjectors)
add_subdirectory(frameworks/tomography/Backprojectors/FDKBackProjectors)
add_subdirectory(frameworks/tomography/Preprocessing/StdPreprocModules)
add_subdirectory(frameworks/tomography/Preprocessing/InspectorModules)

# Tomography GUI
add_subdirectory(frameworks/tomography/Preprocessing/InspectorModulesGUI)
add_subdirectory(frameworks/tomography/Preprocessing/StdPreprocModulesGUI)

# applications
add_subdirectory(applications/muhrec)
if (WIN32)
   add_subdirectory(applications/CLI/muhrecCLI)
#   add_subdirectory(applications/verticalslicer)
#   add_subdirectory(applications/multiframesplitter)
endif()

add_subdirectory(applications/imageviewer)

# # Binder modules
# add_subdirectory(frameworks/tomography/pyMuhRec)
# add_subdirectory(core/algorithms/pyimgalg)
# add_subdirectory(core/algorithms/pyadvancedfilters)

if(DYNAMIC_LIB)
  # install(
  #   EXPORT ${EXPORT_NAME}
  #   DESTINATION "lib/cmake/kipl"
  #   NAMESPACE kipl::
  # )

  # include(CMakePackageConfigHelpers)
  # configure_package_config_file(
  #   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
  #   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  #   INSTALL_DESTINATION lib/cmake/kipl
  # )
  # write_basic_package_version_file(
  #   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  #   VERSION "${CMAKE_PROJECT_VERSION}"
  #   COMPATIBILITY SameMinorVersion # switch to SameMajorVersion after 1.0
  #                                  # release
  # )
  # install(
  #   FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  #         "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  #   DESTINATION lib/cmake/kipl
  # )
  if (LINUX)
    print('Linux')
    install(FILES 
      ../lib/libkipl.so.1.0.0 
      ../lib/libModuleConfig.so.1.0.0 
      ../lib/libReconFramework.so.1.0.0
      ../lib/libStdBackProjectors.so.1.0.0
      ../lib/libFDKBackProjectors.so.1.0.0
      ../lib/libImagingAlgorithms.so.1.0.0
      ../lib/libReaderConfig.so.1.0.0
      ../lib/libNeXus.so.1.0.0	
      ../lib/libhdf5_cpp.so.11
      ../lib/libNeXusCPP.so.1.0.0
      ../lib/libhdf5_hl.so.10
      ../lib/libhdf5.so.10	
      ../lib/libsz.so.2

  #    /opt/local/lib/libxml2.2.dylib 
  #    /opt/local/lib/libtiff.5.dylib
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

    execute_process(
      COMMAND for f in `ls *.so.1.0.0`; do ln -s $f `basename $f .so.1.0.0`.so.1; done 
      WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
    )
  endif()
  if (APPLE)
    install(FILES 
#    ../lib/libkipl.1.0.0.dylib
#    ../lib/libModuleConfig.1.0.0.dylib
#    ../lib/libReconFramework.1.0.0.dylib
#    ../lib/libStdBackProjectors.1.0.0.dylib
#    ../lib/libFDKBackProjectors.1.0.0.dylib
#    ../lib/libImagingAlgorithms.1.0.0.dylib
#    ../lib/libReaderConfig.1.0.0.dylib
    # external/mac/lib/libNeXus.1.0.0.dylib	
    # external/mac/lib//libhdf5_cpp.11.dylib
    # external/mac/lib//libNeXusCPP.1.0.0.dylib
    # external/mac/lib//libhdf5_hl.10.dylib
    # external/mac/lib//libhdf5.10.dylib	
    # external/mac/lib//libsz.2.dylib
    ../ExternalDependencies/macos/${CMAKE_HOST_SYSTEM_PROCESSOR}/lib/libNeXus.1.0.0.dylib
    ../ExternalDependencies/macos/${CMAKE_HOST_SYSTEM_PROCESSOR}/lib/libNeXusCPP.1.0.0.dylib
    /opt/local/lib/libxml2.2.dylib 
    /opt/local/lib/libtiff.5.dylib
    /opt/local/lib/libfftw3.3.dylib
    /opt/local/lib/libfftw3f.3.dylib
    /opt/local/lib/libcfitsio.dylib
    /opt/local/lib/libarmadillo.10.dylib

    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

    execute_process(
      COMMAND for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done 
      WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
    )
  endif()
  

  if (WIN32)
  install(FILES 
  external/windows/lib64/nexus/libNeXus.dll	
  external/windows/lib64/hdf5/libhdf5_cpp.dll
  external/windows/lib64/nexus/libNeXusCPP.dll
  external/windows/lib64/hdf5/libhdf5_hl.dll
  external/windows/lib64/hdf5/libhdf5.dll
  external/windows/lib64/libsz.2.dylib
  external/windows/lib64/libtiff.dll
  external/windows/lib64/libfftw3.3.dylib
  external/windows/lib64/libfftw3f.3.dylib
  ../ExternalDependencies/windows/lib/libxml2.dll 
  ../ExternalDependencies/windows/lib/libopenblas.dll
  ../ExternalDependencies/windows/lib/cfitsio.dll

  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()

endif(DYNAMIC_LIB)

# turn on testing
enable_testing()

message(${CMAKE_CURRENT_SOURCE_DIR})
## define test
#add_test(
#  NAME
#    testBindAverageImage
#  COMMAND
#    ${CMAKE_COMMAND} -E env IMAGALG_MODULE_PATH=$<TARGET_FILE_DIR:imagalg>
#    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindAverageImage.py
#    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindAverageImage.py
#)
#
#add_test(
#  NAME
#    testBindPolynomialCorrection
#  COMMAND
#    ${CMAKE_COMMAND} -E env IMAGALG_MODULE_PATH=$<TARGET_FILE_DIR:imagalg>
#    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests/python/testBindPolynomialCorrection.py
#)
