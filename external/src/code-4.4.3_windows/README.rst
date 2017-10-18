=========================
Installation Instructions
=========================

Requirements
============

In order to build the Nexus C API the following software has to be installed on
the build system 

* C compiler 
* CMake >= 2.8.8 
* HDF5 libraries and header files 
* Optional: HDF4 libraries and header files
* Optional: MXML libraries and header files
* doxygen: for building the API documentation
* rst2man: for building manpages 

Building the code on Linux
==========================

Enabling a physical file formats
--------------------------------

By default the NAPI is only built with HDF5 support. You can explicit switch on
a particular file format by defining the appropriate CMake variable during
configuration

* ``-DENABLE_HDF5=1`` for HDF5
* ``-DENABLE_HDF4=1`` for HDF4
* and ``-DENABLE_MXML=1`` for MXML

CMake tries then to figure out the locations of the required library binaries
and header files. This should work if the library provides a ``pkg-config`` file
and/or is installed in one of the systems default locations. 

If your library is not in a default location you have basically two options. 

1. if the library is installed with a ``pkg-config`` file you can add the path 
   to this file to your ``PKG_CONFIG_PATH`` environment variable. 
2. define CMake variables during configuration that point to the 
   appropriate location. 

For the second option the following CMake variables are available 

=================  ========================================
CMake variable     Content
=================  ========================================
HDF5_INCLUDE_DIRS  location of HDF5 header files
HDF5_LIBRARY_DIRS  location of HDF5 runtime libraries
HDF4_INCLUDE_DIRS  location of the HDF4 header files
HDF4_LIBRARY_DIRS  location of the HDF4 runtime libraries
MXML_INCLUDE_DIRS  location of the MXML header files
MXML_LIBRARY_DIRS  location of the MXML runtime libraries
=================  =======================================


Enable language bindings
------------------------

The library provides bindings for C++, Fortran 77, and Fortran 90. To enable
them set the following variables to one during code configuration

================ ===============================
CMAKE variable   language bindings
================ ===============================
ENABLE_CXX       build with C++ bindings
ENABLE_FORTRAN77 build with Fortran 77 bindings
ENABLE_FORTRAN90 build with Fortran 90 bindings
================ ===============================

Enable applications
-------------------

Aside with the C-library the NAPI source distribution ships a couple of command
line programs to work with NeXus files. These programs are not built by
default. In order to include them in the build the ``ENABLE_APPS`` variable
must be set to ``ON``. 

===============  =======================================================
Program          Description
===============  =======================================================
``nxbrowse``     browse a NeXus file
``nxdir``        list the contents of a NeXus file
``nxconvert``    convert a NeXus file to whatever?
``nxtraverse``   no idea what this is good for
``nxdump``      
``nxingest``
``nxsummary``
``nxtranslate``
===============  =======================================================

Building the distribution with these utility applications pulls in some
additional build requirements. These are

* ``libreadline``
* ``libtermcap``
* ``libhistory`` (most probably provided by the ``libreadline`` package)
* ``libxml2``

As one cannot select an individual program to be included in the build, all
these build dependencies must be satisified when ``ENABLE_APPS`` is set to
``ON`` in order for the build to succeed.

Running the build
-----------------

To build the library and program binaries simply use 

.. code-block:: bash 

    $ make 

and for installation 

.. code-block:: bash

    $ make install

This procedure installs the binaries, header files, and the man pages for the
programs (if configured to build them). To build the API documentation use 

.. code-block:: bash

    $ make html
    $ make install-html

Building the code on Windows
============================

These instructions will build everything except the applications using LIBXML2 (e.g. nxtranslate)

* Download and install Win32/x64 HDF4 and/or HDF5 libraries from HDF web site

  - within these installations is a "cmake" directory, I needed to remove/rename this to get everything to configure properly

* Download MXML source from http://www.msweet.org/projects.php?Z3

  - open the vcnet directory
  - you need to edit mxml1.def and a add a single line containing the text    mxml_error   to the end of this file
  - open the visual studio solution, chose either Win32 or x64 build type as appropriate, and build the project
  - copy the files   mxml.h mxml1.lib mxml1.dll   to some other location

* Define HDF4_ROOT, HDF5_ROOT and MXML_ROOT Windows environment variables to point to the top of each installed area

  - you should use / rather than \\ for the paths specified in these variables

* Create an empty build directory somewhere - this can be within your nexus source tree
* Run CMAKE-GUI to generate Visual studio solutions files

  - Provide source and build paths in the relevant boxes
  - Click on Configure, choose appropriate visual studio version (with Win64 suffix if needed) when prompted
  - A list of options will appear, select the  ENABLE_*  boxes for the components you need
  - Also check the CMAKE_INSTALL_PREFIX is appropriate, this is the root where file may be installed to later
  - press Configure again
  - If anything now appears in red, it means it is a newly displayed option. If you change ones of these (or any other value) you need to pres Configure again, otherwise move onto the next step 
  - Click on Generate 

* Browse to the build directory and open the generated NeXus.sln
* Build the solution

  - If you wish to copy files to the install directory specified above, right click and Build the INSTALL project 
  - If you wish to run the tests, right click and Build the RUN_TESTS project. Currently tests pick up the DLL installed above, this will be fixed later to point to the DLL in the build tree.
