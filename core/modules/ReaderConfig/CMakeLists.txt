cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ReaderConfig")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../core/modules/ModuleConfig/include
    ../../../../ExternalDependencies/macos/include
    )
endif()

if (WIN32)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../../ExternalDependencies/windows/include
    ../../../../ExternalDependencies/windows/include/nexus
    ../../../../ExternalDependencies/windows/include/cfitsio
    ../../../../ExternalDependencies/windows/include/libxml2
    ../../../external/include/
    )
endif()

if (UNIX)
include_directories(
    /usr/include/libxml2
    ../../../core/kipl/kipl/include
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../external/src/linalg
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    )
endif()

set(INC_FILES
    readerconfig.h
    fileset.h
    readerconfig_global.h
    buildfilelist.h
    imagereader.h
    readerexception.h
    imagewriter.h
    analyzefileext.h
)

set(SRC_FILES
    readerconfig.cpp
    buildfilelist.cpp
    fileset.cpp
    imagereader.cpp
    readerexception.cpp
    imagewriter.cpp
    analyzefileext.cpp
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
# set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE READERCONFIG_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    ../../../../ExternalDependencies/windows/lib
    ../../../../imagingsuite/external/lib64
    ../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PRIVATE
        kipl
        ImagingAlgorithms
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
    target_link_libraries(${TARGET_NAME}
    PUBLIC

    kipl
    ImagingAlgorithms
  )
endif()

if (UNIX)

target_link_libraries(${TARGET_NAME}
    PUBLIC
    kipl
    ImagingAlgorithms
  )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
