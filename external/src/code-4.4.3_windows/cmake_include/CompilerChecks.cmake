## Process this file with cmake
#==============================================================================
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


include(CheckCCompilerFlag)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)

#------------------------------------------------------------------------------
# need this only in the case of C++ bindings
#------------------------------------------------------------------------------
if(ENABLE_CXX)
    include(CheckCXXCompilerFlag)
endif(ENABLE_CXX)

#------------------------------------------------------------------------------
# Check type sizes 
#------------------------------------------------------------------------------
CHECK_TYPE_SIZE("int" SIZEOF_INT)
set(HAVE_INT ${HAVE_SIZEOF_INT})
CHECK_TYPE_SIZE("long int" SIZEOF_LONG_INT)
set(HAVE_LONG_INT ${HAVE_SIZEOF_LONG_INT})
CHECK_TYPE_SIZE("long long int" SIZEOF_LONG_LONG_INT)
set(HAVE_LONG_LONG_INT ${HAVE_SIZEOF_LONG_LONG_INT})
CHECK_TYPE_SIZE("unsigned long long int" SIZEOF_UNSIGNED_LONG_LONG_INT)
set(HAVE_UNSIGNED_LONG_LONG_INT ${HAVE_SIZEOF_UNSIGNED_LONG_LONG_INT})
CHECK_TYPE_SIZE("size_t" SIZEOF_SIZE_T)
set(HAVE_SIZE_T ${HAVE_SIZEOF_SIZE_T})

#------------------------------------------------------------------------------
# Check for required functions
#------------------------------------------------------------------------------
CHECK_FUNCTION_EXISTS(ftime HAVE_FTIME)
CHECK_FUNCTION_EXISTS(tzset HAVE_TZSET)
CHECK_FUNCTION_EXISTS(strdup HAVE_STRDUP)

#------------------------------------------------------------------------------
# Check for required header files
#------------------------------------------------------------------------------
CHECK_INCLUDE_FILE(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE(dlfcn.h HAVE_DLFCN_H)

if (SIZEOF_LONG_LONG_INT EQUAL 8)
	set(PRINTF_INT64 "lld")
	set(PRINTF_UINT64 "llu")
else()
	message(SEND_ERROR "Cannot printf int64 and uint64")
endif()

#------------------------------------------------------------------------------
# Check and add compiler flags on Unix systems
#------------------------------------------------------------------------------
if (UNIX)
    check_add_c_compiler_flags("-Wall -Wno-unused-variable -Wno-sign-compare "
                               "-Wno-comment")

    if(ENABLE_CXX)
        check_add_cxx_compiler_flags("-Wall -Wno-unused-variable "
                                     "-Wno-sign-compare -Wno-comment")
    endif(ENABLE_CXX)
        
endif(UNIX)
