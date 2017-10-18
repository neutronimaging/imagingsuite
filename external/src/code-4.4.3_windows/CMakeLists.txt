## Process this file with cmake
#=============================================================================
#  NeXus - Neutron & X-ray Common Data Format
#
#  CMakeLists for building the NeXus library and applications.
#
# Copyright (C) 2008-2012 NeXus International Advisory Committee (NIAC)
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
#==============================================================================

cmake_minimum_required(VERSION 2.8.7)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#The name of our project
project (NeXus)
enable_testing()

#set the module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_include")

#set some custom targets
add_custom_target(html)

#-----------------------------------------------------------------------------
# setup installation directories
#
# This is an OS specific task and we need to take care about the target 
# architecture. 
#-----------------------------------------------------------------------------
if(CMAKE_HOST_UNIX)
    #easy on Unix - just follow the GNU standard
    include(GNUInstallDirs)
    set(STATIC_LIBRARY_SUFFIX "")
    set(NEXUS_INSTALL_SHLIB ${CMAKE_INSTALL_LIBDIR})
else()
    set(CMAKE_INSTALL_LIBDIR lib)
    set(CMAKE_INSTALL_DOCDIR doc)
    set(CMAKE_INSTALL_INCLUDEDIR include)
    set(CMAKE_INSTALL_MANDIR man)
    set(CMAKE_INSTALL_BINDIR bin)
    set(STATIC_LIBRARY_SUFFIX "Static")
    set(NEXUS_INSTALL_SHLIB ${CMAKE_INSTALL_BINDIR})
endif() 


#-----------------------------------------------------------------------------
# define package version
#-----------------------------------------------------------------------------
set (API_VERSION_MAJOR 4)
set (API_VERSION_MINOR 4)
set (API_VERSION_PATCH 2)
set (API_VERSION "${API_VERSION_MAJOR}.${API_VERSION_MINOR}.${API_VERSION_PATCH}")
set (ABI_CURRENT 1)
set (ABI_REVISION 0)
set (ABI_AGE 0)
set (ABI_VERSION "${ABI_CURRENT}.${ABI_REVISION}.${ABI_AGE}")

