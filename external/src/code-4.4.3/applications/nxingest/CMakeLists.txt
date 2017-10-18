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
#
#=============================================================================

#-----------------------------------------------------------------------------
# build and link the program executable
#-----------------------------------------------------------------------------
add_executable (nxingest nxingest_main.cpp 
                         nxingest_utils.cpp
                         nxingest_debug.cpp 
                         nxingest_nexus.cpp
                         nxingest_parse.cpp 
                         nxingest_time.cpp
                         nxingest_debug.h 
                         nxingest.h 
                         nxingest_main.h
                         nxingest_nexus.h 
                         nxingest_parse.h
                         nxingest_time.h 
                         nxingest_utils.h)

include_directories(${MXML_INCLUDE_DIRS})
target_link_libraries(nxingest NeXus_Shared_Library)


#-----------------------------------------------------------------------------
# install the program binary and some documentation
#-----------------------------------------------------------------------------
install (TARGETS nxingest 
         DESTINATION ${CMAKE_INSTALL_BINDIR} 
         COMPONENT Runtime)
install (FILES nxingest.txt gpl.txt 
         DESTINATION ${CMAKE_INSTALL_DOCDIR}/programs/nxingest 
         COMPONENT Documentation)

