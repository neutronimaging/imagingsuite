cmake_minimum_required(VERSION 3.14)
message("ReaderConfig")
set(TARGET_NAME "ReaderConfig")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
../../../core/kipl/kipl/include
../../../core/modules/ModuleConfig/include
../../../core/algorithms/ImagingAlgorithms/include
)

if (APPLE)
include_directories(
    ../../../../ExternalDependencies/macos/include
    )
endif()

if (WIN32)
include_directories(
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
    # ../../../external/src/linalg
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
    folders.h
)

set(SRC_FILES
    readerconfig.cpp
    buildfilelist.cpp
    fileset.cpp
    imagereader.cpp
    readerexception.cpp
    imagewriter.cpp
    analyzefileext.cpp
    folders.cpp
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

#target_compile_definitions(${TARGET_NAME} PRIVATE READERCONFIG_LIBRARY HAVE_NEXUS)
target_compile_definitions(${TARGET_NAME} PRIVATE READERCONFIG_LIBRARY)


#add_definitions(-DHAVE_NEXUS)

if (WIN32)
    # target_link_directories(${TARGET_NAME} PRIVATE
    # ../../../../ExternalDependencies/windows/lib
    # ../../../../imagingsuite/external/lib64
    # ../../../../imagingsuite/external/lib64/nexus
    # )
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
        kipl
        ImagingAlgorithms


        )
endif()

# if (APPLE)
#     target_link_libraries(${TARGET_NAME}
#     PUBLIC
#     kipl
#     ImagingAlgorithms
#     ${NEXUS_LIBRARIES}
#   )
# endif()

if (UNIX)

target_link_libraries(${TARGET_NAME}
    PUBLIC
    kipl
    ImagingAlgorithms
    LibXml2::LibXml2
    # ${NEXUS_LIBRARIES}
  )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
