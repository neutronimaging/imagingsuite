/*
  NeXus - Neutron & X-ray Common Data Format
  
  IDL Application Program Interface Routines
  
  Copyright (C) 2007-* Jussi Kauppila, Mark Koennecke 
  
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
*/
#include <stdio.h>
#include <stdlib.h>
#include "idl_export.h"
#include "../../include/napi.h" 
#include "handle.h"

/* Windows path: */
/* #include "C:\Program Files\NeXus Data Format\include\napi.h" */

/* prototype for IDL_Load */
int IDL_Load( void );

/*
 * Define message codes and their corresponding printf(3) format
 * strings. Note that message codes start at zero and each one is
 * one less that the previous one. Codes must be monotonic and
 * contiguous.
 */
static IDL_MSG_DEF msg_arr[] =
{
#define M_TM_INPRO                       0
  {  "M_TM_INPRO",   "%NThis is from a loadable module procedure." },
#define M_TM_INFUN                       -1
  {  "M_TM_INFUN",   "%NThis is from a loadable module function." },
};

/*
 * The load function fills in this message block handle with the
 * opaque handle to the message block used for this module. The other
 * routines can then use it to throw errors from this block.
 */
static IDL_MSG_BLOCK msg_block;

static char nexusError[1024];

static void idlError(void *pData, char *message){
	strncpy(nexusError,message,1023);
}

/* Implementation of the NeXus IDL API */

/*======================================================================
 * NxOpen
 * status = NXopen (file_name, access_method, file_id)
 *======================================================================*/
static IDL_VPTR NXopen_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR result;
  int status, iHandle;
  NXaccess am;
  char* accesstype;
  char* path;
  NXhandle hHandle;

  /* if ENSURE fails it jumps a "long jump" back and complains but the
     proper NX_ERROR status is not returned. Maybe write an own macro */
  IDL_ENSURE_STRING(argv[0]);
  IDL_ENSURE_STRING(argv[1]);

  /* Convert argumets from IDL side to C-land */
  path = IDL_VarGetString(argv[0]);
  accesstype = IDL_VarGetString(argv[1]);

  /* If number of arguments is not 3 return status NX_ERROR */
  if (argc != 3) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}


  /* If the accesstype isn't NXACC_CREATE5 NXACC_CREATE NXACC_READ,NXACC_RDWR return NX_ERROR */

   if (strcmp(accesstype,"NXACC_CREATE5") == 0) {;
       am = NXACC_CREATE5;
    } else if (strcmp(accesstype,"NXACC_CREATE") == 0) {
       am = NXACC_CREATE;
    } else if (strcmp(accesstype,"NXACC_CREATE4") == 0) {
       am = NXACC_CREATE4;
    } else if (strcmp(accesstype,"NXACC_CREATEXML") == 0) {
       am = NXACC_CREATEXML;
    } else if (strcmp(accesstype,"NXACC_READ") == 0) {
       am = NXACC_READ;
    } else if (strcmp(accesstype,"NXACC_RDWR") == 0) {
       am = NXACC_RDWR;
    } else {
       IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown Access Type");
       return IDL_GettmpInt(NX_ERROR);
    }

  NXMSetError(NULL,idlError);
  status = NXopen (path, am, &hHandle);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  iHandle = HHMakeHandle(hHandle);
    if(iHandle == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Error creating the handle");
		return IDL_GettmpInt(NX_ERROR);
	}


  result = IDL_GettmpInt(iHandle);

  /* Output argument */
  IDL_VarCopy(result, argv[2]);

  return IDL_GettmpInt(status);
}


/*======================================================================
 * Nxopenpath
 * status = NXopenpath(file_id, path_string)
 *======================================================================*/

static IDL_VPTR NXopenpath_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  char* path;

  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);

  fileid = (int)IDL_LongScalar(argv[0]);
  path = IDL_VarGetString(argv[1]);


  if(HHCheckIfHandleExists(fileid) == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);
  NXMSetError(NULL,idlError);

  status = NXopenpath (hHandle, path);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXopengrouppath
 * status = NXopengrouppath(file_id, path_string)
 *======================================================================*/

static IDL_VPTR NXopengrouppath_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  char* path;

  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);

  fileid = (int)IDL_LongScalar(argv[0]);
  path = IDL_VarGetString(argv[1]);


  if(HHCheckIfHandleExists(fileid) == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);
  NXMSetError(NULL,idlError);

  status = NXopengrouppath (hHandle, path);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}
/*======================================================================
 * NXgetpath
 * string = NXgetpath(file_id)
 *======================================================================*/
static IDL_VPTR NXgetpath_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  char path[1024];

  if (argc != 1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "One argument expected");
	return IDL_StrToSTRING("One argument expected");
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int)IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_StrToSTRING("ERROR: Unknown file ID");
	}

  hHandle = HHGetPointer(fileid);
  NXMSetError(NULL,idlError);

  status = NXgetpath(hHandle,path,1024);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_StrToSTRING(nexusError);
  }
   return IDL_StrToSTRING(path);
}

/*======================================================================
 * Nxclose
 * status = NXclose (file_id)
 *=======================================================================*/
static IDL_VPTR  NXclose_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;

  /* If number of arguments is not 2 return 0 */
  if (argc != 1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "One argument expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  /* Convert IDL variables from arguments to C-land */
  fileid = (int) IDL_LongScalar(argv[0]);

  if( HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = (int *)HHGetPointer(fileid);


  NXMSetError(NULL,idlError);

  status = NXclose (&hHandle);

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

	  HHRemoveHandle(fileid);

  return IDL_GettmpInt(status);
}

/*======================================================================
 * datatypecheck
 *
 *=======================================================================*/


int datatypecheck(IDL_VPTR argv) {
  int data_type;
  char* datatype;

  if(argv->type == IDL_TYP_STRING) {
	IDL_ENSURE_STRING(argv);
	datatype = IDL_VarGetString(argv);

  if (strcmp(datatype,"NX_CHAR") == 0) {
       data_type = NX_CHAR;
    } else if (strcmp(datatype,"NX_INT8") == 0) {
       data_type = NX_INT8;
    } else if (strcmp(datatype,"NX_UINT8") == 0) {
       data_type = NX_UINT8;
    } else if (strcmp(datatype,"NX_INT16") == 0) {
       data_type = NX_INT16;
    } else if (strcmp(datatype,"NX_UINT16") == 0) {
       data_type = NX_UINT16;
    } else if (strcmp(datatype,"NX_INT32") == 0) {
       data_type = NX_INT32;
    } else if (strcmp(datatype,"NX_UINT32") == 0) {
       data_type = NX_UINT32;
    } else if (strcmp(datatype,"NX_FLOAT32") == 0) {
       data_type = NX_FLOAT32;
    } else if (strcmp(datatype,"NX_FLOAT64") == 0) {
       data_type = NX_FLOAT64;
    } else {
	 	char statusBuffer[100];
		sprintf(statusBuffer,"Unknown attribute datatype: %s", datatype );
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
		return(-1);
    }

  }
else {
  IDL_ENSURE_SCALAR(argv);
  data_type =  IDL_LongScalar(argv);
  if(data_type == 4); else if (data_type == 5); else if (data_type == 6); else if
     (data_type == 20); else if (data_type == 21); else if (data_type == 22); else if
     (data_type == 23); else if (data_type == 24); else if (data_type == 25); else
	{
	char statusBuffer[100];
	sprintf(statusBuffer,"Unknown attribute datatype: %i", data_type );
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
	return(-1);
	}
  }

 return(data_type);
}


/*======================================================================
 * NXsetnumberformat
 * status = NXsetnumberformat(file_id,data_type,format_string)
 *=======================================================================*/

static IDL_VPTR NXsetnumberformat_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int data_type;
  char* format_string;
  char* datatype;

  if (argc != 3) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}
  IDL_ENSURE_SCALAR(argv[0]);

  IDL_ENSURE_STRING(argv[2]);
  format_string = IDL_VarGetString(argv[2]);


  data_type = datatypecheck(argv[1]);
  if(data_type == -1) return IDL_GettmpInt(NX_ERROR);

