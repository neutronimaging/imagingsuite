cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME "AdvancedFilters")
project(kipl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ../../../core/kipl/kipl/include
    )

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

target_compile_definitions(${TARGET_NAME} PRIVATE ADVANCEDFILTERS_LIBRARY)

if (APPLE)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
        kipl
    )
endif()

if (WIN32)
 
    target_link_libraries(${TARGET_NAME}
    PUBLIC
        kipl
    )
endif()

if (LINUX)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
        kipl
    )
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
