/**
  * This file implements some utilities for processing application
	* definitions. Most importantly loading and flattening the
	* inheritance hierachy.
	*
	* copyright: GPL
  *
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	*/
#include "nxvcontext.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <string.h>

static void mergeInheritance(pNXVcontext self)
{
	/* TODO
   - check the extends attribute of definition
	 - If not NXobject:
	   * load the base application definition
		 * merge the base application defintition into
		   the tree.
	OR: Discuss if we want this. I have a Tcl script which does
	that. So, there is the option to use inheritance only internally
	and have deployed application definitions which are complete.
	This saves on complexity here. And may be even the users.
	*/
}
/*--------------------------------------------------------------*/
int NXVloadAppDef(pNXVcontext self, char *nxdlFile)
{
	char *pPtr = NULL;
  char *xmlData = NULL;
	xmlNodePtr root = NULL;
  /*
	 get at the raw name without any extras. Such as that the
	 retriever can do her job properly
	 */
	pPtr = strrchr(nxdlFile,'/');
	if(pPtr == NULL){
		pPtr = nxdlFile;
	}

  xmlData = self->nxdlRetriever(pPtr,self->retrieverUserData);
	if(xmlData == NULL){
		NXVsetLog(self,"sev","fatal");
		NXVsetLog(self,"message","Failed to load application definition");
		NXVlog(self);
		self->errCount++;
		return 1;
	}

	if(self->nxdlDoc != NULL){
		xmlFreeDoc(self->nxdlDoc);
	}
	self->nxdlDoc = xmlParseDoc((xmlChar *)xmlData);
	root = xmlDocGetRootElement(self->nxdlDoc);
	if(root == NULL){
		xmlFreeDoc(self->nxdlDoc);
		self->nxdlDoc = NULL;
		NXVsetLog(self,"sev","fatal");
		NXVsetLog(self,"message","Failed to parse application definition");
		NXVlog(self);
		self->errCount++;
		return 1;
	}

  mergeInheritance(self);

	return 0;
}
/*----------------------------------------------------------------*/
xmlNodePtr NXVfindEntry(pNXVcontext self)
{
	xmlNodePtr root = xmlDocGetRootElement(self->nxdlDoc);
	xmlNodePtr cur;

	if(xmlStrcmp(root->name,(xmlChar *)"definition" ) != 0){
		NXVsetLog(self,"sev","fatal");
		NXVsetLog(self,"message",
			"No definition element in application definition, INVALID");
		NXVlog(self);
		self->errCount++;
		return NULL;
	}

  cur = root->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *) "group") == 0){
			return cur;
		}
		cur = cur->next;
	}

	/*
		not found a group element
	*/
	NXVsetLog(self,"sev","fatal");
	NXVsetLog(self,"message",
		"No group element in application definition, INVALID");
	NXVlog(self);
	self->errCount++;
	return NULL;
}
