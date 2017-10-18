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

#In 2.8 findJava and findJNI there is a lack of
#definition for javah and javadoc 
FIND_PROGRAM(JAVA_NATIVE
  NAMES javah
  PATHS ${JAVA_BIN_PATH}
)

FIND_PROGRAM(JAVA_DOC
  NAMES javadoc
  PATHS ${JAVA_BIN_PATH}
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JAVAEXTRA DEFAULT_MSG JAVA_NATIVE JAVA_DOC)

MARK_AS_ADVANCED(JAVA_NATIVE JAVA_DOC)
