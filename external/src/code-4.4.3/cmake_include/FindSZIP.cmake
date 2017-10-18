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
#  For further information, see <http://www.nexusformat.org>
#
#
#====================================================================

# looks in SZIP_ROOT environment variable for hint
# set SZIP_FOUND SZIP_DEFINITIONS SZIP_INCLUDE_DIRS  SZIP_LIBRARIES  SZIP_ROOT_DIR

if (WIN32)
    set(SZIP_SEARCH_DEFAULT $ENV{HDF5_ROOT})
	find_library(SZIP_LIBRARIES NAMES libszip szip HINTS ${SZIP_SEARCH} ENV SZIP_ROOT PATHS ${SZIP_SEARCH_DEFAULT} PATH_SUFFIXES bin dll lib DOC "location of szip lib" NO_SYSTEM_ENVIRONMENT_PATH)
else(WIN32)
    set(SZIP_SEARCH_DEFAULT $ENV{HDF5_ROOT} "/usr" "/usr/local" "/usr/local/hdf5" "/sw")
	find_library(SZIP_LIBRARIES NAMES sz HINTS ${SZIP_SEARCH} ENV SZIP_ROOT PATHS ${SZIP_SEARCH_DEFAULT} PATH_SUFFIXES lib DOC "location of szip lib")
endif(WIN32)

find_path(SZIP_INCLUDE_DIRS NAMES sz.h szlib.h HINTS ${SZIP_SEARCH} ENV SZIP_ROOT PATHS ${SZIP_SEARCH_DEFAULT} PATH_SUFFIXES include DOC "location of szip includes" NO_SYSTEM_ENVIRONMENT_PATH)

#if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    find_library(ZIP_LIB NAMES z zlib zdll zlib1 zlibd zlibd1 PATHS $ENV{HDF5_ROOT}/bin $ENV{HDF5_ROOT}/lib)
#    find_path(ZIP_INCLUDE zlib.h PATHS $ENV{HDF5_ROOT}/include)
#endif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

#if(SZIP_INCLUDE)
#    include_directories(${SZIP_INCLUDE})
#endif(SZIP_INCLUDE)

#if(ZIP_INCLUDE)
#    include_directories(${ZIP_INCLUDE})
#endif(ZIP_INCLUDE)

set(SZIP_DEFINITIONS "")
get_filename_component(_SZIP_LIBDIR ${SZIP_LIBRARIES} PATH)
get_filename_component(SZIP_ROOT_DIR "${_SZIP_LIBDIR}/.." ABSOLUTE)

mark_as_advanced(SZIP_LIBRARIES SZIP_INCLUDE_DIRS)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SZIP DEFAULT_MSG SZIP_LIBRARIES SZIP_INCLUDE_DIRS)

