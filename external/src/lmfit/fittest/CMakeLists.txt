include_directories(
    ${CMAKE_SOURCE_DIR}/lib
    ${FitTest_SOURCE_DIR}/lib
    )

set(src_files runfittest.c)

add_executable(runfittest ${src_files})

target_link_libraries(runfittest ${lmfit_LIBRARY} ${FitTest_LIBRARIES})