/*if(argv[1]->type == IDL_TYP_STRING) {
  IDL_ENSURE_STRING(argv[1]);
  datatype = IDL_VarGetString(argv[1]);

  if (strcmp(datatype,"NX_CHAR") == 0) {
       data_type = NX_CHAR;
    } else if (strcmp(datatype,"NX_INT8") == 0) {
       data_type = NX_INT8;
    } else if (strcmp(datatype,"NX_UINT8") == 0) {
       data_type = NX_UINT8;
    } else if (strcmp(datatype,"NX_INT16") == 0) {
       data_type = NX_INT16;
    } else if (strcmp(datatype,"NX_UINT16") == 0) {
       data_type = NX_UINT16;
    } else if (strcmp(datatype,"NX_INT32") == 0) {
       data_type = NX_INT32;
    } else if (strcmp(datatype,"NX_UINT32") == 0) {
       data_type = NX_UINT32;
    } else if (strcmp(datatype,"NX_FLOAT32") == 0) {
       data_type = NX_FLOAT32;
    } else if (strcmp(datatype,"NX_FLOAT64") == 0) {
       data_type = NX_FLOAT64;
    } else {
	 	char statusBuffer[100];
		sprintf(statusBuffer,"Unknown attribute datatype: %s", datatype );
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
		return IDL_GettmpInt(NX_ERROR);
    }

  }
else {
  IDL_ENSURE_SCALAR(argv[1]);
  data_type =  IDL_LongScalar(argv[1]);
  if(data_type == 4); else if (data_type == 5); else if (data_type == 6); else if
     (data_type == 20); else if (data_type == 21); else if (data_type == 22); else if
     (data_type == 23); else if (data_type == 24); else if (data_type == 25); else
	{
	char statusBuffer[100];
	sprintf(statusBuffer,"Unknown attribute datatype: %i", data_type );
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
	return IDL_GettmpInt(NX_ERROR);
	}
  }*/

  fileid = (int ) IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = (int *)HHGetPointer(fileid);
  NXMSetError(NULL,idlError);

  status = NXsetnumberformat(hHandle, data_type,format_string);

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(status);
}
/*======================================================================
 * NXmakegroup
 * status = NXmakegroup (file_id, group_name, group_class)
 *=======================================================================*/

static IDL_VPTR NXmakegroup_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  char* group_name;
  char* group_class;
  if (argc != 3) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);
  IDL_ENSURE_STRING(argv[2]);

  fileid = (int)IDL_LongScalar(argv[0]);

  group_name = IDL_VarGetString(argv[1]);
  group_class = IDL_VarGetString(argv[2]);
  if(HHCheckIfHandleExists(fileid) == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXmakegroup (hHandle, group_name, group_class);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(status);
}

/*======================================================================
 * Nxopengroup
 * status = NXopengroup (file_id, group_name, group_class)
 *=======================================================================*/

static IDL_VPTR NXopengroup_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  char* group_name;
  char* group_class;

  if (argc != 3) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);
  IDL_ENSURE_STRING(argv[2]);

  fileid = (int)IDL_LongScalar(argv[0]);
  group_name = IDL_VarGetString(argv[1]);
  group_class = IDL_VarGetString(argv[2]);


  if(HHCheckIfHandleExists(fileid) == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXopengroup (hHandle, group_name, group_class);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXclosegroup
 * status = NXclosegroup (file_id)
 *=======================================================================*/

static IDL_VPTR NXclosegroup_this(int argc, IDL_VPTR *argv)
{

  int fileid;
  NXhandle hHandle;
  int iHandle, status;

  if (argc != 1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "One argument expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int )IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);


  NXMSetError(NULL,idlError);

  status = NXclosegroup (hHandle);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXopendata
 * status = NXopendata (file_id, data_name)
 *=======================================================================*/

static IDL_VPTR NXopendata_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  char* data_name;

  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);

  fileid = (int )IDL_LongScalar(argv[0]);
  data_name = IDL_VarGetString(argv[1]);

  if(HHCheckIfHandleExists(fileid) == -1) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXopendata (hHandle, data_name);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}


/*======================================================================
 * Nxclosedata
 * status = NXclosedata (file_id)
 *=======================================================================*/
static IDL_VPTR  NXclosedata_this(int argc, IDL_VPTR *argv)
{

  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  NXlink data_id;

  /* If number of arguments is not 2 return 0
   Should be handled also with the calling conversion not shure if necessary..*/
  if (argc != 1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "One argument expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int ) IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

 /* status = NXgetdataID (hHandle, &data_id);
  if(status == NX_ERROR) {
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dataset must be opened before nxclosedata");
		return IDL_GettmpInt(NX_ERROR);
	  }*/

  NXMSetError(NULL,idlError);

  status = NXclosedata (hHandle);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXcompress
 * statusstatus = NXcompress (file_id, compress_type)
 *=======================================================================*/

static IDL_VPTR NXcompress_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int compresstype;
  char* compress_type;
  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);

  fileid = (int )IDL_LongScalar(argv[0]);
  compress_type = IDL_VarGetString(argv[1]);

   if (strcmp(compress_type,"NX_COMP_LZW") == 0) {;
       compresstype = NX_COMP_LZW;
    } else if (strcmp(compress_type,"NX_COMP_HUF") == 0) {
       compresstype = NX_COMP_HUF;
    } else if (strcmp(compress_type,"NX_COMP_RLE") == 0) {
       compresstype = NX_COMP_RLE;
    } else {
       IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown Compression Type");
       return IDL_GettmpInt(NX_ERROR);
    }
  if(HHCheckIfHandleExists(fileid) == -1) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);
  NXMSetError(NULL,idlError);

  status = NXcompress (hHandle, (int) &compresstype);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXmakedata
 * status = NXmakedata (file_id, data_name, data_type, rank, dimensions[])
 *=======================================================================*/

static IDL_VPTR NXmakedata_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int dimension[NX_MAXRANK], trueDimension[NX_MAXRANK], n, datatype;
  int rank;
  IDL_LONG *dim;
  char* data_name;
  char* data_type;
  //IDL_LONG dims[IDL_MAX_ARRAY_DIM];


  if (argc != 5) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Five arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);

  IDL_ENSURE_SCALAR(argv[3]);


  data_name = IDL_VarGetString(argv[1]);

  datatype = datatypecheck(argv[2]);
  if(datatype == -1) return IDL_GettmpInt(NX_ERROR);

  fileid = (int) IDL_LongScalar(argv[0]);
  rank = (int ) IDL_LongScalar(argv[3]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = (int *)HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  if(argv[4]->type == IDL_TYP_STRING) {
	char* unlimited_test = IDL_VarGetString(argv[4]);
	if(strcmp(unlimited_test,"NX_UNLIMITED") == 0) {
		int unlimited_dims[1] = {NX_UNLIMITED};
	  	status = NXmakedata(hHandle, data_name, datatype, (int)rank, (int *) unlimited_dims);
		}
	  else {
    IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dimension array must be type long or String: 'NX_UNLIMITED'");
	return IDL_GettmpInt(NX_ERROR);
	}
  }
  else if(argv[4]->type == IDL_TYP_LONG) {
  	  IDL_ENSURE_ARRAY(argv[4]);
	  /* dimensions have to be swapped for Fortran C storage order */
	  dim = (IDL_LONG *)argv[4]->value.arr->data;
	  for(n = 0; n < rank; n++){
	    trueDimension[n] = dim[rank - 1 -n];
          }
  	  status = NXmakedata(hHandle, data_name, datatype, rank, 
			      trueDimension);
  }
  else {
    IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dimension array must be type long or String: 'NX_UNLIMITED'");
	return IDL_GettmpInt(NX_ERROR);
	}

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }
  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXcompmakedata
 * statusstatus = NXcompmakedata (file_id, data_name, data_type, rank, dimensions[], compress_type, bufsize[])
 *=======================================================================*/

