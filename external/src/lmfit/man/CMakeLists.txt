function(one_page pname section)
    add_custom_command(
        OUTPUT "${pname}.${section}"
        COMMAND pod2man -s ${section} -c "lmfit manual"
        "${CMAKE_CURRENT_SOURCE_DIR}/${pname}.pod"
        "${CMAKE_CURRENT_BINARY_DIR}/${pname}.${section}"
        DEPENDS ${pname}.pod
        )
    add_custom_command(
        OUTPUT ${pname}.html
        COMMAND pod2html --title="lmfit manual" --noindex
        ${CMAKE_CURRENT_SOURCE_DIR}/${pname}.pod
        > ${CMAKE_CURRENT_BINARY_DIR}/${pname}.html
        DEPENDS ${pname}.pod
        )
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${pname}.${section}
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man${section}"
        )
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${pname}.html
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/lmfit/html"
        )
endfunction()

add_custom_target(
    man ALL
    DEPENDS lmcurve.3 lmmin.3 lmmin2.3 lmfit.7
    )

add_custom_target(
    html ALL
    DEPENDS lmcurve.html lmmin.html lmmin2.html lmfit.html
    )

one_page(lmcurve 3)
one_page(lmmin 3)
one_page(lmmin2 3)
one_page(lmfit 7)
