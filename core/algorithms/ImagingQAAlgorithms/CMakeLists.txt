cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ImagingQAAlgorithms")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    ../../../core/kipl/kipl/include
    # ../../../external/src/linalg
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../external/mac/include/
    ../../../../ExternalDependencies/macos/include
    ../../../../ExternalDependencies/macos/include/nexus
    ../../../external/mac/include/hdf5
    )

link_directories(../../../../lib /opt/local/lib )

set(INC_FILES
  ballanalysis.h
  collimationestimator.h
  imagingqaalgorithms.h
  profileextractor.h
  ballassemblyanalysis.h
  contrastsampleanalysis.h
  imagingqaalgorithms_global.h
  resolutionestimators.h
)

set(SRC_FILES
  ballanalysis.cpp
  collimationestimator.cpp
  imagingqaalgorithms.cpp
  resolutionestimators.cpp
  ballassemblyanalysis.cpp
  contrastsampleanalysis.cpp
  profileextractor.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE IMAGINGALGORITHMS_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

target_link_directories(${TARGET_NAME} PRIVATE
    /opt/local/lib
    ../../../external/mac/lib/
    )
    
if (APPLE)
target_link_libraries(${TARGET_NAME}
    PRIVATE
    kipl
    xml2
    fftw3
    fftw3f
    cfitsio
    tiff
    armadillo
    lapack
    blas
    NeXus.1.0.0
    NeXusCPP.1.0.0
  )
endif()

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX

          PRIVATE
            kipl
            xml2
            fftw3
            fftw3f
            cfitsio
            tiff
            armadillo
            lapack
            blas
            NeXus
            NeXusCPP
        )
    else()
    target_link_libraries(${TARGET_NAME} 
          PRIVATE
            kipl
            xml2
            fftw3
            fftw3f
            cfitsio
            tiff
            armadillo
            lapack
            blas
            NeXus
            NeXusCPP
        )
    endif()
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
