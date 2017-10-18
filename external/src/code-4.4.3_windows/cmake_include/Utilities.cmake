## Process this file with cmake
#=============================================================================
#  NeXus - Neutron & X-ray Common Data Format
#
#  CMakeLists for building the NeXus library and applications.
#
#  Copyright (C) 2011 Freddie Akeroyd
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
include(CMakeParseArguments)

#-----------------------------------------------------------------------------
# This module provides some utility functions that are used throughout the
# project for configuration
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# checks a list of possible compiler flags and adds allowed ones 
# to the current list
#
function(check_add_c_compiler_flags)
    foreach(FLAG ${ARGV})
        check_c_compiler_flag(${FLAG} RES)
	    if (RES)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}" PARENT_SCOPE)
	    endif()
	endforeach()
endfunction()

#------------------------------------------------------------------------------
# checks a list of possible compiler flags and adds allowed ones to the current
# list
#
function(check_add_cxx_compiler_flags)
    foreach(FLAG ${ARGV})
        check_cxx_compiler_flag(${FLAG} RES)
	    if (RES)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}" PARENT_SCOPE)
	    endif()
	endforeach()
endfunction()

#------------------------------------------------------------------------------
# define a HAVE_  if both  BUILD_  and  _FOUND  are defined e.g. creates
# HAVE_HDF5 if both BUILD_HDF5 and HDF5_FOUND are ture
#
function(create_have_vars)
    foreach(NAME ${ARGV})
        if(${BUILD_${NAME}} AND ${${NAME}_FOUND})
	        set(HAVE_${NAME} ON PARENT_SCOPE)
	    else()
	        set(HAVE_${NAME} OFF PARENT_SCOPE)
	    endif()
	endforeach()
endfunction()

#-----------------------------------------------------------------------------
# Still need to check whether or not this function is required. 
#
function(install_pdb target)
#	set (OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
	set (OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/Release)
	get_target_property (OUT_NAME ${target} OUTPUT_NAME)
	get_filename_component (OUT_BASE_NAME ${OUT_NAME} NAME_WE)
#	set(PDB_FILE ${OUT_DIR}/${OUT_BASE_NAME}${CMAKE_DEBUG_POSTFIX}.pdb)
	set(PDB_FILE ${OUT_DIR}/${OUT_BASE_NAME}.pdb)
	install (FILES ${PDB_FILE} DESTINATION bin COMPONENT Runtime)  
endfunction()

