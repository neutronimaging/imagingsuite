/*-----------------------------------------------------------------------------
  NeXus - Neutron & X-ray Common Data Format
   
  Utility to convert a NeXus file into HDF4/HDF5/XML/...
 
  Author: Freddie Akeroyd, Ray Osborn
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
  For further information, see <http://www.nexusformat.org>
 
 $Id: nxconvert.c 991 2008-03-19 19:30:03Z Freddie Akeroyd $
-----------------------------------------------------------------------------*/

#ifndef NXCONVERT_COMMON
#define NXCONVERT_COMMON 1

#define NX_XML		0
#define NX_HDF4		1
#define NX_HDF5		2
#define NX_DEFINITION	3

static const char* nx_formats[] = { "XML", "HDF4", "HDF5", "DEFINITION", NULL };

// not needed now we are not being used as a library
//#ifdef _WIN32
//#ifdef NXCONVERT_EXPORTS
//__declspec(dllexport)
//#else
//__declspec(dllimport)
//#endif /* NXCONVERT_EXPORTS */ 
//#endif /* _WIN32 */
extern int convert_file(int nx_format, const char* inFile, int nx_read_access, const char* outFile, int nx_write_access, const char* definition_name_);

#endif /* NXCONVERT_COMMON */
