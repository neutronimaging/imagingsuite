======
nxdump 
======

-----------------------------
list contents of a NeXus file
-----------------------------

:Manual section: 1
:Manual group: NeXus tools
    
SYNOPSIS
========
    nxdump [NEXUS FILE]
    
DESCRIPTION
===========
    nxdump prompts for a NeXus file name, opens the file, and recursively 
    lists the groups and their contents, i.e., other groups and fields, within 
    the file.  The name and class of each group, and the name, datatype, 
    rank and dimensions of each field are output.
    
OPTIONS
=======
    No command line options are supported
    
SEE ALSO
========.
    nxbrowse(1) http://www.nexusformat.org
    
AUTHOR
======
   nxdump was originally written by Ray Osborn <ROsborn@anl.gov>
