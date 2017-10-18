Version 4.3.0
=============

For the latest information see 

    http://wiki.nexusformat.org/Nexus_43_Release_Notes


New Features
============

* Links to external files via the NeXus external linking mechanism have now been enhanced to take advantage of native HDF5 external linking. Previously a nexus external file link was only visible to NeXus aware programs, and this will continue to be the case for XML and HDF4 based files. In the case of files created with the HDF5 underlying format, external file links will now be visible to any HDF5 (1.8.*) aware program. 

* HDF5 based files can now have multiple "unlimited" dimensions (previously only one was allowed) 

* New API functions have been added to handle very large arrays. Most original NeXus functions had array dimensions of type "int" which restricted the maximum size of an array. New functions with a "64" suffix have been added which use int64_t rather than int - existing functions continue to work as normal, so there is no need to update code unless you want to make use of the larger dimensions. 

* A new python tree API has been added

* A GUI java based NXvalidate program has now been added 

* The NeXus API now ensures thread safety, even if the underlying HDF/XML library is not built that way. The current approach would not allow any concurrency in writing, but HDF5 does not support this anyway at the moment. 

* A new function NXreopen() has been added which will create additional NXhandle objects from an existing NXhandle, allowing you to have several NXhandle structures referring to the same file. This can give a large performance gain if you need to write to different parts of a file as separate threads can be created with their own NXhandles, thus removing the need to open and close data groups that can lead to unnecessary flushing to disk etc. 

* New application NXtraverse added 

Changed Features
================

* If creating an external link you are no longer supposed to create the group beforehand. As HDF5 can handle groups natively, that is counterproductive and we would have to remove the empty group again (hoping that it is actually empty that is). It is easier to create the group in the API if required.
* The HDF5 1.6.* series libraries are no longer supported - NeXus now requires 1.8.* or higher. The 1.6.* series is now very old and moving to 1.8.* has allowed us to make use of new and improved features, such as native external file linking (see above) 

System Requirements
===================

* MXML XML Parsing Library: Version 2.2.2 or higher of mxml is required. Earlier versions have a bug and the XML API will not work. This package can be downloded in both source and binary rpm form and is also available as part of Fedora Extras. IMPORTANT NOTE: Debian also provides the mxml package, but it based on 2.0 and will not work properly. 

* Python Interface: You will need both the numpy and ctypes modules to be available. These are provided in both the Fedora and EPEL repositories. 

* HDF5 Version: Only the HDF5-1.8.* series (and above) is now supported. 

Version 4.2.0
=============

For the latest information see 

    http://wiki.nexusformat.org/Nexus_42_Release_Notes

==System Requirements==
'''MXML XML Parsing Library'''

