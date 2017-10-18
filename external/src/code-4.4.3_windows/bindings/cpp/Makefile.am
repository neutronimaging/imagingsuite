## Process this file with automake to produce Makefile.in
#====================================================================
#  NeXus - Neutron & X-ray Common Data Format
#  
#  $Id: Makefile.am 598 2005-08-19 16:19:15Z faa59 $
#  
#  Makefile for NeXus C++ bindings
#
#  Copyright (C) 2008 Freddie Akeroyd
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
#  For further information, see http://www.nexusformat.org/
#
#====================================================================
####################### Subversion Reposity details ########################
# Repository Location     $HeadURL$
# Revision of last commit $LastChangedRevision$ 
# Date of last commit     $LastChangedDate$
# Author of last commit   $LastChangedBy$
############################################################################
LIBNEXUS=$(top_builddir)/src/libNeXus.la
lib_LTLIBRARIES=libNeXusCPP.la

# nxincludedir=$(includedir)/nexus
nxincludedir=$(pkgincludedir)
nxinclude_HEADERS=NeXusFile.hpp NeXusException.hpp NeXusStream.hpp

libNeXusCPP_la_SOURCES=NeXusFile.hpp NeXusFile.cpp NeXusException.hpp NeXusException.cpp NeXusStream.hpp NeXusStream.cpp
libNeXusCPP_la_LIBADD=$(LIBNEXUS)
libNeXusCPP_la_LDFLAGS=@SHARED_LDFLAGS@ -version-info $(NXLTVERSINFO) -L$(top_builddir)/src/.libs

AM_CPPFLAGS=-I. -I$(top_srcdir)/include -DIN_NEXUS_CPP_LIBRARY=1

if MINGW_MSYS
MINGW_CXXFLAGS=-Wl,--export-all-symbols
msimplibdir = ${libdir}
libNeXusCPP.def: libNeXusCPP.la
	pexports .libs/libNeXusCPP-0.dll > libNeXusCPP.def
if HAVE_MS_LIB
msimplib_DATA = libNeXusCPP.dll.lib libNeXusCPP.dll.exp libNeXusCPP.def
libNeXusCPP.dll.exp: libNeXusCPP.dll.lib
libNeXusCPP.dll.lib: libNeXusCPP.def
	rm -f libNeXusCPP.dll.lib libNeXusCPP.dll.exp
	$(MS_LIB) /MACHINE:I386 /DEF:libNeXusCPP.def /OUT:libNeXusCPP.dll.lib
else
msimplib_DATA = libNeXusCPP.def
endif
endif

AM_CXXFLAGS=-prefer-pic $(MINGW_CXXFLAGS)
CLEANFILES=libNeXusCPP.def libNeXusCPP.dll.lib libNeXusCPP.dll.exp

include $(top_srcdir)/build_rules.am
