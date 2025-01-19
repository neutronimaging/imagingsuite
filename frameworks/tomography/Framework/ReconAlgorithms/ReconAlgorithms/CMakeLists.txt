cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "ReconAlgorithms")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ../../../../../core/kipl/kipl/include
    ../../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../../core/modules/ModuleConfig/include
    )

set(INC_FILES
    backprojectorbase.h
    nnforwardprojector.h
    basicbackprojector.h
    reconalgorithmexception.h
    basicforwardprojector.h
    reconalgorithms.h
    forwardprojectorbase.h
    reconalgorithms_global.h
    linearforwardprojector.h
)

set(SRC_FILES
    backprojectorbase.cpp
    linearforwardprojector.cpp
    basicbackprojector.cpp
    nnforwardprojector.cpp
    basicforwardprojector.cpp
    reconalgorithmexception.cpp
    forwardprojectorbase.cpp
    reconalgorithms.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE RECONALGORITHMS_LIBRARY)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})

target_link_libraries(${TARGET_NAME}
        PUBLIC
        kipl
        ImagingAlgorithms
        ModuleConfig
    )

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
