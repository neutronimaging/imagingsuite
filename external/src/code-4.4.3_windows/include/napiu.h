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
  
#ifndef NEXUSAPIU
#define NEXUSAPIU

#include "napi.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern  NXstatus  NXUwriteglobals(NXhandle file_id, const char* user, const char* affiliation, const char* address, const char* phone, const char* fax, const char* email);

extern  NXstatus  NXUwritegroup(NXhandle file_id, const char* group_name, const char* group_class);

extern NXstatus  NXUwritedata(NXhandle file_id, const char* data_name, const void* data, int data_type, int rank, const int dim[], const char* units, const int start[], const int size[]);

extern NXstatus  NXUreaddata(NXhandle file_id, const char* data_name, void* data, char* units, const int start[], const int size[]);

extern NXstatus  NXUwritehistogram(NXhandle file_id, const char* data_name, const void* data, const char* units);

extern NXstatus  NXUreadhistogram(NXhandle file_id, const char* data_name, void* data, char* units);

extern NXstatus  NXUsetcompress(NXhandle file_id, int comp_type, int comp_size);

extern NXstatus  NXUfindgroup(NXhandle file_id, const char* group_name, char* group_class);

extern NXstatus  NXUfindclass(NXhandle file_id, const char* group_class, char* group_name, int find_index);

extern NXstatus  NXUfinddata(NXhandle file_id, const char* data_name);

extern NXstatus  NXUfindattr(NXhandle file_id, const char* attr_name);

extern NXstatus  NXUfindsignal(NXhandle file_id, int signal, char* data_name, int* data_rank, int* data_type, int data_dimensions[]);

extern NXstatus  NXUfindaxis(NXhandle file_id, int axis, int primary, char* data_name, int* data_rank, int* data_type, int data_dimensions[]);

extern NXstatus  NXUfindlink(NXhandle file_id, NXlink* group_id, const char* group_class);

extern NXstatus  NXUresumelink(NXhandle file_id, NXlink group_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*NEXUSAPIU*/

