/*
  This is a library of support functions and data structures which can be
  used in order to create interfaces between the NeXus-API and scripting
  languages or data analysis systems with a native code interface. 

  copyright: GPL

  Mark Koennecke, October 2002
  Mark Koennecke, November 2002
*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "nxinterhelper.h"
#include "nxdataset.h"
/*-----------------------------------------------------------------
  An own error handler. nx_getlasterror will return the test of
  the last NeXus error.
  --------------------------------------------------------------------*/
static char errorText[256]= "";

/*--------------------------------------------------------------------*/
pNXDS createNXDataset32(int rank, int typecode, int dim[])
{
  int64_t newDim[NX_MAXRANK];
  int i;

  for(i = 0; i < rank; i++){
    newDim[i] = dim[i];
  }
  return createNXDataset(rank,typecode, newDim);
}
/*--------------------------------------------------------------------*/
static void nxinterError(void *pData, char *error){
  strncpy(errorText,error,255);
}
/*-----------------------------------------------------------------------*/
char *nx_getlasterror(void){
  return strdup(errorText);
}
/*-------------------- opening and closing -------------------------------*/
void *nx_open(char *filename, int accessMethod){
  NXhandle handle = NULL;
  int status;

  NXMSetError(NULL,nxinterError);
  status = NXopen(filename,(NXaccess)accessMethod, &handle);
  if(status == NX_OK){
    return handle;
  }else{
    return NULL;
  }
}
/*------------------------------------------------------------------------*/
void *nx_flush(void *hundle){
  NXhandle handle;
  int status;

  handle = (NXhandle)hundle;
  status = NXflush(&handle);
  if(status == NX_OK){
    return handle;
  } else {
    return NULL;
  }
}
/*-----------------------------------------------------------------------*/
void nx_close(void *hundle){
  NXhandle handle;

  handle = (NXhandle)hundle;
  NXclose(&handle);
}
/*=================== group handling functions ========================*/
int nx_makegroup(void *handle, char *name, char *nxclass){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXmakegroup(hfil,name, nxclass);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------*/
int nx_opengroup(void *handle, char *name, char *nxclass){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXopengroup(hfil,name, nxclass);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------*/
int nx_openpath(void *handle, char *path){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXopenpath(hfil,path);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------*/
int nx_opengrouppath(void *handle, char *path){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXopengrouppath(hfil,path);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*--------------------------------------------------------------------*/
char *nx_getpath(void *handle){
  int status;
  NXhandle hfil;
  char path[1024];

  hfil = (NXhandle)handle;
  status = NXgetpath(hfil,path,1024);
  if(status == NX_OK){
    return strdup(path);
  } else {
    return strdup("Error in NXgetpath");
  }
}
/*--------------------------------------------------------------------*/
int nx_closegroup(void *handle){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXclosegroup(hfil);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*-------------------------------------------------------------------*/
char *nx_getnextentry(void *handle, char separator){
  int status, length, type;
  NXhandle hfil;
  char *resultBuffer = NULL;
  NXname group,nxclass;

  hfil = (NXhandle)handle;
  status = NXgetnextentry(hfil,group, nxclass,&type);
  if(status == NX_OK){
    length = 30 + strlen(group) + strlen(nxclass);
    /*
      This introduces a memory leak. I had hoped, that swig would
      kill it for me after use, but I'am afraid, this is not the
      case. Unfortately I do not know how to fix the issue.
    */
    resultBuffer = (char *)malloc(length*sizeof(char));
    if(resultBuffer == NULL){
      return NULL;
    }
    sprintf(resultBuffer,"%s%c%s%c%d",group,separator,nxclass,
	    separator,type);
    return resultBuffer;
  } else {
    return NULL;
  }
}
/*-------------------------------------------------------------------*/
void *nx_getgroupID(void *handle){
  int status;
  NXhandle hfil;
  NXlink *linki;

  linki = (NXlink *)malloc(sizeof(NXlink));
  if(linki == NULL){
    return NULL;
  }
  hfil = (NXhandle)handle;
  status = NXgetgroupID(hfil,linki);
  if(status == NX_OK){
    return linki;
  } else {
    return NULL;
  }
}
/*------------------------------------------------------------------*/
int  nx_initgroupdir(void *handle){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXinitgroupdir(hfil);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*========================== dataset handling =======================*/
int nx_makedata(void *ptr, char *name, int rank, int type, 
		    void *dimPtr){
  int status;
  NXhandle hfil;
  pNXDS dimData;

  hfil = (NXhandle)ptr;
  dimData = (pNXDS)dimPtr;
  if(dimData->type != NX_INT32){
    NXReportError("ERROR: dimension data not integer");
    return 0;
  }
  status = NXmakedata(hfil, name, type, rank, 
		      dimData->u.iPtr);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*--------------------------------------------------------------------*/
int nx_compmakedata(void *ptr, char *name, int rank, int type, 
		    void *dimPtr, void *bufPtr){
  int status;
  NXhandle hfil;
  pNXDS dimData, bufData;

  hfil = (NXhandle)ptr;
  dimData = (pNXDS)dimPtr;
  if(dimData->type != NX_INT32){
    NXReportError("ERROR: dimension data not integer");
    return 0;
  }
  bufData = (pNXDS)bufPtr;
  status = NXcompmakedata(hfil, name, type, rank, 
			  dimData->u.iPtr, NX_COMP_LZW,bufData->u.iPtr);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*----------------------------------------------------------------------*/
int nx_opendata(void *handle, char *name){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXopendata(hfil,name);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*----------------------------------------------------------------------*/
int nx_closedata(void *handle){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXclosedata(hfil);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*------------------------------------------------------------------------*/
int nx_putslab(void *handle, void *dataset, void *startDim){
  int status;
  NXhandle hfil;
  pNXDS data;
  pNXDS start;
  int  end[NX_MAXRANK], i;

  hfil = (NXhandle)handle;
  data = (pNXDS)dataset;
  start = (pNXDS)startDim;
  for(i = 0; i < data->rank;i++){
    end[i] = data->dim[i];
  }

  status = NXputslab(hfil,data->u.ptr,start->u.iPtr,end);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}
/*-----------------------------------------------------------------------*/
void *nx_getslab(void *handle, void *startdim, void *sizedim){
  pNXDS resultdata;
  pNXDS start, size;
  int status, rank, type, dim[NX_MAXRANK];
  NXhandle hfil;

  hfil = (NXhandle)handle;
  start = (pNXDS)startdim;
  size =   (pNXDS)sizedim;

  /*
    get info first, then allocate data
  */
  status = NXgetinfo(hfil, &rank,dim,&type);
  if(status != NX_OK){
    return NULL;
  }
  
  resultdata = createNXDataset32(rank,type,size->u.iPtr);
  if(resultdata == NULL){
    return NULL;
  }

  status = NXgetslab(hfil,resultdata->u.ptr,start->u.iPtr,
		     size->u.iPtr);
  if(status == NX_OK){
    return resultdata;
  }else{
    dropNXDataset(resultdata);
    return NULL;
  }
}
/*------------------------------------------------------------------------*/
void *nx_getds(void *handle, char *name){
  pNXDS result = NULL;
  int rank, type,dim[NX_MAXRANK],status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXopendata(hfil,name);
  if(status != NX_OK){
    return NULL;
  }

  status = NXgetinfo(hfil,&rank,dim,&type);
  if(status != NX_OK){
    return NULL;
  }

  result = createNXDataset32(rank,type,dim);
  if(result == NULL){
    NXclosedata(hfil);
    return NULL;
  }

  status = NXgetdata(hfil,result->u.ptr);
  if(result == NULL){
    NXclosedata(hfil);
    dropNXDataset(result);
    return NULL;
  }
  NXclosedata(hfil);
  return result;
}
/*----------------------------------------------------------------------*/
int   nx_putds(void *handle, char *name, void *dataset){
  NXhandle hfil;
  int status;
  pNXDS data;

  hfil = (NXhandle)handle;
  data = (pNXDS)dataset;

  status = NXopendata(hfil,name);
  if(status != NX_OK){
    status = NXmakedata64(hfil,name,data->type,data->rank,data->dim);
    if(status != NX_OK){
      return 0;
    }
    NXopendata(hfil,name);
  }

  status = NXputdata(hfil,data->u.ptr);
  NXclosedata(hfil);
  if(status != NX_OK){
    return 0;
  }else{
    return 1;
  }
}
/*------------------------------------------------------------------------*/
void *nx_getdata(void *handle){
  pNXDS result = NULL;
  int rank, type,dim[NX_MAXRANK],status;
  NXhandle hfil;


  hfil = (NXhandle)handle;
  status = NXgetinfo(hfil,&rank,dim,&type);
  if(status != NX_OK){
    return NULL;
  }

  result = createNXDataset32(rank,type,dim);
  if(result == NULL){
    NXclosedata(hfil);
    return NULL;
  }

  status = NXgetdata(hfil,result->u.ptr);
  if(result == NULL){
    dropNXDataset(result);
    return NULL;
  }
  return result;
}
/*----------------------------------------------------------------------*/
int   nx_putdata(void *handle, void *dataset){
  NXhandle hfil;
  int status;
  pNXDS data;

  hfil = (NXhandle)handle;
  data = (pNXDS)dataset;

  if(data == NULL){
    NXReportError("ERROR: NULL data pointer in nx_putdata");
    return 0;
  }

  status = NXputdata(hfil,data->u.ptr);
  if(status != NX_OK){
    return 0;
  }else{
    return 1;
  }
}
/*----------------------------------------------------------------------*/
void *nx_getinfo(void *handle){
  NXhandle hfil;
  int status, type, rank, dim[NX_MAXRANK], rdim[1], i;
  pNXDS data = NULL;

  hfil = (NXhandle)handle;

  status = NXgetinfo(handle,&rank,dim,&type);
  if(status != NX_OK){
    return NULL;
  }
  rdim[0] = 2 + rank;
  data = createNXDataset32(1,NX_INT32,rdim);
  data->u.iPtr[0] = type;
  data->u.iPtr[1] = rank;
  for(i = 0; i < rank; i++){
    data->u.iPtr[2+i] = dim[i];
  }
  return data;
}
/*----------------------------------------------------------------------*/
void *nx_getdataID(void *handle){
  NXhandle hfil;
  int status;
  NXlink *linki;

  linki = (NXlink *)malloc(sizeof(NXlink));
  if(linki == NULL){
    return NULL;
  }

  hfil = (NXhandle)handle;
  status = NXgetdataID(hfil,linki);
  if(status == NX_OK){
    return linki;
  } else {
    free(linki);
    return NULL;
  }
}
/*-------------------------------------------------------------------*/
char *nx_getnextattr(void *handle, char separator){
  int status, length, type;
  char *result;
  NXhandle hfil;
  NXname aName;

  hfil = (NXhandle)handle;
  status = NXgetnextattr(hfil,aName, &length, &type);
  if(status == NX_OK){
    /*
      This introduces a memory leak. I had hoped, that swig would
      kill it for me after use, but I'am afraid, this is not the
      case. Unfortately I do not know how to fix the issue.
    */
    result = (char *)malloc((20+strlen(aName))*sizeof(char));
    if(result == NULL){
      return NULL;
    }
    memset(result,0,(20+strlen(aName))*sizeof(char));
    sprintf(result,"%s%c%d%c%d", aName,separator,
	    length,separator,type);
    return result;
  } else {
    return NULL;
  }
}
/*-------------------------------------------------------------------*/
int   nx_putattr(void *handle, char *name, void *ds){
  int status;
  NXhandle hfil;
  pNXDS data;

  hfil = (NXhandle)handle;
  data = (pNXDS)ds;
  status = NXputattr(hfil,name,data->u.ptr,data->dim[0],data->type);
  if(status == NX_OK){
    return 1;
  }else{
    return 0;
  }
}
/*-------------------------------------------------------------------*/
void *nx_getattr(void *handle, char *name, int type, int length){
  NXhandle hfil;
  int status, tp, ll, dim[1];
  pNXDS data = NULL;


  hfil = (NXhandle)handle;

  /*
    prepare dataset
  */
  dim[0] = length+1;
  data = createNXDataset32(1,type,dim);
  if(data == NULL){
    return NULL;
  }
  
  /*
    finally read the real data
  */
  ll = length;
  tp = type;
  status = NXgetattr(hfil,name,data->u.ptr,&ll,&tp);
  if(status != NX_OK){
    dropNXDataset(data);
    return NULL;
  }

  return data;
}
/*-----------------------------------------------------------------------*/
int nx_makelink(void *handle, void *link){
  NXhandle hfil;
  NXlink* lk;
  int status;

  hfil = (NXhandle)handle;
  lk = (NXlink *)link;

  status = NXmakelink(hfil,lk);
  if(status == NX_OK){
    return 1;
  }else{
    return 0;
  }
}
/*-----------------------------------------------------------------------*/
int nx_makenamedlink(void *handle, char *name, void *link){
  NXhandle hfil;
  NXlink* lk;
  int status;

  hfil = (NXhandle)handle;
  lk = (NXlink *)link;

  status = NXmakenamedlink(hfil,name,lk);
  if(status == NX_OK){
    return 1;
  }else{
    return 0;
  }
}
/*-----------------------------------------------------------------------*/
int nx_opensourcegroup(void *handle){
  NXhandle hfil;
  int status;

  hfil = (NXhandle)handle;

  status = NXopensourcegroup(hfil);
  if(status == NX_OK){
    return 1;
  }else{
    return 0;
  }
}
/*-----------------------------------------------------------------------*/
char *nx_inquirefile(void *handle){
  int status;
  NXhandle hfil;
  char *pPtr = NULL;

  hfil = (NXhandle)handle;
  pPtr = malloc(1024*sizeof(char));
  status = NXinquirefile(hfil,pPtr, 1024);
  if(status == NX_OK){
    return pPtr;
  } else {
    return NULL;
  }
}
/*-----------------------------------------------------------------------*/
void *nx_isexternalgroup(void *handle, char *name, char *nxclass){
  int status, length = 1024;
  NXhandle hfil;
  char url[1024];

  hfil = (NXhandle)handle;
  status = NXisexternalgroup(hfil,name, nxclass,url,length);
  if(status == NX_OK){
    createTextNXDataset(url);
  } else {
    return NULL;
  }
  return NULL;/*NOTREACHED*/
}
/*-----------------------------------------------------------------------*/
int nx_linkexternal(void *handle, char *name, char *nxclass, 
		       char *url){
  int status;
  NXhandle hfil;

  hfil = (NXhandle)handle;
  status = NXlinkexternal(hfil,name, nxclass,url);
  if(status == NX_OK){
    return 1;
  } else {
    return 0;
  }
}



