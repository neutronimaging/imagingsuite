## Process this file with cmake
#====================================================================
#  NeXus - Neutron & X-ray Common Data Format
#
#  CMakeLists for building the NeXus library and applications.
#
#  Copyright (C) 2010 Stephen Rankin
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
#  For further information, see <http://www.nexusformat.org>
#
#
#====================================================================

#--------------------------------------------------------------------
# checking for additional build dependencies pulled in by the 
# applications. 
#--------------------------------------------------------------------
#The LibXML 2 Libraries
find_package(LibXml2)

find_library(READLINE readline)
if(READLINE)
   set(HAVE_LIBREADLINE 1)
   set(READLINE_LINK "readline")
endif(READLINE)

find_library(TERMCAP termcap)
if(TERMCAP)
   set(TERMCAP_LINK "")
endif(TERMCAP)

find_library(HISTORY history)
if(HISTORY)
   set(HISTORY_LINK "")
endif(HISTORY)

find_library(M m)
if(M)
   set(M_LINK "-lm")
endif(M)

find_library(DL dl)
if(DL)
   set(DL_LINK "-ldl")
endif(DL)

# Recurse into the subdirectories.
include_directories("../include")

#-----------------------------------------------------------------------------
# proceed with building the programs
#-----------------------------------------------------------------------------

add_subdirectory (NXbrowse)
#add_subdirectory(c-nxvalidate)

if (ENABLE_CXX)
  add_subdirectory (NXdir)
  add_subdirectory (NXconvert)
  add_subdirectory (NXtraverse)
endif()

if (ENABLE_FORTRAN90 AND CMAKE_Fortran_COMPILER_WORKS)
    add_subdirectory (NXdump)
endif()


if(LIBXML2_FOUND)
    add_subdirectory (NXsummary)
    add_subdirectory (NXtranslate)
endif(LIBXML2_FOUND)

if (WITH_MXML)
    add_subdirectory (nxingest)
endif()

#-----------------------------------------------------------------------------
# programs built in the original distribution 
#
# currently not supported
#-----------------------------------------------------------------------------
#install (PROGRAMS nxdiff DESTINATION bin COMPONENT Runtime)


