## Process this file with automake to produce Makefile.in
#====================================================================
#  NeXus - Neutron & X-ray Common Data Format
#  
#  $Id$
#  
#  Makefile for NeXus FORTRAN 90 bindings
#
#  Copyright (C) 2004 Freddie Akeroyd
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
#  For further information, see <http://www.nexusformat.org>
#
#
#====================================================================
#
# Unix makefile for NeXusIDL-API DLM.
#
# This makefile is used to build NeXusIDL-API --- an example DLM.
# It works by determining what platform you are on and then
# recursively invoking make with the proper options to do the build.
#
idldlmdir = @IDLDLM@
idldlm_LTLIBRARIES = libNeXusIDL-API.la
idldlm_DATA = NeXusIDL-API.dlm

libNeXusIDL_API_la_SOURCES = NeXusIDL-API.c handle.c handle.h
libNeXusIDL_API_la_LIBADD = $(top_builddir)/src/libNeXus.la
libNeXusIDL_API_la_LDFLAGS = @SHARED_LDFLAGS@ -L$(IDLROOT)/bin/$(IDL_HOST) -lidl $(LDFLAGS)

AM_CPPFLAGS = -I$(IDLROOT)/external/include

all : all-am NeXusIDL-API.so

install-exec-hook :
	( cd $(DESTDIR)$(idldlmdir); ln -s libNeXusIDL-API.so NeXusIDL-API.so )

NeXusIDL-API.so : libNeXusIDL-API.la
	ln -sf .libs/libNeXusIDL-API.so $@
	if test ! -r NeXusIDL-API.dlm; then ln -s $(srcdir)/NeXusIDL-API.dlm .; fi

CLEANFILES = NeXusIDL-API.so

EXTRA_DIST = \
 build_testmodule.pro \
 build_win.bat \
 data \
 NeXusIDL-API.def \
 NeXusIDL-API.dlm \
 NeXusIDL-API.export \
 nxext.h5 \
 nxext.hdf \
 nxext.xml \
 NXtest.h5 \
 NXtest.hdf \
 NXtest.xml \
 README.html \
 read_test.pro \
 recursiveread.pro \
 recursivesearch.pro \
 testfocus.pro \
 write_test.pro \
 testidlnapi

dist-hook :
	find $(distdir)/data -depth -type d -name '.svn' -exec rm -fr {} \;

include $(top_srcdir)/build_rules.am
