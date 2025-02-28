set(TARGET_NAME "ScatteringCorrection")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../../../core/algorithms/ImagingAlgorithms/include
)

set(INC_FILES
  src/ImageNormalization.h
  src/ScatterEstimation.h
  src/ReferenceLoader.h	
  src/SegmentBB.h
  src/ScatteringCorrection.h
  src/ScatteringCorrection_global.h
)

set(SRC_FILES
  src/ImageNormalization.cpp
  src/ScatterEstimation.cpp
  src/ReferenceLoader.cpp
  src/SegmentBB.cpp
  src/ScatteringCorrection.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE SCATTERINGCORRECTION_LIBRARY HAVE_NEXUS)

add_definitions(-DHAVE_NEXUS)

    
target_link_libraries(${TARGET_NAME}
    PUBLIC
    kipl
    ImagingAlgorithms
  )

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