static IDL_VPTR NXcompmakedata_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int rank, dimension[NX_MAXRANK], trueDimension[NX_MAXRANK], 
    trueChunk[NX_MAXRANK], n,  datatype;
  IDL_LONG *dim;
  int comptype;
  char* data_type;
  char* data_name;
  char* comp_type;

  if (argc != 7) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Seven arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);

  IDL_ENSURE_SCALAR(argv[3]);
  IDL_ENSURE_ARRAY(argv[4]);
  IDL_ENSURE_STRING(argv[5]);
  IDL_ENSURE_ARRAY(argv[6]);

  data_name = IDL_VarGetString(argv[1]);

  datatype = datatypecheck(argv[2]);
  if(datatype == -1) return IDL_GettmpInt(NX_ERROR);

  /* Compressio type check */
  if(argv[5]->type == IDL_TYP_STRING) {
  IDL_ENSURE_STRING(argv[5]);
  comp_type = IDL_VarGetString(argv[5]);

  if (strcmp(comp_type,"NX_COMP_LZW") == 0) {
       comptype = NX_COMP_LZW;
    } else if (strcmp(comp_type,"NX_COMP_HUF") == 0) {
       comptype = NX_COMP_HUF;
    } else if (strcmp(comp_type,"NX_COMP_RLE") == 0) {
       comptype = NX_COMP_RLE;
    } else {
	char statusBuffer[100];
	sprintf(statusBuffer,"Unknown Compression Type: %s", comp_type );
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
	return IDL_GettmpInt(NX_ERROR);
    }

  }
else {
  IDL_ENSURE_SCALAR(argv[5]);
  comptype = IDL_LongScalar(argv[5]);
  if(datatype == NX_COMP_LZW); else if (datatype == NX_COMP_HUF); else if (datatype == NX_COMP_RLE);
  else{
	char statusBuffer[100];
	sprintf(statusBuffer,"Unknown Compression Type: %i", comptype );
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
	return IDL_GettmpInt(NX_ERROR);
	}
  }

  if(argv[4]->type != IDL_TYP_LONG) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Argument 5, dimensions, Array must be type long");
	return IDL_GettmpInt(NX_ERROR);
  }

  if(argv[6]->type != IDL_TYP_LONG) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Argument 7, Buffer Array: Array must be type long");
	return IDL_GettmpInt(NX_ERROR);
  }

  fileid = (int ) IDL_LongScalar(argv[0]);
  rank = (int ) IDL_LongScalar(argv[3]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
 hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  /* dimensions have to be swapped for Fortran C storage order */
  dim = (IDL_LONG *)argv[4]->value.arr->data;
  for(n = 0; n < rank; n++){
    trueDimension[n] = dim[rank - 1 -n];
  }
  dim = (IDL_LONG *)argv[6]->value.arr->data;
  if(dim != NULL){
    for(n = 0; n < rank; n++){
      trueChunk[n] = dim[rank - 1 -n];
    }
  } else {
    for(n = 0; n < rank; n++){
      trueChunk[n] = trueDimension[n];
    }
  }

  status = NXcompmakedata(hHandle, data_name, datatype, (int)rank, 
			  trueDimension, comptype, 
			  trueChunk);

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXputdata
 * status = NXputdata (file_id, data[])
 *=======================================================================*/

static IDL_VPTR NXputdata_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int rank, datatype;
  int dimension[IDL_MAX_ARRAY_DIM];
  NXlink data_id;

  fileid = (int)IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  hHandle = HHGetPointer(fileid);

  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  status = NXgetdataID (hHandle, &data_id);
/*  if(status == NX_ERROR) {
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dataset must be opened before nxputdata");
		return IDL_GettmpInt(NX_ERROR);
		} */

 /* get info, datatype especially! */
    status = NXgetinfo (hHandle, &rank, dimension, &datatype);
    if(status == NX_ERROR) {
 	 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Can't get the dimension and datatype info from The Nexus file");
		return IDL_GettmpInt(NX_ERROR);
	}

	/* Check if the IDL datatypes and Nexusdatatypes match */

    if ((datatype == NX_CHAR) && (argv[1]->type == IDL_TYP_STRING)) {
    } else if ((datatype == NX_INT8) && (argv[1]->type == IDL_TYP_BYTE)) {
    	/*	 This datatype cannot be created on the idl side so it cannot be an argument.  */

     	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NX_INT8: data type is not supported in IDL");
		return IDL_GettmpInt(NX_ERROR);
	} else if (datatype == NX_UINT8){
    } else if ((datatype == NX_INT16) && (argv[1]->type == IDL_TYP_INT)) {
    } else if ((datatype == NX_UINT16) && (argv[1]->type == IDL_TYP_UINT)) {
    } else if ((datatype == NX_INT32) && (argv[1]->type == IDL_TYP_LONG)) {
    } else if ((datatype == NX_UINT32) && (argv[1]->type == IDL_TYP_ULONG))  {
    } else if ((datatype == NX_FLOAT32) && (argv[1]->type == IDL_TYP_FLOAT)) {
    } else if ((datatype == NX_FLOAT64) && (argv[1]->type == IDL_TYP_DOUBLE)) {
    } else {
		char statusBuffer[100];
		/* Maybe print more information to the user about which datatypes match OR do conversions */
		sprintf(statusBuffer,"IDL and NeXus types don't match!");
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
		return IDL_GettmpInt(NX_ERROR);
    }


  NXMSetError(NULL,idlError);
  if(datatype == NX_CHAR) {

  	/* Should there be a test here for trying to put a multidimensinal char array in to NeXus
  	 * becose they are not really supported */

	  char* char_buffer = IDL_VarGetString(argv[1]);
	  status = NXputdata (hHandle, char_buffer);
  }
  else {
	  status = NXputdata (hHandle, argv[1]->value.arr->data);
  }

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXputslab
 * statusstatus = NXputslab (file_id, data, start[], size[])
 *=======================================================================*/

static IDL_VPTR NXputslab_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int rank, datatype, n;
  int start[NX_MAXRANK], size[NX_MAXRANK];
  IDL_LONG *stDim, *szDim;
  int dimension[IDL_MAX_ARRAY_DIM];
  NXlink data_id;

  if (argc != 4) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Four arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}


  fileid = (int)IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_ARRAY(argv[2]);
  IDL_ENSURE_ARRAY(argv[3]);

  if(argv[2]->type != IDL_TYP_LONG) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Array must be type long");
	return IDL_GettmpInt(NX_ERROR);
  }

  if(argv[3]->type != IDL_TYP_LONG) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Array must be type long");
	return IDL_GettmpInt(NX_ERROR);
  }

  hHandle = HHGetPointer(fileid);

  status = NXgetdataID (hHandle, &data_id);
