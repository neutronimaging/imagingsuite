# ~~~
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2021 ImaginSuite contributors (https://github.com/neutronimaging/imagingsuite)
# ~~~
set(TARGET_NAME "advfilters")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenMP)

include_directories(
    ../../kipl/kipl/include
    )

if (WIN32)
    include_directories(
        ../../../core/kipl/kipl/include
        ../../../core/modules/ModuleConfig/include
        )
endif()


set(SRC_FILES 
    advancedfilters.cpp
    advancedfilterexception.cpp
    filterenums.cpp
    pybinder.cpp
)

pybind11_add_module(${TARGET_NAME}
  ${LINK_TYPE}
  MODULE
  ${SRC_FILES}  
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

set(LINK_TYPE "MODULE")

message("PyAdvancedFilters ${CMAKE_HOST_SYSTEM_PROCESSOR}")
if (APPLE)
target_link_libraries(${TARGET_NAME}
  PUBLIC
    pybind11::module
    kipl
    AdvancedFilters
  )
endif(APPLE)

if (WIN32)   
    target_link_libraries(${TARGET_NAME}
    PUBLIC
      pybind11::module
      OpenMP::OpenMP_CXX

      kipl
      AdvancedFilters
    )
endif()

if (UNIX AND NOT APPLE)
    
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX
            # pybind11::module

            kipl
            AdvancedFilters
            )
    else()
        target_link_libraries(${TARGET_NAME}
          PUBLIC
            # pybind11::module

            kipl
            AdvancedFilters
          )
    endif()
endif()

set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)