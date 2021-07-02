cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ModuleConfig")
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

link_directories(../../../../lib /opt/local/lib )

set(INC_FILES
  include/ModuleConfig_global.h 
  include/publication.h 
  include/ProcessModuleBase.h 
  include/ParameterHandling.h 
  include/ModuleItemBase.h 
  include/ModuleException.h 
  include/ModuleConfig.h 
  include/ConfigBase.h 
  include/datamodulebase.h 
  include/modulelibnamemanger.h 
)

set(SRC_FILES
  src/publication.cpp 
  src/stdafx.cpp 
  src/ProcessModuleBase.cpp 
  src/ParameterHandling.cpp 
  src/ModuleItemBase.cpp 
  src/ModuleException.cpp 
  src/ModuleConfig.cpp 
  src/ConfigBase.cpp 
  src/datamodulebase.cpp 
  src/modulelibnamemanger.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE MODULECONFIG_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

target_link_directories(${TARGET_NAME} PRIVATE
    /opt/local/lib
    ../../../external/mac/lib/
    )

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

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
