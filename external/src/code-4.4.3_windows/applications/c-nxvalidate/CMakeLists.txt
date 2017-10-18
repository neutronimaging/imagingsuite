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

add_subdirectory(include)
add_subdirectory(src)
include_directories(include)

link_directories(${HDF5_LIBRARY_DIRS} )

add_executable (nxvalidate nxvmain.c)

# This is an ugly hack to work around the fact that the default
# cmake HDF5 library search thing does not set this variable though
# says it does.
set(HDF5_HL_LIBRARIES "/usr/pkg/lib/libhdf5_hl.dylib")


target_link_libraries(nxvalidate NXV_Static_Library
  ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES} ${LIBXML2_LIBRARIES} )

install (TARGETS nxvalidate
         DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
#install (FILES nxvalidate.1
#         DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 COMPONENT Documentation)