#-----------------------------------------------------------------------------
function(search_module_library VAR)
    set(multiValueArgs SEARCH_PATH)
    set(oneValueArgs)
    cmake_parse_arguments(find_module_library "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(${VAR}_LIBRARY_DIRS)
        #if the user has provided a path we use this one 
        find_library(LIBFILES NAME ${LIB_NAMES} PATHS ${${VAR}_LIBRARY_DIRS} NO_DEFAULT_PATH)
    else()
        #if the user has not provided a path we look in the 
        #system defaults
        find_library(LIBFILES NAME ${LIB_NAMES} PATHS)
        get_filename_component(${VAR}_LIBRARY_DIRS ${LIBFILES} PATH)
    endif()

    if(${LIBFILES} MATCHES "LIBFILES-NOTFOUND")
        message(STATUS "Could not find ${VAR} runtime binaries")
        set(HAVE_${VAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

#-----------------------------------------------------------------------------
function(search_module_header VAR)

endfunction()

#-----------------------------------------------------------------------------
#
# Looks for a particular module 
#
# The function has three strategies to look for header and library files
# 1.) Check for user provided ${VAR}_INCLUDE_DIRS and ${VAR}_LIBRARY_DIRS
#     If the use has provided a location we look there for the libraries. 
#     The search is exclusive. Only the user provided directories are 
#     searched. This is to avoid conflicts with other version of a 
#     particular library which might be installed in the default locations.
# 2.) If the user has not provided any location we firs try pkg-config 
#     to look for the module. The name of the module is passed via the 
#     MOD_NAME keywoard argument.
# 3.) If no  pkg-config module is available, the function looks in the 
#     default search paths. It can use the environment variable 
#     ${MOD_NAME}_ROOT as a path hint.
#
# Arguments:
#   Positional arguments: 
#   VAR ................. prefix used for all output variables
# Results:
#   HAVE_{VAR} .......... true if the module was found (false otherwise)
#   ${VAR}_INCLUDE_DIRS . holding the header directory
#   ${VAR}_LIBRARY_DIRS . directory with runtime libraries
#   
# 
# 
function(find_module VAR )
    set(oneValueArgs MOD_NAME)
    set(multiValueArgs LIB_NAMES HEADER_NAMES)
    cmake_parse_arguments(find_module "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(LIB_NAMES  ${find_module_LIB_NAMES})
    set(HEADER_NAMES ${find_module_HEADER_NAMES})
    set(MOD_NAME ${find_module_MOD_NAME})
    
    set(HAVE_${VAR} TRUE PARENT_SCOPE)

    if(${VAR}_INCLUDE_DIRS OR ${VAR}_LIBRARY_DIRS)
        #if the user has provided the search path we have to do nothing but check 
        #wether or not the library files exist 
        if(${VAR}_LIBRARY_DIRS)
            #if the user has provided a path we use this one 
            find_library(${VAR}_LIBFILES NAME ${LIB_NAMES} 
                         PATHS ${${VAR}_LIBRARY_DIRS} NO_DEFAULT_PATH)
        else()
            #if the user has not provided a path we look in the 
            #system defaults
            find_library(${VAR}_LIBFILES NAME ${LIB_NAMES} PATHS)
            get_filename_component(${VAR}_LIBRARY_DIRS ${${VAR}_LIBFILES} PATH)
        endif()

        if(${${VAR}_LIBFILES} MATCHES "${VAR}_LIBFILES-NOTFOUND")
            message(STATUS "Could not find ${VAR} runtime binaries")
            set(HAVE_${VAR} FALSE PARENT_SCOPE)
        endif()

        #-------------------------------------------------------------------------
        # search for the header file
        #-------------------------------------------------------------------------
        if(${VAR}_INCLUDE_DIRS)
            #just check if the user provided path contains the header file
            find_file(${VAR}_HDRFILES NAME ${HEADER_NAMES} 
                      PATHS ${${VAR}_INCLUDE_DIRS} NO_DEFAULT_PATH)
        else()
            find_file(${VAR}_HDRFILES NAME ${HEADER_NAMES} PATHS)
            get_filename_component(${VAR}_INCLUDE_DIRS ${${VAR}_HDRFILES} PATH)
        endif()
        
        if(${${VAR}_HDRFILES} MATCHES "${VAR}_HDRFILES-NOTFOUND")
            message(STATUS "Could not find ${VAR} header files!")
            set(HAVE_${VAR} FALSE PARENT_SCOPE)
        endif()

    else()
        #if the user has not provided any configuration we have to do this manually
        if(PKG_CONFIG_FOUND AND MOD_NAME)
            #the easy way  - we use package config
            message(STATUS "pkg-config is searching for : ${MOD_NAME}")
            pkg_search_module(${VAR} ${MOD_NAME})
        endif()

        #if pkg-config was not successful we have to do this the hard way
        if(NOT ${VAR}_FOUND)
            find_library(${VAR}_LIBFILES NAME ${LIB_NAMES} PATHS ENV ${MOD_NAME}_ROOT)
            if(${${VAR}_LIBFILES} MATCHES "${VAR}_LIBFILES-NOTFOUND")
                set(STATUS "Could not find ${VAR} runtime binaries!")
                set(HAVE_${VAR} FALSE PARENT_SCOPE)
            else()
                get_filename_component(${VAR}_LIBRARY_DIRS 
                    ${${VAR}_LIBFILES} PATH) 
                message(STATUS "${VAR} libraries: ${${VAR}_LIBFILES}")
            endif()

            find_file(${VAR}_HDRFILES NAME ${HEADER_NAMES} PATHS ENV ${MOD_NAME}_ROOT)
            if(${${VAR}_HDRFILES} MATCHES "${VAR}_HDRFILES-NOTFOUND")
                message(STATUS "Could not find ${VAR} header files!")
                set(HAVE_${VAR} FALSE PARENT_SCOPE)
            else()
                get_filename_component(${VAR}_INCLUDE_DIRS ${${VAR}_HDRFILES} PATH)
                message(STATUS "${VAR} headers: ${${VAR}_HDRFILES}")
            endif()
            
        endif()

    endif()
    set(${VAR}_INCLUDE_DIRS ${${VAR}_INCLUDE_DIRS} PARENT_SCOPE)
    set(${VAR}_LIBRARY_DIRS ${${VAR}_LIBRARY_DIRS} PARENT_SCOPE)
	set(${VAR}_LIBFILES ${${VAR}_LIBFILES} PARENT_SCOPE)

endfunction()
