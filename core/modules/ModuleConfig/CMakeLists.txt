cmake_minimum_required(VERSION 3.30)

set(TARGET_NAME "ModuleConfig")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
)

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
  include/modulelibnamemanager.h 
)

set(SRC_FILES
  src/publication.cpp 
  src/ProcessModuleBase.cpp 
  src/ParameterHandling.cpp 
  src/ModuleItemBase.cpp 
  src/ModuleException.cpp 
  src/ModuleConfig.cpp 
  src/ConfigBase.cpp 
  src/datamodulebase.cpp 
  src/modulelibnamemanager.cpp
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE MODULECONFIG_LIBRARY)


target_link_libraries(${TARGET_NAME}
    PUBLIC
    kipl
    LibXml2::LibXml2
  )

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
