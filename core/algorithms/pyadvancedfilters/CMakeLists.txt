set(TARGET_NAME "advancedfilters")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../kipl/kipl/include
    /opt/local/include
    ../../../external/src/linalg
    )

link_directories(
    ../../../../lib
    /opt/local/lib
    )

# create python module
add_library(${TARGET_NAME}
  ${LINK_TYPE}
  MODULE

  advancedfilters.cpp
  advancedfilterexception.cpp
  filterenums.cpp
  pybinder.cpp
  )

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
#set_target_properties(${TARGET_NAME} PROPERTIES CMAKE_INSTALL_RPATH "\$ORIGIN")

target_link_directories(${TARGET_NAME} PRIVATE
    /opt/local/lib
    ../../../external/mac/lib
    )


if (APPLE)
target_link_libraries(${TARGET_NAME}
  PUBLIC
    pybind11::module

  PRIVATE
    kipl
    fftw3
    fftw3f
    xml2
    tiff
    armadillo
    lapack
    blas
    AdvancedFilters
    cfitsio
    NeXus.1.0.0
    NeXusCPP.1.0.0
  )
endif(APPLE)

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX
            pybind11::module

          PRIVATE
            kipl
            fftw3
            fftw3f
            xml2
            tiff
            armadillo
            lapack
            blas
            AdvancedFilters
            cfitsio
            NeXus
            NeXusCPP
            )
    else()
        target_link_libraries(${TARGET_NAME}
          PUBLIC
            pybind11::module

          PRIVATE
            kipl
            fftw3
            fftw3f
            xml2
            tiff
            armadillo
            lapack
            blas
            AdvancedFilters
            cfitsio
            NeXus
            NeXusCPP
          )
    endif()
endif()

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

  #     execute_process(
  #       COMMAND for f in `ls *.1.0.0.*`; do ln -s $f `basename $f .1.0.0.dylib`.1.dylib; done 
  #       WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib"
  #     )
endif(DYNAMIC_LIB)
