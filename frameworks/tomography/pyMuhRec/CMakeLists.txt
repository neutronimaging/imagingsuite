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
    add_definitions(-DHAVEPYBIND11 -DNO_QT)
endif()

set(TARGET_NAME "muhrectomo") 

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    ../../../core/kipl/kipl/include
    ../../../external/src/linalg
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../external/mac/include/
    ../../../external/mac/include/nexus
    ../../../external/mac/include/hdf5
    ../Framework/ReconFramework/include
    ../Backprojectors/StdBackProjectors/include
    ../../../core/algorithms/ImagingAlgorithms/include
    )
link_directories(../../../../lib /opt/local/lib )

# add_definitions(-DVERSION="1.6.0")
# create python module

set(INC_FILES
  src/reconstructor.h
)

set(SRC_FILES
  src/pyReconEngine.cpp
  src/pyReconException.cpp
  src/pyReconConfig.cpp
  src/pyReconFactory.cpp
  src/pyReconstructor.cpp
  src/reconstructor.cpp
  src/pybinder.cpp
)

set(LINK_TYPE "MODULE")

add_library(${TARGET_NAME}
  ${LINK_TYPE}
#  MODULE
  ${SRC_FILES}  
  ${INC_FILES}
  )

target_link_libraries(${TARGET_NAME}
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    xml2
    tiff
    ModuleConfig
    ReconFramework
    StdBackProjectors
  )

set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

if(DYNAMIC_LIB)
  install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
  install(FILES 
      ../../../../lib/libkipl.1.0.0.dylib 
      ../../../../lib/libModuleConfig.1.0.0.dylib 
      ../../../../lib/libReconFramework.1.0.0.dylib
      ../../../../lib/libStdBackProjectors.1.0.0.dylib
      ../../../../lib/libImagingAlgorithms.1.0.0.dylib
      ../../../../lib/libReaderConfig.1.0.0.dylib
      ../../../../lib/libNeXus.1.0.0.dylib	
      ../../../../lib/libhdf5_cpp.11.dylib
      ../../../../lib/libNeXusCPP.1.0.0.dylib
	    ../../../../lib/libhdf5_hl.10.dylib
      ../../../../lib/libhdf5.10.dylib	
      ../../../../lib/libsz.2.dylib

      /opt/local/lib/libxml2.2.dylib 
      /opt/local/lib/libtiff.5.dylib

      DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

      execute_process(
        COMMAND for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done 
        WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
      )
endif(DYNAMIC_LIB)