cmake_minimum_required(VERSION 3.5)
set(TARGET_NAME "verticalslicer")
project(${TARGET_NAME} VERSION 4.3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
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
        ../../../../ExternalDependencies/windows/include/libxml2
        ../../../../ExternalDependencies/windows/include/nexus
        ../../../external/include/
    )
endif()

set(SRC_FILES
    main.cpp
)

set(INC_FILES

)

message(CLIApp)

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
