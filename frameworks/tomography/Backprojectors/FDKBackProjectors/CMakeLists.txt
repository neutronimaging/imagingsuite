cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "FDKBackProjectors")
project(kipl LANGUAGES CXX)


include_directories(
    ../../../../core/kipl/kipl/include
    ../../../../core/algorithms/ImagingAlgorithms/include
    ../../../../core/modules/ModuleConfig/include
    ../../Framework/ReconFramework/include
    )


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

target_compile_definitions(${TARGET_NAME} PRIVATE FDKBACKPROJ_LIBRARY)

message(${CMAKE_HOST_SYSTEM_PROCESSOR})

target_link_libraries(${TARGET_NAME}
        PUBLIC
        kipl
        ImagingAlgorithms
        ModuleConfig
        ReconFramework
    )


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
