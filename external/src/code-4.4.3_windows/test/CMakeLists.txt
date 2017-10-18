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

set(NAPITESTSRC napi_test.c)

#------------------------------------------------------------------------------
# Need to copy the test data files 
#------------------------------------------------------------------------------
add_subdirectory(data)

include_directories(${PROJECT_SOURCE_DIR}/include )
link_directories(${HDF5_LIBRARY_DIRS} 
                 ${HDF4_LIBRARY_DIRS}
                 ${MXML_LIBRARY_DIRS})

if (WIN32)
  set(TESTSPATH "$ENV{HDF4_ROOT}/bin;$ENV{HDF5_ROOT}/bin;$ENV{MXML_ROOT};${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR};$ENV{PATH}")
  STRING(REPLACE "/" "\\" TESTSPATH "${TESTSPATH}")
  STRING(REPLACE "\\;" ";" TESTSPATH "${TESTSPATH}")
  STRING(REPLACE ";" "\\;" TESTSPATH "${TESTSPATH}") 
endif(WIN32)

#------------------------------------------------------------------------------
# In all cases we have to build the C-tests
#------------------------------------------------------------------------------
if(WITH_HDF5)
    add_executable(napi_test_hdf5 napi_test.c)
    target_link_libraries(napi_test_hdf5 NeXus_Shared_Library)
    add_test(NAME "NAPI-C-HDF5-test" COMMAND  napi_test_hdf5)
    
    add_executable(napi_attra_test_hdf5 napi_attra_test.c)
    target_link_libraries(napi_attra_test_hdf5 NeXus_Shared_Library)
    add_test(NAME "NAPI-C-HDF5-attra-test" COMMAND  napi_attra_test_hdf5)

    if (WIN32)
      set_property(TEST "NAPI-C-HDF5-test" "NAPI-C-HDF5-attra-test" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
    endif(WIN32)
endif()

if(WITH_HDF4)
    add_executable(napi_test_hdf4 napi_test.c)
    target_link_libraries(napi_test_hdf4 NeXus_Shared_Library)
    add_test(NAME "NAPI-C-HDF4-test"
             COMMAND  napi_test_hdf4)
             
    add_executable(napi_attra_test_hdf4 napi_attra_test.c)
    target_link_libraries(napi_attra_test_hdf4 NeXus_Shared_Library) 
    add_test(NAME "NAPI-C-HDF4-attra-test" COMMAND  napi_attra_test_hdf4)
    set_tests_properties("NAPI-C-HDF4-attra-test" PROPERTIES WILL_FAIL TRUE)

    if (WIN32)
      set_property(TEST "NAPI-C-HDF4-test" "NAPI-C-HDF4-attra-test" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
    endif(WIN32)
endif()

if(WITH_MXML)
    add_executable(napi_test-xml napi_test.c)
    target_link_libraries(napi_test-xml NeXus_Shared_Library)
    add_test(NAME "NAPI-C-MXML-test"
             COMMAND  napi_test-xml)
             
    add_executable(napi_test-xml-table napi_test.c)
    target_link_libraries(napi_test-xml-table NeXus_Shared_Library)
    add_test(NAME "NAPI-C-MXML-TABLE-test"
             COMMAND  napi_test-xml-table)
             
    add_executable(napi_attra_test_xml napi_attra_test.c)
    target_link_libraries(napi_attra_test_xml NeXus_Shared_Library)
    add_test(NAME "NAPI-C-MXML-attra-test" COMMAND napi_attra_test_xml)
    set_tests_properties("NAPI-C-MXML-attra-test" PROPERTIES WILL_FAIL TRUE)

    if (WIN32)
      set_property(TEST "NAPI-C-MXML-test" "NAPI-C-MXML-TABLE-test" "NAPI-C-MXML-attra-test" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
    endif(WIN32)
endif()

#------------------------------------------------------------------------------
# (most probably a) regression test 
#------------------------------------------------------------------------------
add_executable(leak_test1 leak_test1.c)
target_link_libraries(leak_test1 NeXus_Shared_Library)
add_test(NAME "NAPI-C-leak-test-1"
         COMMAND  leak_test1)
if (WIN32)
  set_property(TEST "NAPI-C-leak-test-1" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
endif(WIN32)

#------------------------------------------------------------------------------
# Add test for unlimited dimensions 
#------------------------------------------------------------------------------
add_executable(test_nxunlimited test_nxunlimited.c)
target_link_libraries(test_nxunlimited  NeXus_Shared_Library)
add_test(NAME "NAPI-C-test-nxunlimited"
         COMMAND  test_nxunlimited)
if (WIN32)
  set_property(TEST "NAPI-C-test-nxunlimited" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
endif(WIN32)
         

#------------------------------------------------------------------------------
# Add tests relevant to C++ bindings
#------------------------------------------------------------------------------
if(ENABLE_CXX)
    list(APPEND NAPITESTSRC napi_test_cpp.cxx)
    include_directories(../bindings/cpp)

    if(WITH_HDF5)
        add_executable(napi_test_cpp-hdf5 napi_test_cpp.cxx)
        target_link_libraries(napi_test_cpp-hdf5 NeXus_CPP_Shared_Library)
        add_test(NAME "NAPI-C++-HDF5-test" COMMAND  napi_test_cpp-hdf5)
        if (WIN32)
            set_property(TEST "NAPI-C++-HDF5-test" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
        endif(WIN32)
    endif()

    if(WITH_HDF4)
        add_executable(napi_test_cpp-hdf4 napi_test_cpp.cxx)
        target_link_libraries(napi_test_cpp-hdf4 NeXus_CPP_Shared_Library)
        add_test(NAME "NAPI-C++-HDF4-test" COMMAND  napi_test_cpp-hdf4)
        if (WIN32)
            set_property(TEST "NAPI-C++-HDF4-test" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
        endif(WIN32)
    endif()

    if(WITH_MXML)
        add_executable(napi_test_cpp-xml napi_test_cpp.cxx)
        target_link_libraries(napi_test_cpp-xml NeXus_CPP_Shared_Library)
        add_test(NAME "NAPI-C++-MXML-test" COMMAND  napi_test_cpp-xml)
        if (WIN32)
            set_property(TEST "NAPI-C++-MXML-test" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
        endif(WIN32)

#        add_executable(napi_test_cpp-xml-table napi_test_cpp.cxx)
#        target_link_libraries(napi_test_cpp-xml-table NeXus_CPP_Shared_Library
#                              ${NAPI_LINK_LIBS})
#        add_test(NAME "NAPI-C++-MXML-TABLE-test"
#                 COMMAND napi_test_cpp-xml-table)
        
    endif()

    #--------------------------------------------------------------------------
    # build leak tests
    #--------------------------------------------------------------------------
    add_executable(leak_test2 leak_test2.cxx)
    target_link_libraries(leak_test2 NeXus_CPP_Shared_Library)
    add_test(NAME "NAPI-C++-leak-test-2" 
             COMMAND  leak_test2)
    if (WIN32)
      set_property(TEST "NAPI-C++-leak-test-2" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
    endif(WIN32)

    add_executable(leak_test3 leak_test3.cxx)
    target_link_libraries(leak_test3 NeXus_CPP_Shared_Library)
    add_test(NAME "NAPI-C++-leak-test-3"
             COMMAND  leak_test3)
    if (WIN32)
      set_property(TEST "NAPI-C++-leak-test-3" APPEND PROPERTY ENVIRONMENT "PATH=${TESTSPATH}")
    endif(WIN32)
endif()

if(ENABLE_FORTRAN77)
    include_directories(${PROJECT_BINARY_DIR}/bindings/f77)
    include_directories(${PROJECT_SOURCE_DIR}/bindings/f77)
   
    if(WITH_HDF4)
        add_executable(napif4_test napif4_test.f)
        target_link_libraries(napif4_test NeXus_F77_Shared_Library)
        add_test(NAME "NAPI-F77-HDF4-test"
                 COMMAND  napif4_test)
    endif()
    
    if(WITH_HDF5)
        add_executable(napif5_test napif5_test.f)
        target_link_libraries(napif5_test NeXus_F77_Shared_Library)
        add_test(NAME "NAPI-F77-HDF5-test"
                 COMMAND  napif5_test)
    endif()

    if(WITH_MXML)
        add_executable(napif_test  napif_test.f)
        target_link_libraries(napif_test NeXus_F77_Shared_Library)
        add_test(NAME "NAPI-F77-XML-test"
                 COMMAND  napif_test)
    endif()

endif()

if(ENABLE_FORTRAN90)
    #need to add the directory with the .mod files to the include path
    include_directories(${PROJECT_BINARY_DIR}/bindings/f90)
    add_executable(NXtestf90 NXtest.f90)
    target_link_libraries(NXtestf90 NeXus_F90_Shared_Library)
    add_test(NAME "Fortran-90-test" COMMAND  NXtestf90)
endif()


