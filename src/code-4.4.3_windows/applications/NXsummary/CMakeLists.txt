## Process this file with cmake
#=============================================================================
#  NeXus - Neutron & X-ray Common Data Format
#
#  CMakeLists for building the NeXus library and applications.
#
#  Copyright (C) 2011 Stephen Rankin
#            (C) 2015 Eugen Wintersberger <eugen.wintersberger@desy.de>
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

#-----------------------------------------------------------------------------
# build and link the program binary
#-----------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR}/third_party 
                    ${LIBXML2_INCLUDE_DIR})

add_executable (nxsummary data_util.cpp main.cpp string_util.cpp
                         preferences.cpp output.cpp data_util.hpp
                         nxsummary.hpp string_util.hpp
                         preferences.hpp output.hpp xml_util.hpp)

target_link_libraries(nxsummary NeXus_Shared_Library ${LIBXML2_LIBRARIES})

#-----------------------------------------------------------------------------
# install program binary and man-page
#-----------------------------------------------------------------------------
install(TARGETS nxsummary 
        DESTINATION ${CMAKE_INSTALL_BINDIR} 
        COMPONENT Runtime)
install(FILES nxsummary.1 
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 
        COMPONENT Documentation)

