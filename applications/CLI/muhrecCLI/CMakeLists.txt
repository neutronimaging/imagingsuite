cmake_minimum_required(VERSION 3.30)
set(TARGET_NAME "MuhRecCLI")
project(${TARGET_NAME} VERSION 4.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../frameworks/tomography/Framework/ReconFramework/include/
    ../../../core/algorithms/ImagingAlgorithms/include
    )

set(SRC_FILES
    main.cpp
    muhreccli.cpp
)

set(INC_FILES
    muhreccli.h
)

message("CLI App Muhrec" )

add_executable(${TARGET_NAME}
        ${SRC_FILES}
        ${INC_FILES}
    )

target_link_libraries(${TARGET_NAME} PRIVATE
    kipl
    ModuleConfig
    ImagingAlgorithms
    ReaderConfig
    ReconFramework
    )
if (APPLE)
    link_directories(../../../lib /opt/local/lib )

    target_link_directories(${TARGET_NAME} PRIVATE
        /opt/local/lib
        #../../../external/mac/lib/
        )

    target_link_libraries(${TARGET_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        kipl
        ModuleConfig
        ImagingAlgorithms
        ReaderConfig
        ReconFramework

        LibXml2::LibXml2
        armadillo::armadillo
        OpenBLAS::OpenBLAS
        #lapack
        #blas
    )
endif()

if (WIN32)
    # target_link_directories(${TARGET_NAME} PRIVATE
    #     ../../../../ExternalDependencies/windows/lib
    #     )

    target_link_libraries(${TARGET_NAME} PRIVATE
        kipl
        ModuleConfig
        ImagingAlgorithms
        ReaderConfig
        ReconFramework
    )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${TARGET_NAME})
