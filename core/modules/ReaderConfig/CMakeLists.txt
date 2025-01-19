cmake_minimum_required(VERSION 3.14)
message("ReaderConfig")
set(TARGET_NAME "ReaderConfig")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
../../../core/kipl/kipl/include
../../../core/modules/ModuleConfig/include
../../../core/algorithms/ImagingAlgorithms/include
)

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

target_compile_definitions(${TARGET_NAME} PRIVATE READERCONFIG_LIBRARY)

if (WIN32)
    
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
    # LibXml2::LibXml2
  )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
