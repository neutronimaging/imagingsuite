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
#
#==============================================================================


#------------------------------------------------------------------------------
# find the runtime binaries of the MXML library
#------------------------------------------------------------------------------
find_library(MXML_LIBRARY NAMES mxml mxml1
             HINTS ENV MXML_ROOT)
             
if(MXML_LIBRARY MATCHES MXML_LIBRARY-NOTFOUND)
    message(FATAL_ERROR "Could not find MXML library!")
else()
    get_filename_component(MXML_LIBRARY_DIRS ${MXML_LIBRARY} PATH)
    message(STATUS "Found MXML library: ${MXML_LIBRARY}")
    message(STATUS "MXML libary path: ${MXML_LIBRARY_DIRS}")
endif()

#------------------------------------------------------------------------------
# find the MXML header file
#------------------------------------------------------------------------------
find_path(MXML_INCLUDE_DIRS mxml.h)
         
if(MXML_INCLUDE_DIRS MATCHES MXML_INCLUDE_DIRS-NOTFOUND)
    message(FATAL_ERROR "Could not find MXML header files")
else()
    message(STATUS "Found MXML header files in: ${MXML_INCLUDE_DIRS}")
endif()

include_directories(${MXML_INCLUDE_DIRS})
list(APPEND NAPI_LINK_LIBS ${MXML_LIBRARY})
