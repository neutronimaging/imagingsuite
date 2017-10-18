/**
  * This file contains all the code for validating fields
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	*/
#include "nxvcontext.h"
#include <hdf5.h>
#include <libxml/tree.h>

static void validateData(pNXVcontext self, hid_t fieldID,
	xmlNodePtr enumNode)
{
	/* TODO */
}
/*-------------------------------------------------------------*/
static void validateDimensions(pNXVcontext self, hid_t fieldID,
	xmlNodePtr dimNode)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validateUnits(pNXVcontext self, hid_t fieldID,
	xmlChar *type)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validateType(pNXVcontext self, hid_t fieldID,
	xmlChar *type)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validateSignal(pNXVcontext self, hid_t fieldID,
	xmlChar *type)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validateAxes(pNXVcontext self, hid_t fieldID,
	xmlChar *data)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validateAxis(pNXVcontext self, hid_t fieldID,
	xmlChar *data)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validatePrimary(pNXVcontext self, hid_t fieldID,
	xmlChar *data)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validateAttributes(pNXVcontext self, hid_t fieldID,
	xmlNodePtr fieldNode)
{
	xmlChar *data = NULL, *name = NULL;
	xmlNodePtr cur;

	data = xmlGetProp(fieldNode,(xmlChar *)"type");
	if(data != NULL){
		validateType(self,fieldID,data);
	}

	data = xmlGetProp(fieldNode,(xmlChar *)"units");
	if(data != NULL){
		validateUnits(self,fieldID,data);
	}

	data = xmlGetProp(fieldNode,(xmlChar *)"signal");
	if(data != NULL){
		validateSignal(self,fieldID,data);
	}
	data = xmlGetProp(fieldNode,(xmlChar *)"axes");
	if(data != NULL){
		validateAxes(self,fieldID,data);
	}

	data = xmlGetProp(fieldNode,(xmlChar *)"axis");
	if(data != NULL){
		validateAxis(self,fieldID,data);
	}

	data = xmlGetProp(fieldNode,(xmlChar *)"primary");
	if(data != NULL){
		validatePrimary(self,fieldID,data);
	}

	cur = fieldNode->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *)"attribute") == 0){
			name = xmlGetProp(cur,(xmlChar *)"name");
			/* TODO */
		}
		cur = cur->next;
	}
}
/*--------------------------------------------------------------*/
int NXVvalidateField(pNXVcontext self, hid_t fieldID,
  	xmlNodePtr fieldNode)
{
  xmlNodePtr cur;
  char fName[256], nxdlName[512];
  xmlChar *name;
	char *myPath;

	name = xmlGetProp(fieldNode,(xmlChar *)"name");
	snprintf(nxdlName,sizeof(nxdlName),"%s/%s",
		self->nxdlPath,name);
	H5Iget_name(fieldID,fName,sizeof(fName));
	NXVsetLog(self,"sev","debug");
	NXVsetLog(self,"message","Validating field");
	NXVsetLog(self,"dataPath",fName);
	NXVsetLog(self,"nxdlPath",nxdlName);
	NXVlog(self);

	myPath = self->nxdlPath;
	self->nxdlPath = nxdlName;

  validateAttributes(self,fieldID,fieldNode);

	cur = fieldNode->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrcmp(cur->name,(xmlChar *)"enumeration") == 0){
			validateData(self,fieldID,cur);
		}
		if(xmlStrcmp(cur->name,(xmlChar *)"dimensions") == 0){
			validateDimensions(self,fieldID,cur);
		}
		cur = cur->next;
	}

	self->nxdlPath = myPath;
	return 0;
}
