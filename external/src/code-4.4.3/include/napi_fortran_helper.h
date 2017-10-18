/*---------------------------------------------------------------------------
  NeXus - Neutron & X-ray Common Data Format
  
  Application Program Interface Header File
  
  Copyright (C) 2015 NeXus International Advisory Committee
  
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
  
 ----------------------------------------------------------------------------*/

#include <napi.h>

/* 
 * FORTRAN helpers - for NeXus internal use only 
 */

#    define NXfopen             MANGLE(nxifopen)
#    define NXfclose            MANGLE(nxifclose)
#    define NXfflush            MANGLE(nxifflush)
#    define NXfmakedata         MANGLE(nxifmakedata)
#    define NXfcompmakedata     MANGLE(nxifcompmakedata)
#    define NXfcompress         MANGLE(nxifcompress)
#    define NXfputattr          MANGLE(nxifputattr)
#    define NXfgetpath          MANGLE(nxifgetpath)

  extern NXstatus  NXfopen(char * filename, NXaccess* am, 
					NXhandle pHandle);
  extern NXstatus  NXfclose (NXhandle pHandle);
  extern NXstatus  NXfputattr(NXhandle fid, const char *name, const void *data, 
                                   int *pDatalen, int *pIType);
  extern NXstatus  NXfcompress(NXhandle fid, int *compr_type);
  extern NXstatus  NXfcompmakedata(NXhandle fid, char *name, 
                int *pDatatype,
		int *pRank, int dimensions[],
                int *compression_type, int chunk[]);
  extern NXstatus  NXfmakedata(NXhandle fid, char *name, int *pDatatype,
		int *pRank, int dimensions[]);
  extern NXstatus  NXfflush(NXhandle pHandle);
  extern NXstatus  NXfgetpath(NXhandle fid, char *path, int *pathlen);
