# LinkLibMath
#
# Usage:
#   link_libm(target)
#
# Copyright 2018 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

include(CheckFunctionExists)

function(link_libm target)
    CHECK_FUNCTION_EXISTS(pow HAVE_MATH_FCT)
    if(NOT HAVE_MATH_FCT)
        list(APPEND CMAKE_REQUIRED_LIBRARIES m)
        CHECK_FUNCTION_EXISTS(sqrt LIBM_HAS_FCT)
        if(LIBM_HAS_FCT)
            target_link_libraries(${target} m)
        else()
            message(FATAL_ERROR "Standard math library not found")
        endif()
    endif()
endfunction()
