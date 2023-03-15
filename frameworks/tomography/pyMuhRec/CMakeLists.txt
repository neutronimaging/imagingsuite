cmake_minimum_required(VERSION 3.14)
message("PyMuhRec")
set(TARGET_NAME "muhrectomo") 
project(muhrectomo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(LAPACK)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../../ExternalDependencies/macos/include/
    ../../../../ExternalDependencies/macos/include/nexus
    ../Framework/ReconFramework/include
    ../Backprojectors/StdBackProjectors/include
    ../Backprojectors/FDKBackProjectors/src
    ../../../core/algorithms/ImagingAlgorithms/include
    # ${pybind11_sources_SOURCE_DIR}/include
    )

if (WIN32)
  include_directories(
      ../../../core/kipl/kipl/include
      ../../../core/kipl/kipl/include
      ../../../core/modules/ModuleConfig/include
      ../../../external/include/
      ../../../../ExternalDependencies/windows/include
      ../../../../ExternalDependencies/windows/include/nexus
      ../../../../ExternalDependencies/windows/include/cfitsio
      ../../../../ExternalDependencies/windows/include/libxml2
      ../Framework/ReconFramework/include
      ../Backprojectors/StdBackProjectors/include
      ../Backprojectors/FDKBackProjectors/src
      ../../../core/algorithms/ImagingAlgorithms/include
      )
endif()

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

pybind11_add_module(${TARGET_NAME}
  MODULE
  ${SRC_FILES}  
  ${INC_FILES}
  )
  
set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${TARGET_NAME} PUBLIC RECONFRAMEWORK_LIBRARY HAVE_NEXUS)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

set(LINK_TYPE "MODULE")

if (UNIX)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
      target_link_libraries(${TARGET_NAME}
        PUBLIC
          OpenMP::OpenMP_CXX
          # pybind11::module

          kipl
          ImagingAlgorithms
          ModuleConfig
          ReconFramework
          ReaderConfig
          StdBackProjectors
          FDKBackProjectors
          
        )
      else()
        target_link_libraries(${TARGET_NAME}
        PUBLIC

          # pybind11::module

          kipl
          ImagingAlgorithms
          ModuleConfig
          ReconFramework
          ReaderConfig
          StdBackProjectors
          FDKBackProjectors
        )
      endif()
endif()

# if (APPLE)
#     target_link_directories(${TARGET_NAME} PRIVATE
#         ../../../../ExternalDependencies/macos/${CMAKE_HOST_SYSTEM_PROCESSOR}/lib
#         )

#     target_link_libraries(${TARGET_NAME}
#         PUBLIC
#         pybind11::module

#         kipl
#         ImagingAlgorithms
#         ModuleConfig
#         ReconFramework
#         StdBackProjectors
#         FDKBackProjectors
#     )
# endif()

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    ../../../../ExternalDependencies/windows/lib
    ../../../../imagingsuite/external/lib64
    ../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
      pybind11::module

      kipl
      ImagingAlgorithms
      ModuleConfig
      ReconFramework
      StdBackProjectors
      FDKBackProjectors
      # libxml2
      # cfitsio
      # libtiff
      # armadillo
      # liblapack
      # libopenblas
      # libfftw3-3
      # libfftw3f-3
      # NeXus
      # NeXusCPP
      # Iphlpapi
    )
endif()

# if (UNIX AND NOT APPLE)
#     find_package(OpenMP)
#     if(OpenMP_CXX_FOUND)
#         target_link_libraries(${TARGET_NAME} PUBLIC
#             OpenMP::OpenMP_CXX
#             pybind11::module

#             kipl
#             ImagingAlgorithms
#             ReaderConfig
#             ModuleConfig
#             ReconFramework
#             StdBackProjectors
#             FDKBackProjectors
#             # xml2
#             # tiff
#             # fftw3
#             # fftw3f
#             # cfitsio
#             # NeXus
#             # NeXusCPP
#             )
#     else()
#         target_link_libraries(${TARGET_NAME}
#           PUBLIC
#             pybind11::module

#             kipl
#             ImagingAlgorithms
#             ReaderConfig
#             ModuleConfig
#             ReconFramework
#             StdBackProjectors
#             FDKBackProjectors
#             # xml2
#             # tiff
#             # fftw3
#             # fftw3f
#             # cfitsio
#             # NeXus
#             # NeXusCPP
#           )
#     endif()
# endif()



set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

#if(DYNAMIC_LIB)
  
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
#endif(DYNAMIC_LIB)
