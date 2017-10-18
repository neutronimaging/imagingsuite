Building NeXus with Visual Studio
---------------------------------

Setup
-----

You need to define several Windows environment variables
HDF4ROOT and HDF5ROOT to point to top of unpacked HDF distributions
e.g. 

    HDF4ROOT=c:\program files\hdf42r1 
    HDF5ROOT=C:\program Files\HDF5-164

Note that you do not need the HDF5 version with F90 support even to build the nexus F90 API
The above the the latest version of HDF as available from the NCSA site and
reaquire several external libraries: zlin 1.2.2, jpeg-6b and szip-2.0 (encoder enabled)
The location of these external libraries is isdicated by the following variables e.g.

    JPEGROOT=c:\program files\jpeg-6b
    ZLIBROOT=c:\program fiules\zlib122
    SZIPROOT=c:\program files\szip20

the HDF4 includes will be in %HDFROOT%\debug\include and %HDF5ROOT%\release\include and
similarly HDF5 includes in %HDF5ROOT%\debug\include and %HDF5ROOT%\release\include 

All the visual studio project files make use of these variables and so should not
depend on the location HDF files are installed in.

Currently ZLIB only works as a DLL so your programs will require zlib1.dll

Building
--------

Open nexus.dsw, and then select a project to build

nexus4, nexus5 and nexus45 - Static NeXus libraries with HDF4, HDF5, and both HDF4/HDF5 support
nx45dll                    - Dynamic Link Library (DLL) version of the NeXus library with HDF4 and HDF5 support
napi*_test                 - the various test programs 
nxbrowse                   - the nexus file browser with static libraru
nxbrowse_dll		   - NeXus file browser using nx45dll.dll


Building your own projects with static libraries
------------------------------------------------

To make use of the HDF4ROOT and HDF5ROOT variables from within Visual Studio,
you refer to them as $(HDF4ROOT) and $(HDF5ROOT). When building a 
program using the NeXus library you will need to do the following to the 
project settings:

* Add  HDF4  and/or  HDF5  to the C "preprocessor definitions"
* Add either  $(HDF4ROOT)\debug\include,$(HDF5ROOT)\debug\include  or   
     $(HDF4ROOT)\debug\include,$(HDF5ROOT)\release\include   to the C "additional include directories"
     depending on whether you are building a debug or release project
* For a C project add either   $(HDF4ROOT)\debug\lib,$(HDF5ROOT)\debug\lib   or
                      $(HDF4ROOT)\release\lib,$(HDF5ROOT)\release\lib
     to the "additional library path" in the "input" section of the "link" settings
* For a FORTRAN project add either  $(HDF4ROOT)\debug\lib,$(HDF5ROOT)\debug\lib   or
                      $(HDF4ROOT)\debug\lib,$(HDF5ROOT)\release\lib
     to the "additional library path" in the "input" section of the "link" settings
* Add "wsock32.lib" (for the _htons@4 symbols etc.) and "hdf5.lib"  to the list of
  library modules to load.
 
For a debug build only, you will have to add "libc.lib" to the list of library modules to ignore
on the link input tab.

Building your own projects with dynamic link libraries (DLL)
------------------------------------------------------------

For a working example, see the nxbrowse_dll project

When building a program using the DLL version of the NeXus library 
(NX45DLL.DLL) you will need to do the following to the project settings:

* Set your C code generation options to use "debug multithreaded DLL" or "multithreaded DLL" libraries
* set the FORTRAN libraries option to DLL
* Add  HDF4  and/or  HDF5  to the C "preprocessor definitions"
* Add either  $(HDF4ROOT)\include,$(HDF5ROOT)\c\debug\include  or   
     $(HDF4ROOT)\include,$(HDF5ROOT)\c\release\include   to the C "additional include directories"
     depending on whether you are building a debug or release project
* For a C project add either   $(HDF4ROOT)\dlllibdbg,$(HDF5ROOT)\c\debug\dll   or
                      $(HDF4ROOT)\dlllib,$(HDF5ROOT)\c\release\dll
     to the "additional library path" in the "input" section of the "link" settings
* For a FORTRAN project add either  $(HDF4ROOT)\dllibdbg,$(HDF5ROOT)\f90\debug\dll   or
                      $(HDF4ROOT)\dlllib,$(HDF5ROOT)\f90\release\dll
     to the "additional library path" in the "input" section of the "link" settings
* Add "wsock32.lib" (for the _htons@4 symbols etc.) and either "hdf5ddll.lib"  or  "hdf5dll.lib"
  (depending on debug/release configuration) to the list of library modules to load.
* Either add "nx45dll.lib" to the list of library modules to load in the link tab
  or make your project "depend" on the "nx45dll" project (in which case it links to nx45dll.lib automatically)

For a debug build only, you will have to add "msvcrt.lib" to the list of library 
modules to ignore on the link input tab.

When you run the program, you either need to have the DLL files in the same directory as the EXE file
or you need to add the directories containing the DLL files to the PATH environment variable.

Freddie Akeroyd (Freddie.Akeroyd@rl.ac.uk)
ISIS Facility
Rutherford Appleton Laboratory
GB

$Id$