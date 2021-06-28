cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ImagingAlgorithms")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    )


#include_directories(../../kipl/kipl/include /opt/local/include ../../../external/src/linalg)


link_directories(../../../../lib /opt/local/lib )

set(INC_FILES
  include/StripeFilter.h  
  include/SpotClean.h  
  include/PolynomialCorrection.h  
  include/ImagingException.h  
  include/TranslateProjections.h  
  include/MorphSpotClean.h  
  include/pixelinfo.h  
  include/projectionfilter.h
  include/ImagingAlgorithms_global.h  
  include/gammaclean.h  
  include/averageimage.h  
  include/piercingpointestimator.h  
  include/ReferenceImageCorrection.h  
  include/tomocenter.h
)

set(SRC_FILES
  src/StripeFilter.cpp  
  src/SpotClean.cpp  
  src/PolynomialCorrection.cpp  
  src/ImagingException.cpp  
  src/TranslateProjections.cpp  
  src/MorphSpotClean.cpp  
  src/pixelinfo.cpp
  src/projectionfilter.cpp
  src/gammaclean.cpp  
  src/averageimage.cpp  
  src/piercingpointestimator.cpp  
  src/ReferenceImageCorrection.cpp  
  src/tomocenter.cpp  
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE KIPL_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

target_link_directories(${TARGET_NAME} PRIVATE
    /opt/local/lib
    ../../../external/mac/lib/
    )

target_link_libraries(${TARGET_NAME}
    PRIVATE
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

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
