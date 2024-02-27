# define minimum cmake version
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# execute_process(
#   COMMAND git describe --tags --abbrev=0 | cut -d'v' -f 2
#   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
#   OUTPUT_VARIABLE IMGSUITE_VERSION
#   OUTPUT_STRIP_TRAILING_WHITESPACE
# )

project(imagingsuite VERSION 4.3 LANGUAGES CXX)

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

# Ensure that paths are set correctly according to OS
file(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}/generators" PREFIX_PATH)
file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}/cmake/;${CMAKE_BINARY_DIR}/generators" MODULE_PATH)
# Set environment variables so that CMAKE can find the packages
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PREFIX_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_PATH})
message("CMAKE_PREFIX_PATH is: ${CMAKE_PREFIX_PATH}")
message("CMAKE_MODULE_PATH is: ${CMAKE_MODULE_PATH}")
find_package(PythonInterp REQUIRED)

find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(TIFF REQUIRED)
find_package(pybind11 REQUIRED)

if (WIN32)
  find_package(Armadillo REQUIRED CONFIG)
  find_package(cfitsio REQUIRED)
endif()

if (UNIX)
  # for testing we will need the python interpreter
  find_package(BLAS REQUIRED)
  find_package(FFTW REQUIRED)
  # find_package(Nexus REQUIRED)
  find_package(CFITSIO REQUIRED)
  find_package(Armadillo REQUIRED)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/applications)
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

if(NOT APPLE)
  set(CMAKE_INSTALL_RPATH $ORIGIN)
else()
  set(CMAKE_INSTALL_RPATH @loader_path)
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
#add_subdirectory(core/kipl/UnitTests/tNoiseImage)
add_subdirectory(core/kipl/UnitTests/tedgefunction)
add_subdirectory(core/kipl/UnitTests/tkiplbasetest)
add_subdirectory(core/kipl/UnitTests/tKIPLTimers)
add_subdirectory(core/kipl/UnitTests/tKIPLbaseTImage)
add_subdirectory(core/kipl/UnitTests/tThreadPool)
add_subdirectory(core/kipl/UnitTests/tKiplMath)
add_subdirectory(core/kipl/UnitTests/tKIPLMathFunctions)
add_subdirectory(core/kipl/UnitTests/tLinFit)
#add_subdirectory(core/kipl/UnitTests/tKIPLHistogram)


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
# #add_subdirectory(GUI/qt/WidgetTests)

# # Tomography
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

# add_subdirectory(applications/imageviewer)

# # # Binder modules
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

    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

    execute_process(
      COMMAND for f in `ls *.so.1.0.0`; do ln -s $f `basename $f .so.1.0.0`.so.1; done 
      WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
    )
  endif()
  if (APPLE)
    install(FILES 
    # ../ExternalDependencies/macos/${CMAKE_HOST_SYSTEM_PROCESSOR}/lib/libNeXus.1.0.0.dylib
    # ../ExternalDependencies/macos/${CMAKE_HOST_SYSTEM_PROCESSOR}/lib/libNeXusCPP.1.0.0.dylib
    # /opt/homebrew/lib/libhdf5.dylib
    # /opt/homebrew/lib/libhdf5_cpp.dylib 
    
    # external/mac/lib/libNeXus.1.0.0.dylib	
    # external/mac/lib//libhdf5_cpp.11.dylib
    # external/mac/lib//libNeXusCPP.1.0.0.dylib
    # external/mac/lib//libhdf5_hl.10.dylib
    # external/mac/lib//libhdf5.10.dylib	
    # external/mac/lib//libsz.2.dylib

    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

    execute_process(
      COMMAND for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done 
      WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
    )
  endif()
  



endif(DYNAMIC_LIB)
# if (WIN32)
# message("${OpenBLAS_LIBRARIES}")
# install(FILES 
#   external/lib64/nexus/NeXus.dll	
#   external/lib64/nexus/NeXusCPP.dll
#   external/lib64/hdf5/hdf5_hl.dll
#   external/lib64/hdf5/hdf5.dll
#   external/lib64/hdf5/hdf5_cpp.dll
#   external/lib64/hdf5/zlib.dll
#   external/lib64/hdf5/szip.dll
#   external/lib64/libtiff.dll
#   external/lib64/libjpeg-62.dll
#   external/lib64/zlib1.dll
#   external/lib64/libfftw3-3.dll
#   external/lib64/libfftw3f-3.dll


#   # ${OpenBLAS_LIBRARIES}
#   # external/lib64/libblas.dll
#   # external/lib64/liblapack.dll
#   # external/lib64/liblapacke.dll
#   # external/lib64/libquadmath-0.dll
#   # external/lib64/libgfortran_64-3.dll
#   # external/lib64/libwinpthread-1.dll
#   # external/lib64/libgcc_s_seh-1.dll
#   # external/lib64/libgcc_s_seh_64-1.dll
#   # ../ExternalDependencies/windows/bin/libopenblas.dll

#   ../ExternalDependencies/windows/bin/libxml2.dll 
#   ../ExternalDependencies/windows/bin/cfitsio.dll
#   ../ExternalDependencies/windows/bin/libopenblas.dll

#   DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
# endif()
# message(${CMAKE_CURRENT_SOURCE_DIR})

#set_property(TEST adder PROPERTY ENVIRONMENT_MODIFICATION PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:main>)

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
