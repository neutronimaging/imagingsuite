# ~~~
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2021 ImaginSuite contributors (https://github.com/neutronimaging/imagingsuite)
# ~~~

add_definitions(-DNO_QT)
set(TARGET_NAME "muhrectomo") 

include_directories(
    /usr/include/libxml2
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
    ../Backprojectors/FDKBackProjectors/src
    ../../../core/algorithms/ImagingAlgorithms/include
    )

#link_directories(../../../../lib /opt/local/lib )

link_directories( /opt/local/lib )

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
  MODULE
  ${SRC_FILES}  
  ${INC_FILES}
  )

target_link_libraries(${TARGET_NAME}
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    ImagingAlgorithms
    xml2
    tiff
    ModuleConfig
    ReconFramework
    StdBackProjectors
    FDKBackProjectors
  )

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

if(DYNAMIC_LIB)
  install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
  # install(FILES 
  #     ../../../../lib/libkipl.1.0.0.dylib 
  #     ../../../../lib/libModuleConfig.1.0.0.dylib 
  #     ../../../../lib/libReconFramework.1.0.0.dylib
  #     ../../../../lib/libStdBackProjectors.1.0.0.dylib
  #     ../../../../lib/libFDKBackProjectors.1.0.0.dylib
  #     ../../../../lib/libImagingAlgorithms.1.0.0.dylib
  #     ../../../../lib/libReaderConfig.1.0.0.dylib
  #     ../../../../lib/libNeXus.1.0.0.dylib	
  #     ../../../../lib/libhdf5_cpp.11.dylib
  #     ../../../../lib/libNeXusCPP.1.0.0.dylib
	#     ../../../../lib/libhdf5_hl.10.dylib
  #     ../../../../lib/libhdf5.10.dylib	
  #     ../../../../lib/libsz.2.dylib

  #     /opt/local/lib/libxml2.2.dylib 
  #     /opt/local/lib/libtiff.5.dylib

  #     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

      # execute_process(
      #   COMMAND for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done 
      #   WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
      # )
endif(DYNAMIC_LIB)
