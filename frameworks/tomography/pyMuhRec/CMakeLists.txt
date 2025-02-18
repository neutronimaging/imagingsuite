(VERSION 3.30)
message("PyMuhRec")
set(TARGET_NAME "muhrectomo") 
project(muhrectomo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../../../core/kipl/kipl/include
    ../../../core/modules/ModuleConfig/include
    ../Framework/ReconFramework/include
    ../Backprojectors/StdBackProjectors/include
    ../Backprojectors/FDKBackProjectors/src
    ../../../core/algorithms/ImagingAlgorithms/include
    )

set(INC_FILES
  src/reconstructor.h
)

set(SRC_FILES
  src/pyReconEngine.cpp
  src/pyReconException.cpp
  src/pyReconConfig.cpp
  src/pyReconFactory.cpp
  src/pyReconstructor.cpp
  src/reconstructor.cpp
  src/pybinder.cpp
)

pybind11_add_module(${TARGET_NAME}
  MODULE
  ${SRC_FILES}  
  ${INC_FILES}
  )
  
set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${TARGET_NAME} PUBLIC RECONFRAMEWORK_LIBRARY )

set(LINK_TYPE "MODULE")

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
          OpenMP::OpenMP_CXX
          kipl
          ImagingAlgorithms
          ModuleConfig
          ReconFramework
          ReaderConfig
          StdBackProjectors
          FDKBackProjectors          
        )
elseif (APPLE)
        target_link_libraries(${TARGET_NAME}
        PUBLIC
          kipl
          ImagingAlgorithms
          ModuleConfig
          ReconFramework
          ReaderConfig
          StdBackProjectors
          FDKBackProjectors
        )
elseif (WIN32)
    target_link_libraries(${TARGET_NAME}
    PUBLIC
      pybind11::module
      kipl
      ImagingAlgorithms
      ModuleConfig
      ReconFramework
      StdBackProjectors
      FDKBackProjectors
    )
endif()


set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
