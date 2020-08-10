set(demos
    curve1
    surface1
    nonlin1
    )

foreach(app ${demos})
    add_executable(${app} ${app}.c)
    target_include_directories(${app} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${lmfit_SOURCE_DIR}/lib)
    target_link_libraries(${app} ${lmfit_LIBRARY})
endforeach()