set (NEXUS_VERSION_MAJOR 4)
set (NEXUS_VERSION_MINOR 4)
set (NEXUS_VERSION_PATCH 2)
set (NEXUS_SONAME 4)
set (NAPI_VERSION "${NEXUS_VERSION_MAJOR}.${NEXUS_VERSION_MINOR}.${NEXUS_VERSION_PATCH}")
set (NXLTVERSINFO "${NEXUS_VERSION_MAJOR}:${NEXUS_VERSION_MINOR}:${NEXUS_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# define command line options by which the user can configure the build
#-----------------------------------------------------------------------------
option(ENABLE_HDF5      "Build with HDF5 support" ON)
option(ENABLE_HDF4      "Build with HDF4 support" OFF)
option(ENABLE_MXML      "Build with XML support" OFF)
option(ENABLE_CONTRIB   "Build the user contributions" OFF)
option(ENABLE_FORTRAN90 "Build the Fortran 90 bindings" OFF)
option(ENABLE_FORTRAN77 "Build the Fortran 77 bindings" OFF)
option(ENABLE_JAVA      "Build Java bindings" OFF)
option(ENABLE_CXX       "Build C++ bindings" OFF)
option(ENABLE_APPS      "Build utility applications" OFF)
option(ENABLE_NXINTER   "Build Tcl-Swig Binding" OFF)

#show this only on Windows systems
if(CMAKE_HOST_WIN32)
    option(MINGW_MSYS   "We are building under MINGW." OFF)
    option(HAVE_MS_LIB  "We are building under MINGW." OFF)
endif()

#-----------------------------------------------------------------------------
# include some general purpose modules
#-----------------------------------------------------------------------------
include(Utilities)
include(CompilerChecks)
include(InstallRequiredSystemLibraries)

#-----------------------------------------------------------------------------
# check for package config
#-----------------------------------------------------------------------------
include(FindPkgConfig)

#include(cmake_include/FindCBFLib.cmake)
#include(cmake_include/FindJava.cmake)
#include(cmake_include/FindJNI.cmake)
#include(cmake_include/FindGuile.cmake)
#include(cmake_include/FindMZScheme.cmake)
#include(cmake_include/FindIDL.cmake)

#------------------------------------------------------------------------------
# a list of libraries we have to link NAPI against
# we need this in the global scope as all other programs have to be
# linked against this libraries as well - in this early stage there is
# no pkg-config we could use!
#------------------------------------------------------------------------------
set(NAPI_LINK_LIBS)

#------------------------------------------------------------------------------
# if requested by the user - check for MXMLlibraries
#------------------------------------------------------------------------------
if(ENABLE_MXML)
    include(FindMXML)
    # the required libraries are appended to
    # NAPI_LINK_LIBS by this module
    set(WITH_MXML TRUE)
else()
    #in all cases we build the libary without MXML
    message(STATUS "Build without MXML support!")
endif()

#------------------------------------------------------------------------------
# if requested by the user - check for HDF4 libraries
#------------------------------------------------------------------------------
if(ENABLE_HDF4)
    include(FindHDF4)

    message(STATUS "HDF4 library found in: ${HDF4_LIBRARY_DIRS}")
    message(STATUS "HDF4 headers found in: ${HDF4_INCLUDE_DIRS}")

    # the required libraries are appended to
    # NAPI_LINK_LIBS by this module
    set(WITH_HDF4 TRUE)
else()
    message(STATUS "Build without HDF4 support!")
endif()

#------------------------------------------------------------------------------
# if requested by the user - check for HDF5 libraries
#------------------------------------------------------------------------------
if(ENABLE_HDF5)

include(FindHDF5)

    message (STATUS "${HDF5_COMPONENTS}")
    if (ENABLE_CXX)
        find_package ( HDF5 COMPONENTS CXX HL NO_MODULE REQUIRED shared HINTS "C:/HDF5/*/") 
    else()
        find_package ( HDF5 NO_MODULE REQUIRED shared HINTS "C:/HDF5/*/")
    endif()
    
    find_package(ZLIB)

    # Hide annoying and confusing "HDF5_DIR-NOTFOUND" in CMake-GUI
    if (HDF5_DIR STREQUAL "HDF5_DIR-NOTFOUND")
      unset (HDF5_DIR CACHE)
     endif()

    set(HAVE_HDF5 1)
    list(APPEND NAPI_LINK_LIBS ${HDF5_LIBRARIES} ${ZLIB_LIBRARIES})
    set(WITH_HDF5 TRUE)

    message(STATUS "HDF5 library found in: ${HDF5_LIBRARY_DIRS}")
    message(STATUS "HDF5 headers found in: ${HDF5_INCLUDE_DIRS}")
else()
    message(STATUS "Build without HDF5 support!")
endif()

message(STATUS "Link with: ${NAPI_LINK_LIBS}")

#------------------------------------------------------------------------------
# enable FORTRAN bindings if requested by the user
#------------------------------------------------------------------------------
if(ENABLE_FORTRAN90 OR ENABLE_FORTRAN77)
    enable_language(Fortran)
    
    set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -Wall -fbacktrace -pedantic -fcheck=all -Wextra")
    message(STATUS ${CMAKE_Fortran_FLAGS})
endif()

#-----------------------------------------------------------------------------
# if contributed programs are built we have to add JPEG as a requirement 
# for nxextract
#-----------------------------------------------------------------------------
if(ENABLE_CONTRIB)
    find_package(JPEG REQUIRED)
endif()

#include(cmake_include/FindOpenGenie.cmake)
#include(cmake_include/FindSZIP.cmake)
#include(cmake_include/FindAnt.cmake)

if(ENABLE_NXINTER)
    find_package(SWIG REQUIRED)
    find_package(TCL REQUIRED)
endif()

#find_package(XMLRPC)
find_package(ZLIB)
find_package(LibXml2)
#find_package(LATEX)
#find_package(PythonInterp)

#Find the java runtime and sdk
#if(ENABLE_JAVA_BINDINGS)
#    find_package(Java 1.6)
#    find_package(JNI)
#endif()


find_library(PTHREAD pthread)
if(PTHREAD)
   set(PTHREAD_LINK "-lpthread")

   #this fixes an issue on OpenSuse 13.2 where the MXML library is not 
   #prelinked with threads
   if(WITH_MXML)
       list(APPEND NAPI_LINK_LIBS ${PTHREAD_LINK})
   endif()
endif(PTHREAD)

include_directories("${PROJECT_BINARY_DIR}/include"
                    "${PROJECT_SOURCE_DIR}/include")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${NX_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NX_CFLAGS}")

# Recurse into the subdirectories.
add_subdirectory (include)     #install header files
add_subdirectory (src)         #build the binaries
add_subdirectory (test)        #build the tests
add_subdirectory (bindings)    #build the language bindings
add_subdirectory (scripts)     #install pkg-config files

#on user request utility applications will be built
if(ENABLE_APPS)
    add_subdirectory (applications)
endif()

add_subdirectory (doc)
#add_subdirectory (config)

if(ENABLE_CONTRIB)
    add_subdirectory (contrib)
endif()

#add_subdirectory (InstallerBits)
#add_subdirectory (macosx_install_kit)
#add_subdirectory (scripts)
#add_subdirectory (third_party)
#add_subdirectory (windows)
#add_subdirectory (Windows_extra)

#
# set CPack packaging options
#

# needed for windows NSIS installer
if (CMAKE_SIZEOF_VOID_P MATCHES "8")
    set(ARCH64 1)
	set(ARCHSUFFIX "64")
else()
    set(ARCH64 0)
	set(ARCHSUFFIX "")
endif()

if(WIN32)
	file(GLOB HDF4_DLLS "${HDF4_LIBRARY_DIRS}/../bin/*.dll")
	# for some reason HDF5_LIBRARY_DIRS is blank
	file(GLOB HDF5_DLLS "${HDF5_INCLUDE_DIRS}/../bin/*.dll")
	file(GLOB_RECURSE MXML_DLLS "${MXML_LIBRARY_DIRS}/*.dll")
	install(FILES ${HDF4_DLLS} ${HDF5_DLLS} ${MXML_DLLS} DESTINATION bin COMPONENT Runtime)
	INSTALL(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT Runtime) 
endif()

file(TO_NATIVE_PATH ${PROJECT_SOURCE_DIR} PROJECT_SOURCE_DIR_NATIVE) 
file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR} PROJECT_BINARY_DIR_NATIVE) 
string(REPLACE "\\" "\\\\" PROJECT_SOURCE_DIR_NATIVE_D ${PROJECT_SOURCE_DIR_NATIVE})
string(REPLACE "\\" "\\\\" PROJECT_BINARY_DIR_NATIVE_D ${PROJECT_BINARY_DIR_NATIVE})

configure_file("${PROJECT_SOURCE_DIR}/CPackOptions.cmake.in" "${PROJECT_BINARY_DIR}/CPackOptions.cmake"  @ONLY)
set (CPACK_PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/CPackOptions.cmake")
set (CPACK_GENERATOR TGZ) # not use ZIP on UNIX as problem with symlinks
set (CPACK_SOURCE_GENERATOR TGZ) # not use ZIP on UNIX as problem with symlinks
if(WIN32)
    set (CPACK_GENERATOR ${CPACK_GENERATOR};ZIP;NSIS)
    set (CPACK_SOURCE_GENERATOR ${CPACK_SOURCE_GENERATOR};ZIP) 
elseif(APPLE)
    set (CPACK_GENERATOR ${CPACK_GENERATOR};PackageMaker)
elseif(CYGWIN)
    set (CPACK_GENERATOR ${CPACK_GENERATOR};CygwinBinary)
    set (CPACK_SOURCE_GENERATOR ${CPACK_SOURCE_GENERATOR};CygwinSource) 
elseif(UNIX)
    set (CPACK_GENERATOR ${CPACK_GENERATOR};DEB;RPM)
endif()
# Include of CPack must always be last
include(CPack)