/* if(status == NX_ERROR) {
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dataset must be opened before nxputdata");
		return IDL_GettmpInt(NX_ERROR);
		} */

 /* get info, datatype especially! */
    status = NXgetinfo (hHandle, &rank, dimension, &datatype);
    if(status == NX_ERROR) {
 	 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Can't get the dimension and datatype info from The Nexus file");
		return IDL_GettmpInt(NX_ERROR);
	}
	/* Check if the IDL datatypes and Nexusdatatypes match */

    if ((datatype == NX_CHAR) && (argv[1]->type == IDL_TYP_STRING)) {
    } else if ((datatype == NX_INT8) && (argv[1]->type == IDL_TYP_BYTE)) {
	/*
       } else if (datatype == NX_UINT8){
    	 This datatype cannot be created on the idl side so it cannot be an argument.
     */
    } else if ((datatype == NX_INT16) && (argv[1]->type == IDL_TYP_INT)) {
    } else if ((datatype == NX_UINT16) && (argv[1]->type == IDL_TYP_UINT)) {
    } else if ((datatype == NX_INT32) && (argv[1]->type == IDL_TYP_LONG)) {
    } else if ((datatype == NX_UINT32) && (argv[1]->type == IDL_TYP_ULONG))  {
    } else if ((datatype == NX_FLOAT32) && (argv[1]->type == IDL_TYP_FLOAT)) {
    } else if ((datatype == NX_FLOAT64) && (argv[1]->type == IDL_TYP_DOUBLE)) {
    } else {
		char statusBuffer[100];
		/* Maybe print more information to the user about which datatypes match OR do conversions */
		sprintf(statusBuffer,"IDL and NeXus types don't match!: %i & %i ", 
			datatype, argv[1]->type);
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
		return IDL_GettmpInt(NX_ERROR);
    }

  NXMSetError(NULL,idlError);

  if(datatype == NX_CHAR) {

  	/* Should there be a test here for trying to put a multidimensinal char array in to NeXus
  	 * becose they are not really supported */

	char* char_buffer = IDL_VarGetString(argv[1]);
	status = NXputslab (hHandle, char_buffer, (void *)argv[2]->value.arr->data, (void *)argv[3]->value.arr->data);

  }
  else {
    stDim = (IDL_LONG *) argv[2]->value.arr->data;
    szDim = (IDL_LONG *) argv[3]->value.arr->data;
    for(n = 0; n < rank; n++){
      start[n] = stDim[rank -n -1];
      size[n] = szDim[rank -n -1];
    }
		status = NXputslab (hHandle, argv[1]->value.arr->data, 
				    start, size);

  }

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(status);
}
/*======================================================================
 * NXputattr
 * status = NXputattr (file_id, attr_name, value, length, type)
 *=======================================================================*/

static IDL_VPTR NXputattr_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int length, attr_type;
  long value_long;
  char* attr_name;
  char* attrtype;

  void *value;
  if (argc != 5) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Five arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);
  IDL_ENSURE_SIMPLE(argv[2]);
/*  IDL_ENSURE_SCALAR(argv[3]); */

   attr_name = IDL_VarGetString(argv[1]);
   length = IDL_LongScalar(argv[3]);

   fileid = (int) IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

   hHandle = HHGetPointer(fileid);


  attr_type = datatypecheck(argv[4]);
  if(attr_type == -1) return IDL_GettmpInt(NX_ERROR);

  NXMSetError(NULL,idlError);

    if ((attr_type == NX_CHAR) && (argv[2]->type == IDL_TYP_STRING)) {
		attr_type = NX_CHAR;
		IDL_ENSURE_STRING(argv[2]);
		value = IDL_VarGetString(argv[2]);
		status = NXputattr (hHandle, attr_name, value, length, attr_type);

    } else if ((attr_type == NX_INT8) && (argv[2]->type == IDL_TYP_INT)) {
		short *value_short;
		attr_type = NX_INT8;
		IDL_ENSURE_SCALAR(argv[2]);
		value_short = (short *) IDL_LongScalar(argv[2]);
		status = NXputattr (hHandle, attr_name, &value_short, length, attr_type);

    } else if ((attr_type == NX_UINT8) && (argv[2]->type == IDL_TYP_BYTE)) {
    	UCHAR *value_uchar;
		attr_type = NX_UINT8;
       	IDL_ENSURE_SCALAR(argv[2]);
		value_uchar = (UCHAR *) IDL_LongScalar(argv[2]);
		status = NXputattr (hHandle, attr_name, &value_uchar, length, attr_type);

    } else if ((attr_type == NX_INT16) && (argv[2]->type == IDL_TYP_INT)) {
		IDL_INT *value_short;
		attr_type = NX_INT16;
		IDL_ENSURE_SCALAR(argv[2]);
		value_short = (IDL_INT *) IDL_LongScalar(argv[2]);
		status = NXputattr (hHandle, attr_name, &value_short, length, attr_type);

    } else if ((attr_type == NX_UINT16) && (argv[2]->type == IDL_TYP_UINT)) {
    	IDL_UINT *value_uint;
		attr_type = NX_UINT16;
       	IDL_ENSURE_SCALAR(argv[2]);
 		value_uint = (IDL_UINT *) IDL_LongScalar(argv[2]);
 		status = NXputattr (hHandle, attr_name, &value_uint, length, attr_type);

    } else if ((attr_type == NX_INT32) && (argv[2]->type == IDL_TYP_LONG)) {
		IDL_LONG *value_int;
		attr_type = NX_INT32;
		IDL_ENSURE_SCALAR(argv[2]);
		value_int = (IDL_LONG *)IDL_LongScalar(argv[2]);
		status = NXputattr (hHandle, attr_name, &value_int, length, attr_type);

    } else if ((attr_type == NX_UINT32) && (argv[2]->type == IDL_TYP_ULONG)) {
		IDL_ULONG *value_uint;
		attr_type = NX_UINT32;
		IDL_ENSURE_SCALAR(argv[2]);
		value_uint = (IDL_ULONG *) IDL_LongScalar(argv[2]);
		status = NXputattr (hHandle, attr_name, &value_uint, length, attr_type);

    } else if ((attr_type == NX_FLOAT32) && (argv[2]->type == IDL_TYP_FLOAT)) {
		float value_float;
		attr_type = NX_FLOAT32;
     	IDL_ENSURE_SCALAR(argv[2]);
		value_float =  IDL_DoubleScalar(argv[2]);
		status = NXputattr (hHandle, attr_name, &value_float, length, attr_type);

    } else if ((attr_type == NX_FLOAT64) && (argv[2]->type == IDL_TYP_DOUBLE)) {
		double value_double;
		attr_type = NX_FLOAT64;
		IDL_ENSURE_SCALAR(argv[2]);
		value_double =  IDL_DoubleScalar(argv[2]);
		status = NXputattr (hHandle, attr_name, &value_double, length, attr_type);

    } else {

    char statusBuffer[100];
	sprintf(statusBuffer,"Attribute type type doesn't match with the NeXus type: %i", attr_type );
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, statusBuffer);
	return IDL_GettmpInt(NX_ERROR);
    }

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXgetattr
 * status = NXgetattr (file_id, attr_name, value, length, type)
 *=======================================================================*/

static IDL_VPTR NXgetattr_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status = 1;
  int length, attr_type;
  IDL_LONG dims[IDL_MAX_ARRAY_DIM];
  IDL_VPTR r_value = 0, r_lenght;
  char* attr_name;
  char* attrtype;
  char* value_str;
  void *value;

  if (argc != 5) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Five arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}
  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);
  IDL_ENSURE_SIMPLE(argv[3]);
  IDL_ENSURE_SCALAR(argv[3]);

  if (argv[3]->type != IDL_TYP_LONG) {
  	if(argv[3]->type != IDL_TYP_INT) {
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Long or Int type argument required in lenght");
		return IDL_GettmpInt(NX_ERROR);
  		}
  }

