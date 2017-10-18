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
#=============================================================================


FIND_PROGRAM(DOCBOOK2PDF_EXECUTABLE docbook2pdf)

FIND_PROGRAM(DOCBOOK2TXT_EXECUTABLE docbook2txt)

# handle the QUIETLY and REQUIRED arguments and set 
# PYTHONINTERP_FOUND to TRUE if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DocbookUtils DEFAULT_MSG 
                                  DOCBOOK2PDF_EXECUTABLE 
                                  DOCBOOK2TXT_EXECUTABLE)

MARK_AS_ADVANCED(DOCBOOK2PDF_EXECUTABLE DOCBOOK2TXT_EXECUTABLE)

