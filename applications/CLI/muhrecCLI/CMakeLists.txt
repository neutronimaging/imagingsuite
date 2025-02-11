cmake_minimum_required(VERSION 3.5)
set(TARGET_NAME "MuhRecCLI")
project(${TARGET_NAME} VERSION 4.3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
#find_package(Qt6 REQUIRED COMPONENTS Core )

#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

if (APPLE)
include_directories(
    # /opt/local/include
    # /opt/local/include/libxml2
    # /usr/include/libxml2
    ../../core/kipl/kipl/include
    ../../core/modules/ModuleConfig/include
    ../../core/modules/ReaderConfig
    ../../core/algorithms/ImagingAlgorithms/include
    ../../GUI/qt/QtAddons
    ../../GUI/qt/QtImaging
    ../../GUI/qt/QtModuleConfigure
    ../../core/modules/ReaderGUI
    ../../frameworks/tomography/Framework/ReconFramework/include/
    ../../core/algorithms/ImagingAlgorithms/include
    # ../../external/mac/include/
    #  ../../../ExternalDependencies/macos/include
    #  ../../../ExternalDependencies/macos/include/nexus
    # ../../external/mac/include/hdf5
    )
endif()

if (WIN32)
include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/modules/ReaderConfig
    ../../../core/algorithms/ImagingAlgorithms/include
    ../../../frameworks/tomography/Framework/ReconFramework/include/
    ../../../core/algorithms/ImagingAlgorithms/include
    # ../../../../ExternalDependencies/windows/include
    # ../../../../ExternalDependencies/windows/include/libxml2
    # ../../../../ExternalDependencies/windows/include/nexus
    # ../../../external/include/
    # ../../../external/mac/include/hdf5
    )
endif()

set(SRC_FILES
    main.cpp
    muhreccli.cpp
)

set(INC_FILES
    muhreccli.h
)

message(CLIApp Qt${QT_VERSION_MAJOR})

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