/*  IDL_ENSURE_STRING(argv[4]); */
  IDL_ENSURE_SCALAR(argv[4]);

   attr_name = IDL_VarGetString(argv[1]);

  attr_type = datatypecheck(argv[4]);

  if(attr_type == -1) return IDL_GettmpInt(NX_ERROR);

  fileid = (int) IDL_LongScalar(argv[0]);
  length = (int ) IDL_LongScalar(argv[3]);


  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

   if(attr_type == NX_CHAR) {

		length++; /* Add space for string termination */
		NXmalloc((void**)&value_str, 1, &length, attr_type);
  		status = NXgetattr (hHandle, attr_name, value_str, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_StrToSTRING(value_str);
		NXfree((void**)&value_str);
	}
   else if(attr_type == NX_INT8) {
		IDL_INT value_int;
		/* There is no equivalent of this datatype in the IDL side
		   returning type Int */

		status = NXgetattr (hHandle, attr_name, &value_int, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_GettmpUInt(value_int);
	}
   else if(attr_type == NX_UINT8) {
   		UCHAR value_uchar;
		status = NXgetattr (hHandle, attr_name, &value_uchar, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		/* Why a warning?
		 * Gives a warning in gcc and doesn't compile in ms. visual c
		 * r_value = IDL_GettmpUChar(value_uchar); Return type int.
		 */

		r_value = IDL_GettmpInt(value_uchar);
	}
   else if(attr_type == NX_INT16) {
   		IDL_INT value_int;
		status = NXgetattr (hHandle, attr_name, &value_int, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_GettmpInt(value_int);
	}
   else if(attr_type == NX_UINT16) {
   		IDL_UINT value_uint;
		status = NXgetattr (hHandle, attr_name, &value_uint, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_GettmpUInt(value_uint);
	}
   else if(attr_type == NX_INT32) {
   		IDL_LONG value_long;
		status = NXgetattr (hHandle, attr_name, &value_long, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_GettmpLong(value_long);
	}
   else if(attr_type == NX_UINT32) {
   		IDL_ULONG value_ulong;
		status = NXgetattr (hHandle, attr_name, &value_ulong, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_GettmpULong(value_ulong);
	}
   else if(attr_type == NX_FLOAT32) {
   		float value_float;
		status = NXgetattr (hHandle, attr_name, &value_float, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_Gettmp();
		r_value->type = IDL_TYP_FLOAT;
		r_value->value.f = value_float;

	}
   else if(attr_type == NX_FLOAT64) {
   		double value_double;
		status = NXgetattr (hHandle, attr_name,  &value_double, &length, &attr_type);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    }
		r_value = IDL_Gettmp();
		r_value->type = IDL_TYP_DOUBLE;
		r_value->value.d = value_double;
	}


  r_lenght = IDL_GettmpInt((IDL_INT)length);
  IDL_VarCopy(r_value, argv[2]);
  IDL_VarCopy(r_lenght, argv[3]);

  return IDL_GettmpInt(status);
}

void releaseMemory( UCHAR * memPtr)
{
//	free(memPtr);
}

/*======================================================================
 * NXgetinfo
 * status =  (file_id, rank, dimensions[], data_type)
 *
 *=======================================================================*/

static IDL_VPTR NXgetinfo_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR r_rank, r_datatype, array;
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status, n;
  int rank,  datatype;
  int dimension[IDL_MAX_ARRAY_DIM];
  NXlink data_id;
  IDL_MEMINT dims[] = {IDL_MAX_ARRAY_DIM};
  IDL_INT *data_area_pointer;


  if (argc != 4) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Four arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

/*  status = NXgetdataID (hHandle, &data_id);
  if(status == NX_ERROR) {
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dataset must be opened before nxinfo");
		return IDL_GettmpInt(NX_ERROR);
	  } */

  NXMSetError(NULL,idlError);

  status = NXgetinfo (hHandle, &rank, dimension, &datatype);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  r_datatype = IDL_GettmpInt((IDL_INT)datatype);
  r_rank = IDL_GettmpInt((IDL_INT)rank);

/*IDL_MakeTempArray returns the data area pointer as its value. Last argument 'array' is the IDL_VPTR of array */

  data_area_pointer = (IDL_INT *)IDL_MakeTempArray((int)IDL_TYP_INT, 1, dims, IDL_ARR_INI_ZERO, &array);

  /* Dimensions are reversed to switch from C's Column major order to IDL row major order */
  for(n = 0; n < rank; n++) {
	data_area_pointer[n] = dimension[(rank-1)-n];
	}

//data_area_pointer[0] = (IDL_INT)55;
//*data_area_pointer = *number;
//  malloc(data_area_pointer, array->value.arr->arr_len);

//  memcpy(data_area_pointer, dimension, sizeof(int));

  // Output arguments
  IDL_VarCopy(r_rank, argv[1]);
  IDL_VarCopy(array, argv[2]);
  IDL_VarCopy(r_datatype, argv[3]);
//  releaseMemory(number);

  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXgetgroupinfo
 * status = NXgetgroupinfo (file_id, item_number, group_name, group_class)
 *=======================================================================*/

static IDL_VPTR NXgetgroupinfo_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR IDL_group_name, IDL_group_class, IDL_item_number;
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status;
  int item_number;

/* Of the top of my head: 100.. What is the actual maximum lenght*/
  char group_name[100];
  char group_class[100];

  if (argc != 4) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Four arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXgetgroupinfo(hHandle, &item_number, group_name, group_class);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  IDL_item_number = IDL_GettmpInt((IDL_INT)item_number);

  IDL_group_name = IDL_StrToSTRING(group_name);
  IDL_group_class = IDL_StrToSTRING(group_class);

  /* Output arguments */
  IDL_VarCopy(IDL_item_number, argv[1]);
  IDL_VarCopy(IDL_group_name, argv[2]);
  IDL_VarCopy(IDL_group_class, argv[3]);

  return IDL_GettmpInt(status);
}
/*======================================================================
 * NXinitgroupdir
 * status = NXinitgroupdir (file_id)
 * Not tested at all yet..
 *=======================================================================*/

static IDL_VPTR NXinitgroupdir_this(int argc, IDL_VPTR *argv)
{
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status;

  if (argc != 1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "One argument expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXinitgroupdir(hHandle);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXgetnextentry
 * status = NXgetnextentry (file_id, name, class, data_type)
 *=======================================================================*/

static IDL_VPTR NXgetnextentry_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR IDL_group_name, IDL_group_class, IDL_data_type;
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status;

/* Too long or long enough*/
  char group_name[255];
  char group_class[255];
  int data_type = 0;

  if (argc != 4) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Four arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);


  NXMSetError(NULL,idlError);

  status = NXgetnextentry(hHandle, group_name, group_class, &data_type);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  IDL_data_type = IDL_GettmpInt((IDL_INT)data_type);

  IDL_group_name = IDL_StrToSTRING(group_name);
  IDL_group_class = IDL_StrToSTRING(group_class);

  /* Output arguments*/
  IDL_VarCopy(IDL_group_name, argv[1]);
  IDL_VarCopy(IDL_group_class, argv[2]);
  IDL_VarCopy(IDL_data_type, argv[3]);

  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXgetattrinfo
 * status = NXgetattrinfo (file_id, attr_number)
 *=======================================================================*/

static IDL_VPTR NXgetattrinfo_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR IDL_attr_number;
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status;
  int attr_number = 0;

  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXgetattrinfo (hHandle, &attr_number);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  IDL_attr_number = IDL_GettmpInt((IDL_INT)attr_number);

  /* Output arguments*/
  IDL_VarCopy(IDL_attr_number, argv[1]);

  return IDL_GettmpInt(status);
}
/*======================================================================
 * NXinitattrdir
 * status = NXinitattrdir (file_id)
 *=======================================================================*/

static IDL_VPTR NXinitattrdir_this(int argc, IDL_VPTR *argv)
{
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status;

  if (argc != 1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "One arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);


  NXMSetError(NULL,idlError);

  status = NXinitattrdir (hHandle);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXgetnextattr
 * status = NXgetnextattr (file_id, attr_name, length, attr_type)
 *=======================================================================*/

static IDL_VPTR NXgetnextattr_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR IDL_attr_name = NULL, IDL_length= NULL, IDL_attr_type = NULL;
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status;

/* Too long or long enough*/
  char attr_name[150];
  int length = 0;
  int attr_type = 0;

  if (argc != 4) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Four arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXgetnextattr (hHandle, attr_name, &length, &attr_type);

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }
  if(status == NX_EOD) {
  	  IDL_attr_name = IDL_StrToSTRING("");
	  IDL_length = IDL_GettmpInt((IDL_INT)0);
	  IDL_attr_type = IDL_GettmpInt((IDL_INT)0);

  }

  if(status == NX_OK) {
  IDL_attr_name = IDL_StrToSTRING(attr_name);
  IDL_length = IDL_GettmpInt((IDL_INT)length);
  IDL_attr_type = IDL_GettmpInt((IDL_INT)attr_type);
  }

  /* Output arguments*/
  IDL_VarCopy(IDL_attr_name, argv[1]);
  IDL_VarCopy(IDL_length, argv[2]);
  IDL_VarCopy(IDL_attr_type, argv[3]);
  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXgetgroupID
 * statusstatus = NXgetgroupID (file_id, group_id)
 *======================================================================*/
static IDL_VPTR NXgetgroupID_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR result;
  int status, iHandle, iLink;
  NXaccess am;
  int fileid;
  NXhandle hHandle;
  NXlink *group_id = NULL;

  /* If number of arguments is not 2 return status NX_ERROR */
  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

  group_id = (NXlink *)malloc(sizeof(NXlink));
  if(group_id == NULL){
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unable to allocate memory for the group id link file");
  	return IDL_GettmpInt(NX_ERROR);
  }

  NXMSetError(NULL,idlError);

  status = NXgetgroupID (hHandle, group_id);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  iLink = HHMakeLinkHandle(group_id, fileid);
     if(iLink == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Error creating the link handle");
		return IDL_GettmpInt(NX_ERROR);
	}

  result = IDL_GettmpInt(iLink);

  // Output argument
  IDL_VarCopy(result, argv[1]);

  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXgetdataID
 * status = NXgetdataID (file_id, data_id)
 *======================================================================*/
static IDL_VPTR NXgetdataID_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR result;
  int status, iHandle, iLink;
  NXaccess am;
  int fileid;
  NXhandle hHandle;
  NXlink *data_id = NULL;

  /* If number of arguments is not 2 return status NX_ERROR */
  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

  data_id = (NXlink *)malloc(sizeof(NXlink));
  if(data_id == NULL){
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unable to allocate memory for the data id link file");
  	return IDL_GettmpInt(NX_ERROR);
  }

  NXMSetError(NULL,idlError);

  status = NXgetdataID (hHandle, data_id);

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  iLink = HHMakeLinkHandle(data_id, fileid);

    if(iLink == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Error creating the link handle");
		return IDL_GettmpInt(NX_ERROR);
	}

  result = IDL_GettmpInt(iLink);

  // Output argument
  IDL_VarCopy(result, argv[1]);

  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXgetdata
 * status = NXgetdata (file_id, data)
 *=======================================================================*/

static IDL_VPTR NXgetdata_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR data_array;
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int rank,  datatype, n;
/*int dimension[IDL_MAX_ARRAY_DIM]; */
/*  int dimension[8]; */
  IDL_ARRAY_DIM dimension;
  int data_area_pointer[NX_MAXRANK];
  void *data_buffer = NULL;
  char *char_buffer;
  NXlink data_id;


  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int ) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}


  hHandle = HHGetPointer(fileid);

/*  status = NXgetdataID (hHandle, &data_id);
  if(status == NX_ERROR) {
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dataset must be opened before nxputdata");
		return IDL_GettmpInt(NX_ERROR);
		} */

  NXMSetError(NULL,idlError);

  /* First get the rank, dimension and datatype */
  status = NXgetinfo (hHandle, &rank, data_area_pointer, &datatype);
  if(status == NX_ERROR) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
	}
  /* Dimensions are reversed to switch from C's Column major order to IDL row major order */
  for(n = 0; n < rank; n++) {
	dimension[n] = data_area_pointer[rank - 1 -n];
	}

/* Check data type and prepare a Array or a String for NeXuS API nxgetdata function*/
   switch (datatype) {
	case NX_CHAR:
		/*IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXCHAR");*/

	break;
	case NX_FLOAT32:
		/*IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXFLOAT32");*/
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_FLOAT, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_FLOAT64:
		/*IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXFLOAT64");*/
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_DOUBLE, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_INT8:
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "This data type is not supported in IDL");
//		return IDL_GettmpInt(NX_ERROR);
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_BYTE, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
		/* This data type is not supported in IDL, A dangerous conversion to IDL_TYP_INT? */
	break;
	case NX_UINT8:
		/*	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXUINT8"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_BYTE, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_INT16:
		/*IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXINT16"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_INT, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_UINT16:
		/*IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXINT16"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_UINT, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_INT32:
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXINT32");
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_LONG, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_UINT32:
		/*IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXUINT32");*/
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_ULONG, rank, dimension, IDL_ARR_INI_ZERO, &data_array);
	break;
	default:
		/*IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Data is of an unknown datatype");*/
		return IDL_GettmpInt(NX_ERROR);
	break;
	}

  NXMSetError(NULL,idlError);

if(datatype == NX_CHAR) {
 	status =  NXmalloc ((void **) &char_buffer, rank, data_area_pointer, 
			    datatype);
        if(status == NX_ERROR){
 		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
		return IDL_GettmpInt(NX_ERROR);
    		}

	status = NXgetdata (hHandle, char_buffer);
        if(status == NX_ERROR){
 		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
		return IDL_GettmpInt(NX_ERROR);
		}
	data_array = IDL_StrToSTRING(char_buffer);
	IDL_VarCopy(data_array, argv[1]);
	NXfree((void **) &char_buffer);
   }
else {

	status = NXgetdata (hHandle, data_buffer);
        if(status == NX_ERROR){
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
		return IDL_GettmpInt(NX_ERROR);
	}

   IDL_VarCopy(data_array, argv[1]);

   }

/* For temporary variables VarCopy copies the dynamic part to destination, not the actual data.
 * So not really slower and a lot safer than writing the data straight to argv[1].
 *
 *External Development Guide: IDL Internals: Variables
 * Copying Variables:
 *If the source is a temporary variable, IDL_VarCopy() does not make a duplicate of the dynamic
 *part for the destination. Instead, the dynamic part of the source is given to the destination,
 *and the source variable itself is returned to the pool of free temporary variables. This is the
 *equivalent of freeing the temporary variable. Therefore, the variable must not be used any
 *further and the caller should not explicitly free the variable. This optimization significantly
 *improves resource utilization and performance because this special case occurs frequently.
 */

  return IDL_GettmpInt(status);
}
/*======================================================================
 * NXgetslab
 * status = NXgetslab (file_id, data, start[], size[])
 *=======================================================================*/

