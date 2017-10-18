## Process this file with cmake
#====================================================================
#  NeXus - Neutron & X-ray Common Data Format
#  
#  CMakeLists for building the NeXus library and applications.
#
#  Copyright (C) 2011 Stephen Rankin
#  
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
# 
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free 
#  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
#  MA  02111-1307  USA
#             
#  For further information, see <http://www.neutron.anl.gov/NeXus/>
#
#
#====================================================================

if (SWIG_FOUND AND TCL_FOUND)

    set (SWIG_TCL_WRAP nxinter_wrap.c)
    set (SWIG_EXTRA   nxinterhelper.c)

    set (SOURCE ${SWIG_TCL_WRAP} ${SWIG_EXTRA}  nxdataset.i nxinter.i)

    
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${TCL_INCLUDE_PATH})
    add_definitions(-DIN_NEXUS_LIBRARY ${HDF5_DEFINITIONS} ${HDF5_CPP} ${HDF4_CPP} ${MXML_CPP})

    configure_file(nxinter.i nxinter.i COPYONLY)
    configure_file(nxdataset.i nxdataset.i COPYONLY)

    ADD_CUSTOM_COMMAND( 
        OUTPUT    ${SWIG_TCL_WRAP}
        COMMAND   ${SWIG_EXECUTABLE}
        ARGS      -I$(PROJECT_SOURCE_DIR)/bindings/swig -o ${SWIG_TCL_WRAP} -tcl -ltclsh.i nxinter.i
        COMMENT   "Generating ${SWIG_TCL_WRAP}"
    )

    ADD_CUSTOM_TARGET(NexusSwingTCLBuild ALL echo
        DEPENDS   ${SWIG_TCL_WRAP}
    )

    include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/bindings/swig")

    add_library (SWIG_TCL_Static_Library STATIC ${SWIG_TCL_WRAP} ${SWIG_EXTRA})

    set_target_properties(SWIG_TCL_Static_Library PROPERTIES OUTPUT_NAME libnxtcl)

    target_link_libraries(SWIG_TCL_Static_Library NeXus_Static_Library ${TCL_LIBRARY})

    add_library (SWIG_TCL_Shared_Library SHARED  ${SWIG_TCL_WRAP} ${SWIG_EXTRA} )

    set_target_properties(SWIG_TCL_Shared_Library PROPERTIES 
    						  OUTPUT_NAME nxinter VERSION ${ABI_VERSION})

    target_link_libraries(SWIG_TCL_Shared_Library NeXus_Shared_Library ${TCL_LIBRARY})

    add_executable (nxinter ${SWIG_TCL_WRAP} ${SWIG_EXTRA} ${TCL_INCLUDE_PATH})

    target_link_libraries(nxinter NeXus_Static_Library ${TCL_LIBRARY})

    install(TARGETS SWIG_TCL_Shared_Library 
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT RUNTIME)
    install(TARGETS nxinter 
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT RUNTIME)

endif (SWIG_FOUND AND TCL_FOUND)

#if (SWIG_FOUND AND GUILE_FOUND)
#
#    set (SWIG_GUILE_WRAP nxguile_wrap.c)
#
#    add_definitions(-DSWIGINIT="SCM scm_init_nxinter_module(void); scm_init_nxinter_module();")
#
#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    ${SWIG_GUILE_WRAP}
#        COMMAND   ${SWIG_EXECUTABLE}
#        ARGS      -I$(PROJECT_SOURCE_DIR)/bindings/swig -o ${SWIG_GUILE_WRAP} -guile -lguilemain.i nxinter.i
#        COMMENT   "Generating ${SWIG_GUILE_WRAP}"
#    )
#
#    ADD_CUSTOM_TARGET(NexusSwingGuileBuild ALL echo
#        DEPENDS   ${SWIG_GUILE_WRAP}
#    )
#
#    add_library (SWIG_GUILE_Static_Library STATIC ${SWIG_GUILE_WRAP})
#
#    set_target_properties(SWIG_GUILE_Static_Library PROPERTIES OUTPUT_NAME libnxguile)
#
#    target_link_libraries(SWIG_GUILE_Static_Library NeXus_Static_Library ${GUILE_LIB})
#
#    add_executable (nxguile ${SWIG_GUILE_WRAP} ${GUILE_INCLUDE})
#
#    target_link_libraries(nxguile NeXus_Static_Library ${GUILE_LIB})
#
#endif (SWIG_FOUND AND GUILE_FOUND)
#
#if (SWIG_FOUND AND MZScheme_FOUND)
#
#    set (SWIG_MZScheme_WRAP nxscheme_wrap.c)
#
#    add_definitions(-DSWIGINIT="SCM scm_init_nxinter_module(void); scm_init_nxinter_module();")
#
#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    ${SWIG_MZScheme_WRAP}
#        COMMAND   ${SWIG_EXECUTABLE}
#        ARGS      -I$(PROJECT_SOURCE_DIR)/bindings/swig -o ${SWIG_MZScheme_WRAP} -mzscheme nxinter.i
#        COMMENT   "Generating ${SWIG_MZScheme_WRAP}"
#    )
#
#    ADD_CUSTOM_TARGET(NexusSwingMZSchemeBuild ALL echo
#        DEPENDS   ${SWIG_MZScheme_WRAP}
#    )
#
#    add_library (SWIG_MZScheme_Static_Library STATIC ${SWIG_MZScheme_WRAP})
#
#    set_target_properties(SWIG_MZScheme_Static_Library PROPERTIES OUTPUT_NAME libnxscheme)
#
#    target_link_libraries(testmxml MXML_Static_Library ${PTHREAD_LINK})
#
#endif (SWIG_FOUND AND MZScheme_FOUND)
#
#set (DOC_SRC nxinter.tex)
#
#if (LATEX_FOUND)
#
#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    nxinter.aux
#        COMMAND   ${LATEX_COMPILER}
#        ARGS      ${DOC_SRC}
#        DEPENDS   nxinter.tex
#        COMMENT   "Generating SWIG INTER DVI"
#    )
#
#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    nxinter.dvi
#        COMMAND   ${LATEX_COMPILER}
#        ARGS      ${DOC_SRC}
#        DEPENDS   nxinter.aux
#        COMMENT   "Generating SWIG INTER DVI"
#    )
#
#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    nxinter.ps
#        COMMAND   ${DVIPS_CONVERTER}
#        ARGS      -o nxinter.ps nxinter.dvi
#        DEPENDS   nxinter.dvi
#        COMMENT   "Generating SWIG INTER PS"
#    )
#
#    ADD_CUSTOM_COMMAND( 
#        OUTPUT    nxinter.pdf
#        COMMAND   ${PS2PDF_CONVERTER}
#        ARGS      nxinter.ps nxinter.pdf
#        DEPENDS   nxinter.ps
#        COMMENT   "Generating SWIG INTER PDF"
#    )
#
#    ADD_CUSTOM_TARGET(NexusSwingDOCBuild ALL echo
#        DEPENDS   nxinter.pdf
#    )
#
#endif (LATEX_FOUND)
