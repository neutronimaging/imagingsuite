

set(TARGET_NAME "ImagingQAAlgorithms")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
  ../../../core/kipl/kipl/include
  ../../../core/modules/ModuleConfig/include
)


set(INC_FILES
  ballanalysis.h
  collimationestimator.h
  imagingqaalgorithms.h
  profileextractor.h
  ballassemblyanalysis.h
  contrastsampleanalysis.h
  imagingqaalgorithms_global.h
  resolutionestimators.h
)

set(SRC_FILES
  ballanalysis.cpp
  collimationestimator.cpp
  imagingqaalgorithms.cpp
  resolutionestimators.cpp
  ballassemblyanalysis.cpp
  contrastsampleanalysis.cpp
  profileextractor.cpp
)

add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_compile_definitions(${TARGET_NAME} PRIVATE IMAGINGQAALGORITHMS_LIBRARY)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${TARGET_NAME} PUBLIC
        OpenMP::OpenMP_CXX
        kipl
    )
else()
target_link_libraries(${TARGET_NAME} 
      PUBLIC
        kipl
        )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
