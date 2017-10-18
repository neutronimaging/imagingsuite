/*---------------------------------------------------------------------------
  NeXus - Neutron & X-ray Common Data Format
  
  Application Program Interface Routines
  
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <napi_fortran_helper.h>
#include <napi_internal.h>
#include "nxstack.h"

/*----------------------------------------------------------------------
                 F77 - API - Support - Routines
  ----------------------------------------------------------------------*/
  /*
   * We store the whole of the NeXus file in the array - that way
   * we can just pass the array name to C as it will be a valid
   * NXhandle. We could store the NXhandle value in the FORTRAN array
   * instead, but that would mean writing far more wrappers
   */
NXstatus NXfopen(char *filename, NXaccess * am, NXhandle pHandle)
{
	NXstatus ret;
	NXhandle fileid = NULL;
	ret = NXopen(filename, *am, &fileid);
	if (ret == NX_OK) {
		memcpy(pHandle, fileid, getFileStackSize());
	} else {
		memset(pHandle, 0, getFileStackSize());
	}
	if (fileid != NULL) {
		free(fileid);
	}
	return ret;
}

/* 
 * The pHandle from FORTRAN is a pointer to a static FORTRAN
 * array holding the NexusFunction structure. We need to malloc()
 * a temporary copy as NXclose will try to free() this
 */
NXstatus NXfclose(NXhandle pHandle)
{
	NXhandle h;
	NXstatus ret;
	h = (NXhandle) malloc(getFileStackSize());
	memcpy(h, pHandle, getFileStackSize());
	ret = NXclose(&h);	/* does free(h) */
	memset(pHandle, 0, getFileStackSize());
	return ret;
}

/*---------------------------------------------------------------------*/
NXstatus NXfflush(NXhandle pHandle)
{
	NXhandle h;
	NXstatus ret;
	h = (NXhandle) malloc(getFileStackSize());
	memcpy(h, pHandle, getFileStackSize());
	ret = NXflush(&h);	/* modifies and reallocates h */
	memcpy(pHandle, h, getFileStackSize());
	return ret;
}

/*----------------------------------------------------------------------*/
NXstatus NXfmakedata(NXhandle fid, char *name, int *pDatatype,
		     int *pRank, int dimensions[])
{
	NXstatus ret;
	static char buffer[256];
	int i, *reversed_dimensions;
	reversed_dimensions = (int *)malloc(*pRank * sizeof(int));
	if (reversed_dimensions == NULL) {
		sprintf(buffer,
			"ERROR: Cannot allocate space for array rank of %d in NXfmakedata",
			*pRank);
		NXReportError(buffer);
		return NX_ERROR;
	}
/*
 * Reverse dimensions array as FORTRAN is column major, C row major
 */
	for (i = 0; i < *pRank; i++) {
		reversed_dimensions[i] = dimensions[*pRank - i - 1];
	}
	ret = NXmakedata(fid, name, *pDatatype, *pRank, reversed_dimensions);
	free(reversed_dimensions);
	return ret;
}

/*-----------------------------------------------------------------------*/
NXstatus NXfcompmakedata(NXhandle fid, char *name,
			 int *pDatatype,
			 int *pRank, int dimensions[],
			 int *compression_type, int chunk[])
{
	NXstatus ret;
	static char buffer[256];
	int i, *reversed_dimensions, *reversed_chunk;
	reversed_dimensions = (int *)malloc(*pRank * sizeof(int));
	reversed_chunk = (int *)malloc(*pRank * sizeof(int));
	if (reversed_dimensions == NULL || reversed_chunk == NULL) {
		sprintf(buffer,
			"ERROR: Cannot allocate space for array rank of %d in NXfcompmakedata",
			*pRank);
		NXReportError(buffer);
		return NX_ERROR;
	}
/*
 * Reverse dimensions array as FORTRAN is column major, C row major
 */
	for (i = 0; i < *pRank; i++) {
		reversed_dimensions[i] = dimensions[*pRank - i - 1];
		reversed_chunk[i] = chunk[*pRank - i - 1];
	}
	ret = NXcompmakedata(fid, name, *pDatatype, *pRank,
			     reversed_dimensions, *compression_type,
			     reversed_chunk);
	free(reversed_dimensions);
	free(reversed_chunk);
	return ret;
}

/*-----------------------------------------------------------------------*/
NXstatus NXfcompress(NXhandle fid, int *compr_type)
{
	return NXcompress(fid, *compr_type);
}

/*-----------------------------------------------------------------------*/
NXstatus NXfputattr(NXhandle fid, const char *name, const void *data,
		    int *pDatalen, int *pIType)
{
	return NXputattr(fid, name, data, *pDatalen, *pIType);
}

  /*
   * implement snprintf when it is not available 
   */
int nxisnprintf(char *buffer, int len, const char *format, ...)
{
	int ret;
	va_list valist;
	va_start(valist, format);
	ret = vsprintf(buffer, format, valist);
	va_end(valist);
	return ret;
}

/*--------------------------------------------------------------------------*/
NXstatus NXfgetpath(NXhandle fid, char *path, int *pathlen)
{
	return NXgetpath(fid, path, *pathlen);
}
