cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "FDKBackProjectors")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    /usr/include/libxml2
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../external/src/linalg
    ../../../../core/modules/ModuleConfig/include
    ../../../../../ExternalDependencies/macos/include/
    ../../../../../ExternalDependencies/macos/include/nexus
    ../../../../external/mac/include/
    ../../../../external/mac/include/nexus
    ../../../../external/mac/include/hdf5
    ../../Framework/ReconFramework/include
    )

# link_directories(../../../../lib /opt/local/lib ../../../../external/mac/lib)
#link_directories(/opt/local/lib ../../../../external/mac/lib)

set(INC_FILES
    src/fdkbackproj.h 
	src/fdkbackproj_global.h 
	src/genericbp.h 
    src/fdkreconbase.h 
    src/fdkbp.h 
    src/fdkbp_single.h
#        ../../src/ramp_filter.h
#        ../../src/fdk.h \
#        ../../src/bowtie_correction.h \
#        ../../src/delayload.h \
#        ../../src/file_util.h \
#        ../../src/plm_math.h \
#        ../../src/plm_timer.h \
#        ../../src/proj_image.h \
#        ../../src/proj_image_filter.h \
#        ../../src/proj_matrix.h \
#        ../../src/volume.h \
#        ../../src/threding.h
)

set(SRC_FILES
        src/fdkbackproj.cpp  
        src/genericbp.cpp 
        src/fdkreconbase.cpp 
        src/fdkbp.cpp 
        src/fdkbp_single.cpp

        ## Not used
#        ../../src/ramp_filter.cpp
#        ../../src/fdk.cxx \
#        ../../src/bowtie_correction.cxx \
#        ../../src/delayload.cxx \
#        ../../src/file_util.cxx \
#        ../../src/plm_math.cxx \
#        ../../src/plm_timer.cxx \
#        ../../src/proj_image.cxx \
#        ../../src/proj_matrix.cxx \
#        ../../src/proj_image_filter.cxx \
#        ../../src/proj_matrix.cxx \
#        ../../src/volume.cxx
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE FDKBACKPROJ_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})
if (APPLE)
    if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/local/lib
            ../../../../../ExternalDependencies/macos/arm64/lib
            )
    else()
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/local/lib
            ../../../../external/mac/lib/
            )
    endif()

    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        xml2
        fftw3
        fftw3f
        cfitsio
        tiff
        armadillo
        lapack
        blas
        NeXus.1.0.0
        NeXusCPP.1.0.0
    )
endif()

if (LINUX)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
        xml2
        fftw3
        fftw3f
        cfitsio
        tiff
        armadillo
        lapack
        blas
        # NeXus
        # NeXusCPP
    )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
