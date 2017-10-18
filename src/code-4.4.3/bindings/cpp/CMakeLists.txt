## Process this file with cmake
#==============================================================================
#  NeXus - Neutron & X-ray Common Data Format
#
#  CMakeLists for building the NeXus library and applications.
#
#  Copyright (C) 2011 Stephen Rankin
#
#  This library is free software; you can redistribute it and/or modify it under
#  the terms of the GNU Lesser General Public License as published by the Free
#  Software Foundation; either version 2 of the License, or (at your option) any
#  later version.
#
#  This library is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
#  details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this library; if not, write to the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#  For further information, see <http://www.nexusformat.org>
#
#
#==============================================================================

include(GenerateExportHeader)
#Make NeXus CPP Bindings Static Library

set (HEADERS NeXusFile.hpp NeXusException.hpp NeXusStream.hpp)
set (SOURCES NeXusFile.hpp NeXusFile.cpp NeXusException.hpp
             NeXusException.cpp NeXusStream.hpp NeXusStream.cpp)

set_property(SOURCE ${SOURCES} APPEND PROPERTY COMPILE_FLAGS ${NX_CFLAGS})

#Make NeXus CPP Bindings Shared Library
#------------------------------------------------------------------------------
add_library (NeXus_CPP_Shared_Library SHARED ${HEADERS} ${SOURCES})
target_link_libraries(NeXus_CPP_Shared_Library NeXus_Shared_Library)
target_include_directories(NeXus_CPP_Shared_Library PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_target_properties(NeXus_CPP_Shared_Library PROPERTIES
                      OUTPUT_NAME NeXusCPP
                      VERSION "${ABI_VERSION}"
                      SOVERSION ${ABI_CURRENT})

generate_export_header(NeXus_CPP_Shared_Library 
                       BASE_NAME NEXUS_CPP
                       EXPORT_FILE_NAME NeXusExport.hpp
                       EXPORT_MACRO_NAME NXDLL_EXPORT)

#------------------------------------------------------------------------------
add_library (NeXus_CPP_Static_Library STATIC ${HEADERS} ${SOURCES})
set_target_properties(NeXus_CPP_Static_Library PROPERTIES
                      OUTPUT_NAME NeXusCPP${STATIC_LIBRARY_SUFFIX}
                      COMPILE_FLAGS -DNEXUS_CPP_STATIC_DEFINE)
target_include_directories(NeXus_CPP_Static_Library PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
target_link_libraries(NeXus_CPP_Static_Library NeXus_Static_Library)


#------------------------------------------------------------------------------

install (TARGETS NeXus_CPP_Shared_Library
         RUNTIME DESTINATION ${NEXUS_INSTALL_SHLIB} COMPONENT Runtime
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development)

install (TARGETS NeXus_CPP_Static_Library
         DESTINATION ${CMAKE_INSTALL_LIBDIR}
         COMPONENT Development)

INSTALL (FILES ${HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/NeXusExport.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nexus
         COMPONENT Development)