static IDL_VPTR NXgetslab_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR data_array;
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int rank,  datatype, n;
  IDL_MEMINT start[NX_MAXRANK], size[NX_MAXRANK], dim[NX_MAXRANK];
  int istart[NX_MAXRANK], isize[NX_MAXRANK];
  IDL_LONG *stDim, *szDim;
  IDL_ARRAY_DIM dimension;
  void *data_buffer = NULL;


  if (argc != 4) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Four arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_ARRAY(argv[2]);
  IDL_ENSURE_ARRAY(argv[3]);

  /*Check that the arrays are type long */

  if(argv[2]->type != IDL_TYP_LONG) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Argument 3, slap start, Array must be type long");
	return IDL_GettmpInt(NX_ERROR);
  }

  if(argv[3]->type != IDL_TYP_LONG) {
  	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Argument 4, slap size: Array must be type long");
	return IDL_GettmpInt(NX_ERROR);
  }

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

/*  status = NXgetdataID (hHandle, &data_id);
  if(status == NX_ERROR) {
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Dataset must be opened before nxputdata");
		return IDL_GettmpInt(NX_ERROR);
	      } */

  /* First get the rank, dimension and datatype */
  status = NXgetinfo (hHandle, &rank, dim, &datatype);
  if(status == NX_ERROR) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
	}

  /* swap dimensions again */
  stDim = (IDL_LONG *)argv[2]->value.arr->data;
  szDim = (IDL_LONG *)argv[3]->value.arr->data;
  for(n = 0; n < rank; n++){
    start[n] = stDim[rank -n  -1];
    istart[n] = stDim[rank -n  -1];
    size[n] = szDim[rank -n  -1];
    isize[n] = szDim[rank -n  -1];
  }


