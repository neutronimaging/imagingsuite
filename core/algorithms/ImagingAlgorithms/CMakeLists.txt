cmake_minimum_required(VERSION 3.30)

set(TARGET_NAME "ImagingAlgorithms")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    )
endif()

include_directories(
  ../../../core/kipl/kipl/include
  ../../../core/modules/ModuleConfig/include
)

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
  include/ReplaceUnderexposed.h
  include/tomocenter.h
  include/normalizeimage.h
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
  src/ReplaceUnderexposed.cpp
  src/tomocenter.cpp  
  src/normalizeimage.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE IMAGINGALGORITHMS_LIBRARY )
    
if (APPLE)
target_link_libraries(${TARGET_NAME}
    PUBLIC
    kipl
  )
endif()

if (WIN32)
   
    target_link_libraries(${TARGET_NAME}
    PUBLIC
      kipl
      OpenBLAS::OpenBLAS
      )
endif()

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX

          PUBLIC
            kipl
        )
    else()
    target_link_libraries(${TARGET_NAME} 
          PUBLIC
            kipl
        )
    endif()
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
