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

find_library(MZSCHEME_LIB NAMES mzscheme mzscheme3m PATHS $ENV{MZSCHEME_ROOT} $ENV{MZSCHEME_ROOT}/lib)

find_library(MZDYN_LIB NAMES mzdyn mzdyn3m PATHS $ENV{MZSCHEME_ROOT} $ENV{MZSCHEME_ROOT}/lib $ENV{MZSCHEME_ROOT}/lib/plt)

find_path(MZSCHEME_INCLUDE NAMES scheme.h mzconfig.h schemegc2.h PATHS $ENV{MZSCHEME_ROOT} $ENV{MZSCHEME_ROOT}/include)

if(MZSCHEME_LIB AND MZSCHEME_INCLUDE AND MZDYN_LIB)
    include_directories($ENV{MZSCHEME_INCLUDE})
endif(MZSCHEME_LIB AND MZSCHEME_INCLUDE AND MZDYN_LIB)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MZSCHEMELIB DEFAULT_MSG MZSCHEME_LIB MZSCHEME_INCLUDE MZDYN_LIB)

MARK_AS_ADVANCED(MZSCHEME_LIB MZSCHEME_INCLUDE)