/* Check data type and prepare a Array or a String for NeXuS API nxgetdata function*/
   switch (datatype) {
	case NX_CHAR:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXCHAR"); */

	break;
	case NX_FLOAT32:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXFLOAT32"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_FLOAT, rank, (void *)size, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_FLOAT64:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXFLOAT64"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_DOUBLE, rank, (void *) size, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_INT8:
		/* This data type is not supported in IDL, A dangerous conversion to IDL_TYP_INT? */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_BYTE, rank, (void *)size, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_UINT8:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXUINT8"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_BYTE, rank, (void *)size, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_INT16:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXINT16"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_INT, rank, (void *)size, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_UINT16:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXUINT16"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_UINT, rank, (void *)size, IDL_ARR_INI_ZERO, &data_array);
	break;
	case NX_INT32:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXINT32"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_LONG, rank, (void *)size, IDL_ARR_INI_ZERO, &data_array);

	break;
	case NX_UINT32:
		/* IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NXUINT32"); */
		data_buffer = (void *)IDL_MakeTempArray(IDL_TYP_ULONG, rank, (void *)size, IDL_ARR_INI_ZERO, &data_array);
	break;
	default:
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Data in the NeXus file is of an unknown datatype");
		return IDL_GettmpInt(NX_ERROR);
	break;
	}

  NXMSetError(NULL,idlError);

   if(datatype == NX_CHAR) {
	/* Api doesnt support getslab for textdata!
	 *
	 * Currently The C Api returns the correct string slab put mixes something up badly
	 * and program crashes at some point.
	 * */

	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "NeXus IDL API doesn't support textdata for getslab ");
	return IDL_GettmpInt(NX_ERROR);

/*	char* char_buffer;

	status =  NXmalloc ((void **) &char_buffer, rank, argv[3]->value.arr->data, datatype);
    if(status == NX_ERROR){
 		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
		return IDL_GettmpInt(NX_ERROR);
    	}

	status = NXgetslab (hHandle, char_buffer, (int *) argv[2]->value.arr->data, (int *) argv[3]->value.arr->data);
	 if(status == NX_ERROR){
 		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
		return IDL_GettmpInt(NX_ERROR);
    	}

	IDL_char_data = IDL_StrToSTRING(char_buffer);
	IDL_VarCopy(IDL_char_data, argv[1]);
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Debug 3 zz");

    if(status == NX_ERROR){
 		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
		return IDL_GettmpInt(NX_ERROR);
    	}
*/
     }

  else {
	  status = NXgetslab (hHandle, data_buffer, istart, isize);
	  if(status == NX_ERROR)
		{
		IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
		return IDL_GettmpInt(NX_ERROR);
    	}
   	  IDL_VarCopy(data_array, argv[1]);
   }

  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXflush
 * status = NXflush (file_id)
 *======================================================================*/
static IDL_VPTR NXflush_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR result;
  int status, iHandle;
  NXaccess am;
  int fileid;
  NXhandle hHandle;

  /* If number of arguments is not 1 return status NX_ERROR */
  if (argc != 1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "One argument expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);


  NXMSetError(NULL,idlError);

  status = NXflush (&hHandle);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

/* We don't loose the links with HHChangeHandle */
  HHChangeHandle(hHandle, fileid);

  result = IDL_GettmpLong(fileid);

  // Output argument
  IDL_VarCopy(result, argv[0]);

  return IDL_GettmpInt(status);
}


/*======================================================================
 * Nxinquirefile
 * status = NXinquirefile(handle,filename, filenameLength)
 *=======================================================================*/

static IDL_VPTR NXinquirefile_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR IDL_filename;
  IDL_INT fileid;
  NXhandle hHandle;
  int iHandle, status;

/* Max file name lenght in mac os, unix, linux, windows */
  char filename[254];
  int filenameLength;

  if (argc != 3) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_SCALAR(argv[2]);

  fileid = (int) IDL_LongScalar(argv[0]);
  filenameLength =  IDL_LongScalar(argv[2]);

  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXinquirefile(hHandle, filename, filenameLength);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  IDL_filename = IDL_StrToSTRING(filename);

  /* Output arguments*/
  IDL_VarCopy(IDL_filename, argv[1]);

  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXmakelink
 * status = NXmakelink (file_id, link)
 *======================================================================*/
static IDL_VPTR NXmakelink_this(int argc, IDL_VPTR *argv)
{
  int status, iHandle, iLink;
  NXaccess am;
  int fileid, linkid;
  NXhandle hHandle;
  NXlink *link;

  /* If number of arguments is not 2 return status NX_ERROR */
  if (argc != 2) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Two arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);

  fileid = (int) IDL_LongScalar(argv[0]);

  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  linkid = (int) IDL_LongScalar(argv[1]);

  if(HHCheckIfHandleExists(linkid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown link id");
	return IDL_GettmpInt(NX_ERROR);
	}
  link = (NXlink *)HHGetPointer(linkid);

  NXMSetError(NULL,idlError);

  status = NXmakelink (hHandle, link);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

/* Commented, No output arguments in makelink

  iLink = HHMakeHandle(link);

  result = IDL_GettmpInt(iLink);

  // Output argument
  IDL_VarCopy(result, argv[1]); */

  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXmakenamedlink
 * status = NXmakenamedlink (file_id, nameoflink, link)
 *======================================================================*/
static IDL_VPTR NXmakenamedlink_this(int argc, IDL_VPTR *argv)
{

  int status, iHandle, iLink;
  NXaccess am;
  int fileid, linkid;
  NXhandle hHandle;
  NXlink *link;
  char* nameoflink;

  /* If number of arguments is not 3 return status NX_ERROR */
  if (argc != 3) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);
  IDL_ENSURE_SCALAR(argv[2]);
  fileid = (int) IDL_LongScalar(argv[0]);
  nameoflink = IDL_VarGetString(argv[1]);
  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);

  linkid = (int) IDL_LongScalar(argv[2]);

  if(HHCheckIfHandleExists(linkid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown link id");
	return IDL_GettmpInt(NX_ERROR);
	}
  link = (NXlink *)HHGetPointer(linkid);

  NXMSetError(NULL,idlError);

  status = NXmakenamedlink (hHandle, nameoflink, link);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXlinkexternal
 * status = NXlinkexternal(handle, name, nxclass, nxurl)
 *=======================================================================*/

static IDL_VPTR NXlinkexternal_this(int argc, IDL_VPTR *argv)
{
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  char* name;
  char* nxclass;
  char* nxurl;

  if (argc != 4) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);
  IDL_ENSURE_STRING(argv[2]);
  IDL_ENSURE_STRING(argv[3]);

  fileid = (int)IDL_LongScalar(argv[0]);
  name = IDL_VarGetString(argv[1]);
  nxclass = IDL_VarGetString(argv[2]);
  nxurl = IDL_VarGetString(argv[3]);


  if(HHCheckIfHandleExists(fileid) == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXlinkexternal(hHandle, name, nxclass, nxurl);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }


  return IDL_GettmpInt(status);
}


