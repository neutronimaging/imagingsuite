# interactive programs for trying things during development
set(runnables
    run_qr
    )

foreach(app ${runnables})
    add_executable(${app} ${app}.c)
    target_include_directories(${app} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${lmfit_SOURCE_DIR}/lib)
    target_link_libraries(${app} ${lmfit_LIBRARY})
endforeach()

# unit tests
set(unit_tests
    test_inversion
    )

foreach(app ${unit_tests})
    add_executable(${app} ${app}.c)
    add_test(NAME ${app} COMMAND ${app})
    target_include_directories(${app} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${lmfit_SOURCE_DIR}/lib)
    target_link_libraries(${app} ${lmfit_LIBRARY})
endforeach()

# functional tests
add_test(NAME curve1   COMMAND curve1)
add_test(NAME surface1 COMMAND surface1)
