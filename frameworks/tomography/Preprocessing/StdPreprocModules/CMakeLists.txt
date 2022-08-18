cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "StdPreprocModules")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    /usr/include/libxml2
    ../../Framework/ReconFramework/include/
#    ../../../../external/src/linalg
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../../ExternalDependencies/macos/include
    ../../../../../ExternalDependencies/macos/include/nexus
    ../../../../external/mac/include/
    ../../../../external/mac/include/nexus
    ../../../../external/mac/include/hdf5
    )
endif()

if (WIN32)
    include_directories(
        ../../../../core/kipl/kipl/include
        ../../../../core/modules/ModuleConfig/include
        ../../../../core/algorithms/ImagingAlgorithms/include
        ../../Framework/ReconFramework/include
        ../../../../../ExternalDependencies/windows/include
        ../../../../../ExternalDependencies/windows/include/nexus
        ../../../../../ExternalDependencies/windows/include/cfitsio
        ../../../../../ExternalDependencies/windows/include/libxml2
        ../../../../external/include/
        )
endif()

if (UNIX)
include_directories(
    /usr/include/libxml2
    ../../Framework/ReconFramework/include/
    ../../../../core/kipl/kipl/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    )
endif()

link_directories(../../../../../lib /opt/local/lib )

set(INC_FILES
include/AdaptiveFilter.h
include/ISSfilter.h
include/ProjectionFilter.h
include/StdPreprocModules_global.h
include/BasicRingClean.h
include/MedianMixRingClean.h
include/SinoSpotClean.h
include/TranslateProjectionsModule.h
include/CameraStripeClean.h
include/MorphSpotCleanModule.h
include/SpotClean.h
include/TranslatedProjectionWeighting.h
include/DataScaler.h
include/NormPlugins.h
include/SpotClean2.h
include/WaveletRingClean.h
include/GammaSpotCleanModule.h
include/PolynomialCorrectionModule.h
include/SpotRingClean.h
include/bblognorm.h
include/GeneralFilter.h
include/PreprocEnums.h
include/StdPreprocModules.h
include/ReplaceUnderexposedModule.h
include/VoStripeCleanModule.h
)

set(SRC_FILES 
  src/AdaptiveFilter.cpp
  src/SinoSpotClean.cpp
  src/BasicRingClean.cpp
  src/SpotClean.cpp
  src/CameraStripeClean.cpp
  src/SpotClean2.cpp
  src/DataScaler.cpp
  src/SpotRingClean.cpp
  src/GammaSpotCleanModule.cpp
  src/StdPreprocModules.cpp
  src/GeneralFilter.cpp
  src/TranslateProjectionsModule.cpp
  src/ISSfilter.cpp
  src/TranslatedProjectionWeighting.cpp
  src/MedianMixRingClean.cpp
  src/WaveletRingClean.cpp
  src/MorphSpotCleanModule.cpp
  src/bblognorm.cpp
  src/NormPlugins.cpp
  src/PolynomialCorrectionModule.cpp
  src/PreprocEnums.cpp
  src/ProjectionFilter.cpp
  src/ReplaceUnderexposedModule.cpp
  src/VoStripeCleanModule.cpp
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
  )

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE STDPREPROCMODULES_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

  message(${CMAKE_HOST_SYSTEM_PROCESSOR})
  if (WIN32)
  target_link_directories(${TARGET_NAME} PRIVATE
  ../../../../../ExternalDependencies/windows/lib
  ../../../../../imagingsuite/external/lib64
  ../../../../../imagingsuite/external/lib64/nexus
  )
  
  target_link_libraries(${TARGET_NAME}
  PRIVATE
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        libxml2
        cfitsio
        libtiff
        liblapack
        libopenblas
        libfftw3-3
        libfftw3f-3
        NeXus
        NeXusCPP
        Iphlpapi

      )
endif()

  if (APPLE)
      target_link_directories(${TARGET_NAME} PRIVATE
        /opt/local/lib
        ../../../../../ExternalDependencies/macos/${CMAKE_HOST_SYSTEM_PROCESSOR}/lib
      )
  
      target_link_libraries(${TARGET_NAME}
          PRIVATE
          kipl
          ImagingAlgorithms
          ModuleConfig
          ReconFramework
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

  install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
