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
#=============================================================================

add_definitions(-DIN_NEXUS_LIBRARY=1 ${NX_CPP})

# this means a static nexus library will use a DLL HDF5. OK for now,
# but may want to change later. 
if (WIN32)
add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB=1)
endif (WIN32)

configure_file(nexus_symbols.txt nexus_symbols.sym COPYONLY)
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--version-script=nexus_symbols.sym")

#-----------------------------------------------------------------------------
# generate list of common source files
#-----------------------------------------------------------------------------
set (NAPISRC napi.c napiu.c nxstack.c nxstack.h stptok.c  nxdataset.c 
             napi_fortran_helper.c
             nxdataset.h nx_stptok.h)

set (NAPILINK)

#-----------------------------------------------------------------------------
# add HDF4 specific source files
#-----------------------------------------------------------------------------
if (WITH_HDF4)
  set (NAPISRC ${NAPISRC} napi4.c)
  include_directories(${HDF4_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR})
  link_directories(${HDF4_LIBRARY_DIRS})
endif(WITH_HDF4)

#-----------------------------------------------------------------------------
# add HDF5 specific source files
#-----------------------------------------------------------------------------
if (WITH_HDF5)
  set (NAPISRC ${NAPISRC} napi5.c)
  add_definitions(-DH5_USE_18_API=1)
  include_directories(${HDF5_INCLUDE_DIRS})
  link_directories(${HDF5_LIBRARY_DIRS})
endif (WITH_HDF5)

#-----------------------------------------------------------------------------
# add XML specific source files
#-----------------------------------------------------------------------------
if(WITH_MXML)
    set (NAPISRC ${NAPISRC} nxxml.c nxio.c nxio.h)
	include_directories(${MXML_INCLUDE_DIRS})
    link_directories(${MXML_LIBRARY_DIRS})
endif(WITH_MXML)

set_property(SOURCE ${NAPISRC} APPEND PROPERTY COMPILE_FLAGS ${NX_CFLAGS})

#------------------------------------------------------------------------------
# building the static library
#------------------------------------------------------------------------------
add_library (NeXus_Static_Library STATIC ${NAPISRC})

set_target_properties(NeXus_Static_Library PROPERTIES 
                      OUTPUT_NAME NeXus${STATIC_LIBRARY_SUFFIX})

target_link_libraries(NeXus_Static_Library ${NAPI_LINK_LIBS})
#------------------------------------------------------------------------------
# building the shared library
#------------------------------------------------------------------------------
add_library (NeXus_Shared_Library SHARED ${NAPISRC})

set_target_properties(NeXus_Shared_Library PROPERTIES 
                      OUTPUT_NAME NeXus
                      VERSION ${ABI_VERSION}
                      SOVERSION ${ABI_CURRENT})
if(MSVC)
set_target_properties(NeXus_Shared_Library PROPERTIES LINK_FLAGS 
                           "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/NeXus.def\"")
endif(MSVC)

target_link_libraries(NeXus_Shared_Library ${NAPI_LINK_LIBS})


#------------------------------------------------------------------------------
# install the targets
#-----------------------------------------------------------------------------
install (TARGETS NeXus_Shared_Library
         RUNTIME DESTINATION ${NEXUS_INSTALL_SHLIB} COMPONENT Runtime
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development)

install (TARGETS NeXus_Static_Library 
         DESTINATION ${CMAKE_INSTALL_LIBDIR}
         COMPONENT Development)

