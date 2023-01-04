set(TARGET_NAME "advancedfilters")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(
    ../../kipl/kipl/include
    ../../../../ExternalDependencies/macos/include
    )

if (WIN32)
    include_directories(
        ../../../core/kipl/kipl/include
        ../../../core/modules/ModuleConfig/include
        ../../../external/include/
        ../../../../ExternalDependencies/windows/include
        ../../../../ExternalDependencies/windows/include/nexus
        ../../../../ExternalDependencies/windows/include/cfitsio
        )
endif()


set(SRC_FILES 
    advancedfilters.cpp
    advancedfilterexception.cpp
    filterenums.cpp
    pybinder.cpp
)

# create python module
# add_library(${TARGET_NAME}
#   ${LINK_TYPE}
#   MODULE

#   advancedfilters.cpp
#   advancedfilterexception.cpp
#   filterenums.cpp
#   pybinder.cpp
#   )

pybind11_add_module(${TARGET_NAME}
  ${LINK_TYPE}
  MODULE
  ${SRC_FILES}  
  # ${INC_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

message("PyAdvancedFilters ${CMAKE_HOST_SYSTEM_PROCESSOR}")
if (APPLE)
target_link_libraries(${TARGET_NAME}
  PUBLIC
    # pybind11::module
    kipl
    AdvancedFilters
  )
endif(APPLE)

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    ../../../../ExternalDependencies/windows/lib
    ../../../../imagingsuite/external/lib64
    ../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PUBLIC
      # pybind11::module

    PRIVATE
        kipl
        libxml2
        cfitsio
        libtiff
        liblapack
        libopenblas
        libfftw3-3
        libfftw3f-3
        NeXus
        NeXusCPP
        Iphlpapi
    )
endif()

if (UNIX AND NOT APPLE)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET_NAME} PUBLIC
            OpenMP::OpenMP_CXX
            # pybind11::module

          PRIVATE
            kipl
            fftw3
            fftw3f
            xml2
            tiff
            armadillo
            lapack
            blas
            AdvancedFilters
            cfitsio
            NeXus
            NeXusCPP
            )
    else()
        target_link_libraries(${TARGET_NAME}
          PUBLIC
            # pybind11::module

          PRIVATE
            kipl
            fftw3
            fftw3f
            xml2
            tiff
            armadillo
            lapack
            blas
            AdvancedFilters
            cfitsio
            NeXus
            NeXusCPP
          )
    endif()
endif()

set_target_properties(${TARGET_NAME}
  PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
  )


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)