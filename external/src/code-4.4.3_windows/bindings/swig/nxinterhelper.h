/*
  This is a library of support functions and data structures which can be
  used in order to create interfaces between the NeXus-API and scripting
  languages or data analysis systems with a native code interface. 

  copyright: GPL

  Mark Koennecke, October 2002
*/

#ifndef NXINTERHELPER
#define NXINTERHELPER

#include <napi.h>

/*------------- opening and closing section ---------------------*/
void *nx_open(char *filename, int accessMethod);
void *nx_flush(void *handle);
void  nx_close(void *handle);
/*--------------- group handling section ------------------------*/
int nx_makegroup(void *handle, char *name, char *nxclass);
int nx_opengroup(void *handle, char *name, char *nxclass);
int nx_opengrouppath(void *handle, char *path);
int nx_openpath(void *handle, char *path);
extern char *nx_getpath(void *handle);
int nx_closegroup(void *handle);
char *nx_getnextentry(void *handle, char separator);
void *nx_getgroupID(void *handle);
int nx_initgroupdir(void *handle);

/*---------------- dataset handling -----------------------------*/
int nx_makedata(void *ptr, char *name, int rank, int type, void *dimPtr);
int nx_compmakedata(void *ptr, char *name, int rank, int type, 
		    void *dimPtr, void *bufPtr);

int nx_opendata(void *handle, char *name);
int nx_closedata(void *handle);

int nx_putslab(void *handle, void *dataset, void *startDim);
void *nx_getslab(void *handle, void *startdim, void *size);

void *nx_getds(void *handle, char *name);
int   nx_putds(void *handle, char *name, void *dataset);

void *nx_getdata(void *handle);
int   nx_putdata(void *handle,void *dataset);

void *nx_getinfo(void *handle);
void *nx_getdataID(void *handle);

/*-------------------- attributes --------------------------------*/
char *nx_getnextattr(void *handle, char separator);
int   nx_putattr(void *handle, char *name, void *ds);
void *nx_getattr(void *handle, char *name, int type, int length); 


/*---------------------- link -----------------------------------*/
int nx_makelink(void *handle, void *link);
int nx_makenamedlink(void *handle, char *name, void *link);
int nx_opensourcgroup(void *handle);
void *nx_isexternalgroup(void *handle, char *name, char *nxclass);
int nx_linkexternal(void *handle, char *name, char *nxclass, char *url);
char *nx_inquirefile(void *handle);
int nx_opensourcegroup(void *handle);
/*----------------- error handling -----------------------------*/
char *nx_getlasterror(void);

#endif