Version 2.2.2 or higher of mxml is required. Earlier versions have a bug and the XML API will not work. This package can be downloded in [http://www.easysw.com/~mike/mxml/software.php both source and binary rpm form] and is also available as part of [http://fedoraproject.org/wiki/Extras/UsingExtras Fedora Extras]. IMPORTANT NOTE: Debian also provides the mxml package, but it based on 2.0 and will not work properly.

'''Python Interface'''
You will need both the numpy and ctypes modules to be available.  These are provided in both the Fedora and EPEL repositories.

==Building Notes==
===NAG F90/F95 Compiler===
The NAG compiler needs the '''-mismatch''' flag to be specified or else it will not compile NXmodule.f90 This is achieve by running configure with the '''FCFLAGS''' environment variable set to contain the flag e.g.
<pre>
env FCFLAGS="-mismatch" ./configure --with-f90=f95
</pre>
===HDF4 on Intel Macs===
There is a problem with the include file, hdfi.h (normally in /usr/local/include).  See http://coastwatch.noaa.gov/helparc/software/msg00069.html for details of the modifications necessary to fix it.

==New Features==
===C++ Interface===
See the [http://download.nexusformat.org/doxygen/html/classNeXus_1_1File.html doxygen documentation] and
[http://svn.nexusformat.org/code/branches/4.2/test/napi_test_cpp.cxx NeXus API test program]

===C++ Stream Like interface===
The idea is to provide an IOSteam like interface and allow you to type 
<pre>
    // create an entry and a data item
    File nf(fname, NXACC_CREATE);
    nf << Group("entry1", "NXentry") << Data("dat1", w, "int_attr", 3);
    nf.close();

    File nf1(fname, NXACC_RDWR);
    // add a double_attr to an existing setup
    nf1 >> Group("entry1", "NXentry") >> Data("dat1") << Attr("double_attr", 6.0);
    nf1.close();

    // read back data items
    File nf2(fname, NXACC_READ);
    nf2 >> Group("entry1", "NXentry") >> Data("dat1", w1, "int_attr", i, "double_attr", d);
    // alternative way to read d1
    nf2 >> Data("dat1") >> Attr("double_attr", d1);
</pre>
See also the [http://svn.nexusformat.org/code/branches/4.2/test/napi_test_cpp.cxx NeXus API test program]

===IDL Interface===
There is a new interface to RSI's Interactive Data Language, IDL for NeXus. This 
interface has to be considered beta. Nevertheless it is working most of the time. 
Known issues include:
* Compressed reading and writing do not work for HDF-4 files, probably because of a library version conflict on libz. 
* There is an issue using NXgetslab on 64 bit operating systems; expect a fix for this pretty soon.

===Python Interface===
There is now, thanks to Paul Kienzle, a supported interface for the python scripting language. Arrays are stored in numpy arrays and thus allow for efficient data manipulations.

==Changed Features==

==Known Issues==
See the comments on the IDL interface.

==Miscellaneous bug fixes==
The following items are bugs reported in previous releases and resolved in
the 4.2 release.

==Upcoming Features==

Version 4.1.0
=============

For the latest information see 

    http://wiki.nexusformat.org/Nexus_41_Release_Notes

==System Requirements==
'''MXML XML Parsing Library'''

Version 2.2.2 or higher of mxml is required. Earlier versions have a bug and the XML API will not work. This package can be downloded in [http://www.easysw.com/~mike/mxml/software.php both source and binary rpm form] and is also available as part of [http://fedoraproject.org/wiki/Extras/UsingExtras Fedora Extras]. IMPORTANT NOTE: Debian also provides the mxml package, but it based on 2.0 and will not work properly.

==Building Notes==
===NAG F90/F95 Compiler===
The NAG compiler needs the '''-mismatch''' flag to be specified or else it will not compile NXmodule.f90 This is achieve by running configure with the '''FCFLAGS''' environment variable set to contain the flag e.g.
<pre>
env FCFLAGS="-mismatch" ./configure --with-f90=f95
</pre>
===HDF4 on Intel Macs===
There is a problem with the include file, hdfi.h (normally in /usr/local/include).  See http://coastwatch.noaa.gov/helparc/software/msg00069.html for details of the modifications necessary to fix it.

==New Features==
* New types NX_INT64 and NX_UINT64 to suppport 64 bit integers (only available in HDF5 and XML) [http://trac.nexusformat.org/code/ticket/87 details].
* Python bindings are now included in the Windows install kit [http://trac.nexusformat.org/code/ticket/86 details]

==Miscellaneous bug fixes==
The following items are bugs reported in previous releases and resolved in
the 4.1 release.
* The Fortran 90 part of testsuite failed with the Absoft compiler on MacOSX (it passed with g95 and gfortran (4.2)) [http://trac.nexusformat.org/code/ticket/68 details here]
* NXputattr assumed NULL termination of NX_CHAR attributes, which is usually the case in C but not true for JAVA. A workaround is to add '\0' manually [http://trac.nexusformat.org/code/ticket/83 bug report]
* pkgconfig issue [http://trac.nexusformat.org/code/ticket/84 bug report]
* Build issue with MXML-2.3 [http://trac.nexusformat.org/code/ticket/91 bug report]
* XML buffer resizing performance issue [http://trac.nexusformat.org/code/ticket/92 bug report]
* Documentation is now installed to "datadir" (/usr/share) [http://trac.nexusformat.org/code/ticket/93 bug report]

Version 4.0.0
=============

For the latest information see 

    http://wiki.nexusformat.org/Nexus_4_Release_Notes

==System Requirements==
'''MXML XML Parsing Library'''

Version 2.2.2 of mxml is required. Earlier versions have a bug and the XML API will not work. This package can be downloded in [http://www.easysw.com/~mike/mxml/software.php both source and binary rpm form] and is also available as part of [http://fedoraproject.org/wiki/Extras/UsingExtras Fedora Extras]. IMPORTANT NOTE: Debian also provides the mxml package, but it based on 2.0 and will not work properly.

==Building Notes==
===NAG F90/F95 Compiler===
The NAG compiler needs the '''-mismatch''' flag to be specified or else it will not compile NXmodule.f90 This is achieve by running configure with the '''FCFLAGS''' environment variable set to contain the flag e.g.
<pre>
env FCFLAGS="-mismatch" ./configure --with-f90=f95
</pre>  
==New Features==
The following items are features added to the NeXus API to provide new
functionality to the core library or to assist in the build process.
*Extended XML-API to handle unlimited dimensions
*Add building of Doxygen documentation
*Add support for two dimensional character arrays (HDF4 and HDF5 only)
*Added group attribute support to HDF4 (2006/05/02). Requires HDF4 version (???)
*Add NXmakenamedlink (2007/01/09) to all three file formats (external linking)
*Add NXprintlink
*Improved link testing in test suite
*API can now read generic HDF5 files, such as those produced by matlab
*Add facility to enable/disable error reporting
*New NXsummary tool for summarising contentes of a NeXus file
*Fortran 90 API now works with gfortran 4.2 and above as well as with G95
*PYTHON and TCL bindings provided via a [http://www.swig.org/ SWIG interface]
*Additional NXtranslate translators: SPEC, ESRF-EDF

==Changed Features==
The following aspects of the API have changed in a potentially non-backward compatible way
*The JAVA API now uses org.nexusformat rather than gov.anl.neutron.nexus

==Known Issues==
* The Fortran 90 part of testsuite fails with the Absoft compiler on MacOSX (it passes with g95 and gfortran (4.2)) [http://trac.nexusformat.org/code/ticket/68 details here]

==Miscellaneous bug fixes==
The following items are bugs reported in the 3.x releases and resolved for
the 4.0 release.
*Leading and trailing whitespace is stripped from char data on a read; this can be disabled by passing the NXACC_NOSTRIP option to NXopen
*Fix problems with MXML (what problems?)
*Improve test procedures when not all libraries are present
*Correct sourcepath for javadoc
*Updated makefiles for swig bindings (python, tcl)

Changes in Version 3.0.0
========================

- GNU Autotools are now used for building the API and programs

- XML files can be written by the NeXus API by specifying NXACC_XML to NXopen

- The NXtoXML, NXtoNX4 and NXtoNX5 utilities have been combined into a single utility nxconvert. To make a DTD definition file use nxtodtd

- New utilities nxdir and nxtranslate

- g95 compiler supported by FORTRAN90 bindings. To specify another
  compiler use the  --with-f90   option of configure

Changes in Version 2.0.0
========================

- NeXus library updated to allow the reading and writing of HDF5 files
  using the same API calls.  In creating a file, use NXACC_CREATE4 for
  HDF4 and NXACC_CREATE5 for HDF5.  As long as both HDF4 and HDF5 
  libraries are linked, the API will read both HDF4 and HDF5 files 
  transparently.

- The Makefiles have been reorganized to allow HDF4 and/or HDF5 files to 
  be linked into the NeXus library, with compile and link options stored
  in "makefile_options".

- Rearranged the NeXus libraries so that libNeXus.a contains the standard
  C and F77 interfaces and libNeXus90.a contains the C and F90 interfaces.

- Added the routine NXsameID to the C interface to compare the ID of
  NeXus groups and data sets.

- Added NXtoXML and NXtoDTD, utilities that translate a NeXus file to 
  XML, respectively with and without the data.  Note that NXtoDTD does not
  produce a true XML DTD file.  Instead, it produces a metaDTD as defined
  on the NeXus web pages (although it will lack proper annotation). 

- Changed NXUmodule.f90 so that NXUfindaxis uses the two accepted methods
  of defining dimension scales, i.e. adding the "axis" attribute to the
  axis data or adding the "axes" attribute to the signal data

Changes in Version 1.3.3
========================

- Added NXflush routine to flush data to the output file.

- Added support for unlimited dimensions.

- Used the NX_EXTERNAL macro to prefix callable NeXus functions and defined
  this to do the appropriate DLL import/export operation on Windows.
  README.WIN32 has been updated with instructions on how to make and
  use a NEXUS.DLL.

- NeXus errors now directed to MessageBox() in a Windows DLL.

- Modified timezone selection to use difftime/gmtime method on all
  systems except VMS 6.2 (where gmtime() always returns NULL).
  
- Corrected spurious overflow errors in NXGETCHARDATA.

- Added some unix-style commands, e.g. ls, cd, cat, as equivalents of
  standard commands in NXbrowse.

Changes in Version 1.3.2
========================

- NXbrowse now reads a filename as command line argument and only prompts if
  none is specified.

- Added "dump <data_name> <file_name>" command to NXbrowse, which dumps a 
  NeXus data array into an ASCII file.
  
- Added a toggle command to NXbrowse, called "byteaschar", which forces 
  NX_INT8, NX_UINT8 variables to be printed as character strings.  This is 
  for backward compatibility with files created before the definition of the
  NX_CHAR data type.  8-byte global attributes will be treated as characters
  when the program begins, but, after that, the default is to treat NX_INT8
  and NX_UINT8's as integers.
  
- Introduced proper typecasts for dimensions in data access routines.  This 
  is to correct problems with systems that do not have 4-byte int's.

- Added in the CALLING_STYLE macro to napi.h to allow correct calling of the
  FORTRAN interface under WINDOWS.  Corrected definition of NXcompress() on 
  PC.
  
- Added README.WIN32 to distribution discussing WINDOWS-specific compilation.


Changes in Version 1.3.1
========================

- Added NXgetgroupinfo, NXinitgroupdir, NXgetattrinfo, and NXinitattrdir
  to the core C and F77 API's.  They were already present in the F90 API.

- Updated the above routines in NXmodule.f90 so that they called the C
  versions directly.  This removes the need for the F90 code to know the
  internal details of the NXhandle C struct.

- Released a new version of NXbrowse written in ISO C.  It is the version
  that is now installed with the "make NXbrowse" command (on unix systems).
  The ordering of array indices uses the C convention i.e. it is reversed
  from the original F90 version.

- Corrected the calculation of time zone offsets when compiled by
  Metrowerks CodeWarrior for PowerPC Macs.

Changes in Version 1.3.0
========================

- Added NXcompress to the C, F77, and F90 API's. Compression works only with
  Release 3 of the HDF-4.1 libraries or better. Note: on my DigitalUnix 4.0D
  installation I had to link against the jpeg library supplied with HDF 
  directly due to a conflict with a system shared library. Replace -ljpeg
  by $HDFROOT/lib/libjpeg.a.

- Added NXUsetcompress to F90 utility API to set default compression 
  parameters, and added call to NXcompress in NXUwritedata.

- Changed attribute arguments in the F90 routine NXUwriteglobals to optional. 

Changes in Version 1.2.1
========================

- Released NeXus API under the terms of the GNU Lesser General Public
  License.

Changes in Version 1.2.0
========================

- NXGETCHARDATA, NXGETCHARATTR, NXPUTCHARDATA and NXPUTCHARATTR added 
  to Fortran 77 interface to correct problems with handling character data 
  and attributes.

- Reversed dimension order and adjusted starting indices in NXGETSLAB 
  and NXPUTSLAB in Fortran 77 interface for improved compatibility with the 
  C interface.

- All SDSes created with the DFTAG_NDG tag, rather than DFTAG_SDG,
  which is now deprecated by HDF.

- Expanded number of NeXus routines and data types checked by the test 
  programs "napi_test.c", "napif_test.f" and "NXtest.f90". 
  
- Excluded null terminator from string length check in REPLACE_STRING to
  prevent error messages when the string is exactly the right length.

- Added definitions of NX_MAXRANK (max. no. of dataset dimensions = 32) to 
  and NX_MAXNAMELEN (max. length of dataset name = 64) to header files.

- Made NXIReportError globally visible.

- Improved determination of local time zone when writing the global file
  attribute "file_time".

- Added option to NXbrowse.f90 to allow display of individual array elements
  of the specified index.

- Added check for __ABSOFT macro in "napi.h" to make compatible with 
  Absoft Pro Fortran for Linux.

- Add __stdcall definition to C prototypes under WIN32 so they can link 
  to FORTRAN. 

Changes in Version 1.1.0
========================

- First appearance of the Fortran 90 API.

- Added reading and writing of non-character attributes to the Fortran 77 
  interface.

- Corrected problems with linking groups by making NXgetgroupID 
  and NXmakelink use the HDF tag/ref pair in NXlink structures.

- Added VMS build file MAKE_VMS.COM.

- Added NEXUS_VERSION CHARACTER constant to NAPIF.INC, with value as defined 
  in napi.h.

- Made the Fortran 77 interface treat both NX_UINT8 and NX_CHAR as strings, for
  consistency with the C implementation and also for backward compatability 
  reasons.

- Fixed memory leak on Fortran 77 side on NXclose.

- NXdict added to the normal distribution.

- Added casts to HDF API calling parameters to eliminate type-mismatch
  compiler warnings.

- Modified time creation functions to handle non-working gmtime()
  function on VMS 6.2. 


Changes in Version 1.0.0
========================

- Reversed array dimensions on going from C to FORTRAN, so that the 
  "fastest varying index" is kept consistent.

- Incorporated fixes suggested by Markus Zolliker (markus.zolliker@psi.ch)
  for the following problems with the FORTRAN interface:

  (1) NXopen ends with a "Segmentation fault" if the file is not HDF.
  (2) NXgetentry gives an error message when NAME and CLASS are not
    initialised to blank.

- Changed "data" from char* to void* in NXgetattr.

- Changed value for NXACC_CREATE to be DFACC_CREATE rather than DFACC_ALL
  in NAPIF.INC for consistency with C interface.

- Added the following global attributes :
  NeXus_version - updated whenever a file is opened "read/write", so it
                  will always contain the latest version of the interface
                  the file was written with
  file_name     - set on file creation, then left unchanged
  file_time     - set on file creation, then left unchanged

$Id$
