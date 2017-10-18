/**
 * \if CAPI
 *  \mainpage NeXus API documentation
 * \endif
 * \if CPPAPI
 *  \mainpage NeXus C++ API
 *    \author Peter Peterson
 *    \author Freddie Akeroyd
 * \endif
 *
 * 2000-2008 NeXus Group
 * 

 * \if CAPI
 * \section sec_purpose Purpose of API
 *  The NeXus Application Program Interface is a suite of subroutines, written 
 *  in C but with wrappers in C++, JAVA, PYTHON, Fortran 77 and 90. 
 *  The subroutines call HDF routines to read and write the NeXus files with 
 *  the correct structure. 
 * \endif
 * \if CPPAPI
 * \section sec_purpose_cpp Purpose of API
 *  This provides a C++ like interface to the NeXus C API. It adds:
 *  \li Exceptions
 *  \li Suport for std::vector<type> and std::string
 *  \li Can directly obtain a container of entry/attribute details rather than 
 *      needing to iterate via getNext calls.
 * \endif
 * 
 * An API serves a number of useful purposes: 
 * \li It simplifies the reading and writing of NeXus files. 
 * \li It ensures a certain degree of compliance with the NeXus standard. 
 * \li It allows the development of sophisticated input/output features such 
 *     as automatic unit conversion. This has not been implemented yet. 
 * \li It hides the implementation details of the format. In particular, the 
 *     API can read and write HDF4, HDF5 (and shortly XML) files using the 
 *     same routines. 
 * For these reasons, we request that all NeXus files are written using the 
 * supplied API. We cannot be sure that anything written using the underlying 
 * HDF API will be recognized by NeXus-aware utilities. 
 *
 * \if CAPI
 * \section sec_core Core API
 * \endif
 * \if CPPAPI
 * \section sec_core_cpp Core API
 * \endif
 * The core API provides the basic routines for reading, writing and 
 * navigating NeXus files. It is designed to be modal; there is a hidden 
 * state that determines which groups and data sets are open at any given 
 * moment, and subsequent operations are implicitly performed on these 
 * entities. This cuts down the number of parameters to pass around in API 
 * calls, at the cost of forcing a certain pre-approved mode d'emploi. This 
 * mode d'emploi will be familiar to most: it is very similar to navigating a 
 * directory hierarchy; in our case, NeXus groups are the directories, which 
 * contain data sets and/or other directories. 
 *
 * \if CAPI
 *   The core API comprises several functional groups which are listed on the 
 *   \b Modules tab. 
 * 
 *   C programs that call the above routines should include the following 
 *   header file: 
 *     \code
 *       #include "napi.h"
 *     \endcode
 *   \sa napi_test.c
 * \endif
 * \if CPPAPI
 *   C++ programs that call the above routines should include the following 
 *   header file: 
 *     \code
 *       #include "NeXusFile.hpp"
 *     \endcode
 *   \sa napi_test_cpp.cxx
 * \endif
 */

/**
 * \if CAPI
 * \example napi_test.c
 *   This is the test program for the NeXus C API.
 *   It illustrates calling most functions to read and write a file.
 * \endif
 * \if CPPAPI
 * \example napi_test_cpp.cxx
 *   Test program for the NeXus C++ Interface.
 *   This illustrates calling most of the functions for reading and writing a 
 *   file
 * \endif 
 */
