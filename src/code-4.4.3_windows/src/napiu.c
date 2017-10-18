/*---------------------------------------------------------------------------
  NeXus - Neutron & X-ray Common Data Format
  
  NeXus Utility (NXU) Application Program Interface Header File
  
  Copyright (C) 2005 Freddie Akeroyd
  
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
 
  For further information, see <http://www.nexus.anl.gov/>
  
  $Id$

 ----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "napiu.h"
#include "napi_internal.h"

#define DO_GLOBAL(__name) \
	if (__name != NULL) \
	{ \
		if (NXputattr(file_id, #__name, (char*)__name, (int)strlen(__name), NX_CHAR) != NX_OK) \
		{ \
			return NX_ERROR; \
		} \
	}

 NXstatus  NXUwriteglobals(NXhandle file_id, const char* user, const char* affiliation, const char* address, const char* telephone_number, const char* fax_number, const char* email)
 {
	DO_GLOBAL(user);
	DO_GLOBAL(affiliation);
	DO_GLOBAL(address);
	DO_GLOBAL(telephone_number);
	DO_GLOBAL(fax_number);
	DO_GLOBAL(email);
	return NX_OK;
 }

 /* NXUwritegroup creates and leaves open a group */
 NXstatus  NXUwritegroup(NXhandle file_id, const char* group_name, const char* group_class)
 {
	   int status;
	   status = NXmakegroup(file_id, group_name, group_class);
	   if (status == NX_OK)
	   {
		   status = NXopengroup(file_id, group_name, group_class);
	   }
	 return status;
 }

 NXstatus  NXUwritedata(NXhandle file_id, const char* data_name, const void* data, int data_type, int rank, const int dim[], const char* units, const int start[], const int size[])
 {
	 return NX_OK;
 }

 NXstatus  NXUreaddata(NXhandle file_id, const char* data_name, void* data, char* units, const int start[], const int size[])
 {
	 return NX_OK;
 }

 NXstatus  NXUwritehistogram(NXhandle file_id, const char* data_name, const void* data, const char* units)
 {
	 return NX_OK;
 }

 NXstatus  NXUreadhistogram(NXhandle file_id, const char* data_name, void* data, char* units)
 {
	 return NX_OK;
 }

static int NXcompress_type = 0;
static int NXcompress_size = 0;

 /* NXUsetcompress sets the default compression type and minimum size */
 NXstatus  NXUsetcompress(NXhandle file_id, int comp_type, int comp_size)
 {
	 int status;
     if (comp_type == NX_COMP_LZW || comp_type == NX_COMP_HUF || 
          comp_type == NX_COMP_RLE || comp_type == NX_COMP_NONE)
	 {
         NXcompress_type = comp_type;
         if (comp_size != 0)
		 {
			 NXcompress_size = comp_size;
		 }
         status = NX_OK;
	 }
	 else
	 {
		 NXReportError( "Invalid compression option");
         status = NX_ERROR;
	 }
	 return status;
 }

	/* !NXUfindgroup finds if a NeXus group of the specified name exists */
 NXstatus  NXUfindgroup(NXhandle file_id, const char* group_name, char* group_class)
 {
	int status, n;
	NXname vname, vclass;
      status = NXgetgroupinfo(file_id, &n, vname, vclass);
      if (status != NX_OK)
	  {
		  return status;
	  }
	 return NX_OK;
 }

 NXstatus  NXUfindclass(NXhandle file_id, const char* group_class, char* group_name, int find_index)
 {
	 return NX_OK;
 }

/* NXUfinddata finds if a NeXus data item is in the current group */
 NXstatus  NXUfinddata(NXhandle file_id, const char* data_name)
 {
	 return NX_OK;
 }

 NXstatus  NXUfindattr(NXhandle file_id, const char* attr_name)
 {
	 return NX_OK;
 }

 NXstatus  NXUfindsignal(NXhandle file_id, int signal, char* data_name, int* data_rank, int* data_type, int data_dimensions[])
 {
	 return NX_OK;
 }

 NXstatus  NXUfindaxis(NXhandle file_id, int axis, int primary, char* data_name, int* data_rank, int* data_type, int data_dimensions[])
 {
	 return NX_OK;
 }

 NXstatus  NXUfindlink(NXhandle file_id, NXlink* group_id, const char* group_class)
 {
	 return NX_OK;
 }

 NXstatus  NXUresumelink(NXhandle file_id, NXlink group_id)
 {
	 return NX_OK;
 }

