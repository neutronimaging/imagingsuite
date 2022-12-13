cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "AdvancedFilters")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    /opt/local/include
    /opt/local/include/libxml2
    /usr/include/libxml2
    ../../../core/kipl/kipl/include
    )

# link_directories(/opt/local/lib ../../../../external/mac/lib)

set(INC_FILES
    AnisoDiff.h
    ISSfilterQ3D.h
    advancedfilterexception.h
    lambdaest.h
    ISSfilter.h
    ISSfilterQ3D.hpp
    advancedfilters.h
    separabelfilters.h
    ISSfilter.hpp
    ISSfilterQ3Dp.h
    advancedfilters_global.h
    shock_filter.h
    ISSfilter2D.h
    ISSfilterQ3Dp.hpp
    diff_filterbase.h
    ISSfilterOrig3D.h
    NonLinDiffAOS.h
    filterenums.h
)

set(SRC_FILES
    advancedfilterexception.cpp	
    filterenums.cpp
)


add_library(${TARGET_NAME} SHARED
  ${SRC_FILES}
  ${INC_FILES}
)


set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "\$ORIGIN")

target_compile_definitions(${TARGET_NAME} PRIVATE ADVANCEDFILTERS_LIBRARY)

add_definitions(-DHAVE_NEXUS)

if (APPLE)
    target_link_directories(${TARGET_NAME} PRIVATE
        /opt/local/lib
        ../../../../ExternalDependencies/macos/${CMAKE_HOST_SYSTEM_PROCESSOR}/lib
        ../../../external/mac/lib/
    )

    target_link_libraries(${TARGET_NAME}
        PUBLIC
        kipl
        # tiff
        # armadillo
        # lapack
        # blas
        # NeXus.1.0.0
        # NeXusCPP.1.0.0
    )
endif()

if (WIN32)
    target_link_directories(${TARGET_NAME} PRIVATE
    ../../../../ExternalDependencies/windows/lib
    ../../../../imagingsuite/external/lib64
    ../../../../imagingsuite/external/lib64/nexus
    )
    
    target_link_libraries(${TARGET_NAME}
    PRIVATE
        kipl
        libxml2
        cfitsio
        libtiff
        # armadillo
        liblapack
        libopenblas
        libfftw3-3
        libfftw3f-3
        NeXus
        NeXusCPP
        Iphlpapi
    )
endif()

if (LINUX)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
        fftw3
	    fftw3f
        tiff
        armadillo
        lapack
        blas
        NeXus
        NeXusCPP
    )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
