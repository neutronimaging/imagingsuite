/**
* This module implements all functions and utilities which have to
* do with the validation context.
*
* Copyright: GPL
*
* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
* November 2015
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdarg.h>
#include "nxvcontext.h"
/*------------------------------------------------------------------*/
static void defaultLogPrint(char *key, void *data)
{
	fprintf(stdout,"%s=%s ", key, (char *)data);
}
/*-----------------------------------------------------------------*/
static void NXVdefaultLogger(hash_table *logData, void *userData)
{
	hash_enumerate(logData,defaultLogPrint);
	fprintf(stdout,"\n");
}
/*----------------------------------------------------------------*/
static char *readNXDL(FILE *in)
{
	char *nxdlData = NULL;
	struct stat stat;

	fstat(fileno(in), &stat);
	nxdlData = malloc(stat.st_size*sizeof(char));
	if(nxdlData != NULL){
		fread(nxdlData,stat.st_size,1,in);
	}
	return nxdlData;
}
/*----------------------------------------------------------------*/
static char *NXVdefaultRetriever(char *appDef, void *userData)
{
	char *nxdlRootPath = (char *)userData;
	char *nxdlPath = NULL, *nxdlData = NULL;
	int len;
	FILE *in = NULL;

  len = strlen(appDef) + strlen(nxdlRootPath) + 50;
	nxdlPath = malloc(len*sizeof(char));
	if(nxdlPath == NULL){
		return NULL;
	}

  /* zeroth attempt: the caller may have specified a full
	valid path*/
	in = fopen(appDef,"r");
	if(in != NULL){
		nxdlData = readNXDL(in);
		fclose(in);
		free(nxdlPath);
		return nxdlData;
	}

	/* first attempt: the specified directory */
	snprintf(nxdlPath,len,"%s/%s", nxdlRootPath, appDef);
	in = fopen(nxdlPath,"r");
	if(in != NULL){
		nxdlData = readNXDL(in);
		fclose(in);
		free(nxdlPath);
		return nxdlData;
	}

  /* second attempt: applications directory */
	snprintf(nxdlPath,len,"%s/applications/%s", nxdlRootPath, appDef);
	in = fopen(nxdlPath,"r");
	if(in != NULL){
		nxdlData = readNXDL(in);
		fclose(in);
		free(nxdlPath);
		return nxdlData;
	}

	/* third attempt: contributed definitions */
	snprintf(nxdlPath,len,"%s/contributed_definitions/%s", nxdlRootPath, appDef);
	in = fopen(nxdlPath,"r");
	if(in != NULL){
		nxdlData = readNXDL(in);
		fclose(in);
		free(nxdlPath);
		return nxdlData;
	}

	/* fourth attempt: base classes*/
	snprintf(nxdlPath,len,"%s/base_classes/%s", nxdlRootPath, appDef);
	in = fopen(nxdlPath,"r");
	if(in != NULL){
		nxdlData = readNXDL(in);
		fclose(in);
		free(nxdlPath);
		return nxdlData;
	}

	free(nxdlPath);
	return NULL;
}
/*------------------------------------------------------------------*/
pNXVcontext NXVinit(char *nxdlDir)
{
	pNXVcontext self = NULL;

	self = malloc(sizeof(struct __NXVContext));
	if(self == NULL){
		return NULL;
	}
	memset(self, 0, sizeof(struct __NXVContext));
	self->logger = NXVdefaultLogger;
	self->nxdlRetriever = NXVdefaultRetriever;
	self->retrieverUserData = nxdlDir;
	self->warnOptional = 1;
	self->warnBase = 1;
	self->warnUndefined = 1;
	self->debug = 1;
	hash_construct_table(&self->logData,50);
	return self;
}
/*----------------------------------------------------------------*/
void NXVkill(pNXVcontext self)
{
		if(self == NULL){
			return;
		}
		if(self->dataFile){
			free(self->dataFile);
		}
		if(self->dataPath){
			free(self->dataPath);
		}
		if(self->nxdlFile){
			free(self->nxdlFile);
		}
		if(self->nxdlPath){
			free(self->nxdlPath);
		}
		hash_free_table(&self->logData,free);
		free(self);
}
/*----------------------------------------------------------------*/
void NXDVsetOutputFlags(pNXVcontext self, int warnOptional,
												 int warnBase, int warnUndefined )
{
	assert(self != NULL);
	self->warnOptional = warnOptional;
	self->warnBase = warnBase;
	self->warnUndefined = warnUndefined;
}
/*----------------------------------------------------------------*/
void NXVsetNXDLRetriever(pNXVcontext self, RetrieveNXDL retriever,
												 void *userData)
{
	assert(self != NULL);
	self->nxdlRetriever = retriever;
	self->retrieverUserData = userData;
}
/*---------------------------------------------------------------*/
void NXVsetLogger(pNXVcontext self, validateLogger logger,
	void *userData)
{
	assert(self!= NULL);

	self->logger = logger;
	self->loggerUserData = userData;
}
/*--------------------------------------------------------------*/
void NXVprepareContext(pNXVcontext self, char *dataFile,
		char *appDef)
{
	if(self->dataFile != NULL){
		free(self->dataFile);
	}
	self->dataFile = strdup(dataFile);
	if(self->nxdlFile){
		free(self->nxdlFile);
	}
	if(appDef != NULL){
		self->nxdlFile = strdup(appDef);
	} else {
		self->nxdlFile = NULL;
	}

	hash_free_table(&self->logData,free);
	hash_construct_table(&self->logData,50);
	hash_insert("dataFile",strdup(dataFile),&self->logData);
	self->errCount = 0;
	self->warnCount = 0;
}
/*--------------------------------------------------------------*/
void NXVlog(pNXVcontext self)
{
	assert(self != NULL);
	self->logger(&self->logData, self->loggerUserData);
}
/*--------------------------------------------------------------*/
void NXVsetLog(pNXVcontext self, char *key, char *value)
{
	void *data = NULL;

	assert(self != NULL);
	data = hash_del(key,&self->logData);
	if(data != NULL){
		free(data);
	}
	data = hash_insert(key,strdup(value),&self->logData);
}
/*--------------------------------------------------------------*/
void NXVprintLog(pNXVcontext self, char *key, char *format, ...)
{
	char buf[1024];
  va_list ap;

	va_start(ap,format);
	vsnprintf(buf,sizeof(buf), format, ap);
	va_end(ap);
	NXVsetLog(self,key,buf);
}
