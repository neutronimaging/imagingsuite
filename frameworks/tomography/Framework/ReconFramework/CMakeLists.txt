cmake_minimum_required(VERSION 3.14)
set(TARGET_NAME "ReconFramework")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(LibXml2 REQUIRED)

include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../core/modules/ModuleConfig/include
    ../../../../core/modules/ReaderConfig
)

if (APPLE)
include_directories(
    ../../../../../ExternalDependencies/macos/include
    )
endif()

if (WIN32)
include_directories(
    ../../../../../ExternalDependencies/windows/include
    ../../../../../ExternalDependencies/windows/include/nexus
    ../../../../../ExternalDependencies/windows/include/cfitsio
    ../../../../../ExternalDependencies/windows/include/libxml2
    ../../../../external/include/
)
endif()

# if (UNIX)
# include_directories(
#     /usr/include/libxml2
#     ../../../../core/kipl/kipl/include
#     ../../../../core/algorithms/ImagingAlgorithms/include
#     ../../../../external/src/linalg
#     ../../../../core/modules/ModuleConfig/include
#     ../../../../core/modules/ReaderConfig
#     )
# endif()

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

#target_compile_definitions(${TARGET_NAME} PUBLIC RECONFRAMEWORK_LIBRARY HAVE_NEXUS)
target_compile_definitions(${TARGET_NAME} PUBLIC RECONFRAMEWORK_LIBRARY)

# add_definitions(-DHAVE_NEXUS)


message(${TARGET_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR})

if (WIN32)
    target_link_directories(${TARGET_NAME} PUBLIC
    ../../../../../ExternalDependencies/windows/lib
    ../../../../../imagingsuite/external/lib64
    ../../../../../imagingsuite/external/lib64/nexus
    )
endif()   

target_link_libraries(${TARGET_NAME}
    PUBLIC
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReaderConfig
)

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
