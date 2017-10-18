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

#Make NeXus F90 Bindings Static Library

set (SOURCES NXmodule.f90 NXUmodule.f90)

#------------------------------------------------------------------------------
# build static library for Fortran90 bindings
#------------------------------------------------------------------------------

add_library (NeXus_F90_Static_Library STATIC ${SOURCES})
set_target_properties(NeXus_F90_Static_Library PROPERTIES OUTPUT_NAME NeXus90${STATIC_LIBRARY_SUFFIX})
target_link_libraries(NeXus_F90_Static_Library NeXus_Static_Library)

#------------------------------------------------------------------------------
# build shared library for Fortran90 bindings
#------------------------------------------------------------------------------

add_library (NeXus_F90_Shared_Library SHARED ${SOURCES})

#Note - library version needs to be got from somewhere?
set_target_properties(NeXus_F90_Shared_Library PROPERTIES 
                      OUTPUT_NAME NeXus90
                      VERSION "${ABI_VERSION}"
                      SOVERSION ${ABI_CURRENT})

target_link_libraries(NeXus_F90_Shared_Library NeXus_Shared_Library )

install (TARGETS NeXus_F90_Shared_Library
         RUNTIME DESTINATION ${NEXUS_INSTALL_SHLIB} COMPONENT Runtime
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development)

install (TARGETS NeXus_F90_Static_Library 
         DESTINATION ${CMAKE_INSTALL_LIBDIR}/nexus
         COMPONENT Development)
