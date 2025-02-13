cmake_minimum_required(VERSION 3.30)
set(TARGET_NAME "verticalslicer")
project(${TARGET_NAME} VERSION 4.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
)

set(SRC_FILES
    main.cpp
)

set(INC_FILES

)

message(CLI Vertical Slicer)

add_executable(${TARGET_NAME}
        ${SRC_FILES}
        ${INC_FILES}
    )

target_link_libraries(${TARGET_NAME} PUBLIC
    kipl
    ReaderConfig
)


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${TARGET_NAME})
