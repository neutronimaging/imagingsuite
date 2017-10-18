## Process this file with cmake
#=============================================================================
#  NeXus - Neutron & X-ray Common Data Format
#  
#  CMakeLists for building the NeXus library and applications.
#
#  Copyright (C) 2011 Stephen Rankin
#  
#  This library is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by the
#  Free Software Foundation; either version 2 of the License, or (at your
#  option) any later version.
# 
#  This library is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
#  for more details.
# 
#  You should have received a copy of the GNU Lesser General Public License
#  along with this library; if not, write to the Free Software Foundation,
#  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#             
#  For further information, see <http://www.nexusformat.org>
#
#=============================================================================
include(FindDoxygen)

if (DOXYGEN_FOUND)

    SET(SOURCES Doxyfile_c Doxyfile_cpp Doxyfile_f90 Doxyfile_f77 preamble.h)
    set_directory_properties(PROPERTIES 
        ADDITIONAL_MAKE_CLEAN_FILES "html-c html-cpp html-f77 html-f90")

    foreach(F ${SOURCES})
        configure_file(${F} ${F} @ONLY)
    endforeach()

    #-------------------------------------------------------------------------
    ADD_CUSTOM_TARGET(html_c_api_doc
        COMMAND   ${DOXYGEN_EXECUTABLE} Doxyfile_c
        COMMENT   "C DOXYGEN"
    )
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html-c
            DESTINATION ${CMAKE_INSTALL_DOCDIR}/api
            COMPONENT html OPTIONAL)

    add_dependencies(html html_c_api_doc)

    #-------------------------------------------------------------------------
    if(ENABLE_CXX)
        ADD_CUSTOM_TARGET(html_cpp_api_doc
            COMMAND   ${DOXYGEN_EXECUTABLE} Doxyfile_cpp
            COMMENT   "CPP DOXYGEN"
        )
        
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html-cpp
                DESTINATION ${CMAKE_INSTALL_DOCDIR}/api
                COMPONENT html OPTIONAL)
        
        add_dependencies(html html_cpp_api_doc)
    endif()

#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    html-java latex-java
#        COMMAND   ${DOXYGEN_EXECUTABLE}
#        ARGS      ${PROJECT_SOURCE_DIR}/doc/doxygen/Doxyfile_java
#        DEPENDS   html-cpp latex-cpp
#        COMMENT   "Java DOXYGEN"
#    )
#
#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    html-python latex-python
#        COMMAND   ${DOXYGEN_EXECUTABLE}
#        ARGS      ${PROJECT_SOURCE_DIR}/doc/doxygen/Doxyfile_python
#        DEPENDS   html-java latex-java
#        COMMENT   "Python DOXYGEN"
#    )

    #-------------------------------------------------------------------------
    if(ENABLE_FORTRAN90)
        ADD_CUSTOM_TARGET(html_f90_api_doc
            COMMAND   ${DOXYGEN_EXECUTABLE} Doxyfile_f90
            COMMENT   "F90 DOXYGEN"
        )

        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html-f90
                DESTINATION ${CMAKE_INSTALL_DOCDIR}/api
                COMPONENT html OPTIONAL)
        
        add_dependencies(html html_f90_api_doc)
    endif()
   
    #-------------------------------------------------------------------------
    if(ENABLE_FORTRAN77)
        ADD_CUSTOM_TARGET(html_f77_api_doc
            COMMAND   ${DOXYGEN_EXECUTABLE} Doxyfile_f77
            COMMENT   "F77 DOXYGEN"
        )
    
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html-f77
                DESTINATION ${CMAKE_INSTALL_DOCDIR}/api
                COMPONENT html OPTIONAL)
        add_dependencies(html html_f77_api_doc)
    endif()

    add_custom_target(install-html
        ${CMAKE_COMMAND}
        -D "CMAKE_INSTALL_COMPONENT=html"
        -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake")
    add_dependencies(install-html html)

else()
    message(STATUS "Cannot find doxygen - no API documentation will be built!")
endif ()

