
configure_file(nexus.pc.in nexus.pc @ONLY)
configure_file(nexus-cpp.pc.in nexus-cpp.pc @ONLY)
configure_file(nexus-f77.pc.in nexus-f77.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nexus.pc  
              ${CMAKE_CURRENT_BINARY_DIR}/nexus-cpp.pc
              ${CMAKE_CURRENT_BINARY_DIR}/nexus-f77.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