/*======================================================================
 * NXisexternalgroup
 * status = NXisexternalgroup(handle,name, nxclass, nxurl, nxurllen)
 *=======================================================================*/

static IDL_VPTR NXisexternalgroup_this(int argc, IDL_VPTR *argv)
{
  IDL_VPTR IDL_NXurl;
  int fileid;
  NXhandle hHandle;
  int iHandle, status;
  int nxurllen;
  char* name;
  char* nxclass;
  char* nxurl;
  if (argc != 5) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Five arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRING(argv[1]);
  IDL_ENSURE_STRING(argv[2]);
  IDL_ENSURE_SCALAR(argv[4]);

  fileid = (int )IDL_LongScalar(argv[0]);
  name = IDL_VarGetString(argv[1]);
  nxclass = IDL_VarGetString(argv[2]);
  nxurllen =  IDL_LongScalar(argv[4]);

		nxurllen++; /* Add space for string termination */
		status = NXmalloc((void**)&nxurl, 1, &nxurllen, NX_CHAR);
  		if(status == NX_ERROR){
 			IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
			return IDL_GettmpInt(NX_ERROR);
	    	}




  if(HHCheckIfHandleExists(fileid) == -1) {
        IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}

  hHandle = HHGetPointer(fileid);

  NXMSetError(NULL,idlError);

  status = NXisexternalgroup(hHandle, name, nxclass, nxurl, nxurllen);

  NXfree((void**)&nxurl);

  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
 	IDL_NXurl = IDL_StrToSTRING("");
 	IDL_VarCopy(IDL_NXurl, argv[3]);
	return IDL_GettmpInt(NX_ERROR);
  }

  IDL_NXurl = IDL_StrToSTRING(nxurl);

  /* Output arguments*/
  IDL_VarCopy(IDL_NXurl, argv[3]);

  return IDL_GettmpInt(status);
}

/*======================================================================
 * NXsameID
 * status = NXsameID (file_id, link1, link2)
 *======================================================================*/
static IDL_VPTR NXsameID_this(int argc, IDL_VPTR *argv)
{


  int status, iHandle, iLink;
  NXaccess am;
  int fileid, linkid_one, linkid_two;
  NXhandle hHandle;
  NXlink *link_one, *link_two;

  if (argc != 3) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Three arguments expected");
	return IDL_GettmpInt(NX_ERROR);
	}

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_SCALAR(argv[1]);
  IDL_ENSURE_SCALAR(argv[2]);

  fileid = (int ) IDL_LongScalar(argv[0]);
  linkid_one = (int ) IDL_LongScalar(argv[1]);
  linkid_two = (int ) IDL_LongScalar(argv[2]);

  if(HHCheckIfHandleExists(fileid) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Unknown file id");
	return IDL_GettmpInt(NX_ERROR);
	}
  hHandle = HHGetPointer(fileid);


  if(HHCheckIfHandleExists(linkid_one) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Argument 2, Unknown link id");
	return IDL_GettmpInt(NX_ERROR);
	}

  link_one = (NXlink *)HHGetPointer(linkid_one);

  if(HHCheckIfHandleExists(linkid_two) == -1) {
	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, "Argument 3, Unknown link id");
	return IDL_GettmpInt(NX_ERROR);
	}

  link_two = (NXlink *)HHGetPointer(linkid_two);

  NXMSetError(NULL,idlError);

  status = NXsameID (hHandle, link_one, link_two);
  if(status == NX_ERROR){
 	IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_INFO, nexusError);
	return IDL_GettmpInt(NX_ERROR);
  }

  return IDL_GettmpInt(1);

}


int IDL_Load(void)
{
  /*
   * These tables contain information on the functions and procedures
   * that make up the NeXusIDL-API DLM. The information contained in these
   * tables must be identical to that contained in NeXusIDL-API.dlm.
   */
  static IDL_SYSFUN_DEF2 function_addr[] = {
    { NXopen_this, "NXOPEN", 0, IDL_MAXPARAMS, 0, 0},
    { NXclose_this, "NXCLOSE", 0, IDL_MAXPARAMS, 0, 0},
    { NXsetnumberformat_this, "NXSETNUMBERFORMAT", 0, IDL_MAXPARAMS, 0, 0},
    { NXopenpath_this, "NXOPENPATH", 0, IDL_MAXPARAMS, 0, 0},
    { NXopengrouppath_this, "NXOPENGROUPPATH", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetpath_this, "NXGETPATH", 0, IDL_MAXPARAMS, 0, 0},
    { NXmakegroup_this, "NXMAKEGROUP", 0, IDL_MAXPARAMS, 0, 0},
    { NXopengroup_this, "NXOPENGROUP", 0, IDL_MAXPARAMS, 0, 0},
	{ NXcompress_this, "NXCOMPRESS", 0, IDL_MAXPARAMS, 0, 0},
    { NXclosegroup_this, "NXCLOSEGROUP", 0, IDL_MAXPARAMS, 0, 0},
    { NXmakedata_this, "NXMAKEDATA", 0, IDL_MAXPARAMS, 0, 0},
    { NXcompmakedata_this, "NXCOMPMAKEDATA", 0, IDL_MAXPARAMS, 0, 0},
    { NXputdata_this, "NXPUTDATA", 0, IDL_MAXPARAMS, 0, 0},
    { NXputslab_this, "NXPUTSLAB", 0, IDL_MAXPARAMS, 0, 0},
    { NXputattr_this, "NXPUTATTR", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetattr_this, "NXGETATTR", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetinfo_this, "NXGETINFO", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetgroupinfo_this, "NXGETGROUPINFO", 0, IDL_MAXPARAMS, 0, 0},
    { NXinitgroupdir_this, "NXINITGROUPDIR", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetnextentry_this, "NXGETNEXTENTRY", 0, IDL_MAXPARAMS, 0, 0},
    { NXinitattrdir_this, "NXINITATTRDIR", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetattrinfo_this, "NXGETATTRINFO", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetnextattr_this, "NXGETNEXTATTR", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetgroupID_this, "NXGETGROUPID", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetdataID_this, "NXGETDATAID", 0, IDL_MAXPARAMS, 0, 0},
    { NXopendata_this, "NXOPENDATA", 0, IDL_MAXPARAMS, 0, 0},
    { NXclosedata_this, "NXCLOSEDATA", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetdata_this, "NXGETDATA", 0, IDL_MAXPARAMS, 0, 0},
    { NXgetslab_this, "NXGETSLAB", 0, IDL_MAXPARAMS, 0, 0},
    { NXflush_this, "NXFLUSH", 0, IDL_MAXPARAMS, 0, 0},
    { NXinquirefile_this, "NXINQUIREFILE", 0, IDL_MAXPARAMS, 0, 0},
    { NXmakelink_this, "NXMAKELINK", 0, IDL_MAXPARAMS, 0, 0},
    { NXmakenamedlink_this, "NXMAKENAMEDLINK", 0, IDL_MAXPARAMS, 0, 0},
    { NXlinkexternal_this, "NXLINKEXTERNAL", 0, IDL_MAXPARAMS, 0, 0},
    { NXisexternalgroup_this, "NXISEXTERNALGROUP", 0, IDL_MAXPARAMS, 0, 0},
    { NXsameID_this, "NXSAMEID", 0, IDL_MAXPARAMS, 0, 0}
  };

  /* 
   * Create a message block to hold our messages. Save its handle where
   * the other routines can access it.
   */
  if (!(msg_block = IDL_MessageDefineBlock("NeXusIDL-API",
					   IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  /*
   * Register our routine. The routines must be specified exactly the same
   * as in NeXusIDL-API.dlm.
   */

   IDL_SysRtnAdd(function_addr, TRUE, IDL_CARRAY_ELTS(function_addr));

   return IDL_TRUE;
}
