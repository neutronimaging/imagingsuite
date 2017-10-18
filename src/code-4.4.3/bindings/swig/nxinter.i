/*
   Interface file for SWIG and the NeXus-API.

   copyright: GPL
  
   The criminal act of writing this code was initially commited by:
	
   Mark Koennecke in October 2002

   Updated: April 2006, Mark Koennecke
*/
%module nxinter
%{
#include "nxinterhelper.h"

%}
%title "NeXus-API Interface Fucntions"

/*-------------------------------- Constants ------------------------*/
%section "Constants"

/* Access Codes */
#define NXACC_READ    1
#define NXACC_RDWR    2
#define NXACC_CREATE  3
#define NXACC_CREATE4 4
#define NXACC_CREATE5 5
#define NXACC_CREATEXML 6

/* data types */
#define NX_FLOAT32   5
#define NX_FLOAT64   6
#define NX_INT8     20  
#define NX_UINT8    21
#define NX_BOOLEAN  21
#define NX_INT16    22  
#define NX_UINT16   23
#define NX_INT32    24
#define NX_UINT32   25
#define NX_INT64    26
#define NX_UINT64   27
#define NX_CHAR      4
/*--------------------------------------------------------------------*/
%include "nxdataset.i"
/*-------------------- error handling --------------------------------*/
extern char *nx_getlasterror(void);
/*------------------------ opening and closing --------------------------*/
%section "Opening and Closing NeXus Files"
extern void *nx_open(char *filename, int accessCode);
extern void *nx_flush(void *handle);
extern void  nx_close(void *handle);


%section "Navigating Groups"
extern int nx_makegroup(void *handle, char *name, char *nxclass);
extern int nx_opengroup(void *handle, char *name, char *nxclass);
extern int nx_openpath(void *handle, char *path);
extern int nx_opengrouppath(void *handle, char *path);
extern char *nx_getpath(void *handle);
extern int nx_closegroup(void *handle);
extern char *nx_getnextentry(void *handle, char separator);
extern void *nx_getgroupID(void *handle);
extern int nx_initgroupdir(void *handle);

%section "Dataset Handling"
extern int nx_makedata(void *handle, char *name, int rank, int type, 
		        void *dimPtr);
extern int nx_compmakedata(void *handle, char *name, int rank, int type, 
		        void *dimPtr, void *bufPtr);
extern int nx_opendata(void *handle, char *name);
extern int nx_closedata(void *handle);
extern int nx_putslab(void *handle, void *dataset, void *startDim);
extern void *nx_getslab(void *handle, void *startdim, void *size);
extern void *nx_getds(void *handle, char *name);
extern int   nx_putds(void *handle, char *name, void *dataset);
extern void *nx_getdata(void *handle);
extern int   nx_putdata(void *handle,void *dataset);
extern void *nx_getinfo(void *handle);
extern void *nx_getdataID(void *handle);


%section "Attributes"
extern char *nx_getnextattr(void *handle, char separator);
extern int   nx_putattr(void *handle, char *name, void *ds);
extern void *nx_getattr(void *handle, char *name, int type, int length); 

%section "Making Links"
extern int nx_makelink(void *handle, void *link);
extern int nx_makenamedlink(void *handle, char *name, void *link);
extern int nx_opensourcegroup(void *handle);

%section "External Linking"
extern char *nx_inquirefile(void *handle);
extern void *nx_isexternalgroup(void *handle, char *name, char* nxclass);
extern int nx_linkexternal(void *handle, char *name, char *nxclass, char *nxurl);
