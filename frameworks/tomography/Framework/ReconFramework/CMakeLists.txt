(VERSION 3.30)
set(TARGET_NAME "ReconFramework")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/modules/ReaderConfig
)

set(INC_FILES
    include/ReconHelpers.h
    include/ReconFramework.h
    include/ReconFactory.h
    include/ReconException.h
    include/ReconEngine.h
    include/ReconConfig.h
    include/ProjectionReader.h
    include/PreprocModuleBase.h
    include/ModuleItem.h
    include/ReconFramework_global.h
    include/BackProjectorModuleBase.h
    include/processtiminglogger.h
)

set(SRC_FILES
    src/ReconHelpers.cpp
    src/ReconFramework.cpp
    src/ReconFactory.cpp
    src/ReconException.cpp
    src/ReconConfig.cpp
    src/ProjectionReader.cpp
    src/PreprocModuleBase.cpp
    src/ModuleItem.cpp
    src/BackProjectorModuleBase.cpp
    src/processtiminglogger.cpp
    src/ReconEngine.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PUBLIC RECONFRAMEWORK_LIBRARY)


message(${TARGET_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR})

target_link_libraries(${TARGET_NAME}
    PUBLIC
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReaderConfig
        LibXml2::LibXml2
)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
