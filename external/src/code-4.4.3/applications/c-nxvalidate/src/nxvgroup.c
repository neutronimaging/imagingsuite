/**
  * This file contains all the code for validating groups
	*
	* copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
	*/
#include <nxvalidate.h>
#include "nxvcontext.h"
#include <string.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <libxml/tree.h>

static void validateGroupAttributes(pNXVcontext self,
	hid_t groupID, xmlNodePtr groupNode)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static int isOptional(xmlNodePtr node)
{
	xmlChar *min= NULL;
	int num;

	min = xmlGetProp(node,(xmlChar *)"minOccurs");
	if(min == NULL){
		return 0;
	}
	num = atoi((char *)min);
	if(num == 0){
		return 1;
	} else {
		return 0;
	}
}
/*----------------------------------------------------------------
 Helper stuff for group Finding
 ----------------------------------------------------------------*/
typedef struct {
		char *nxClass;
		char *name;
} findByClassData;
/*--------------------------------------------------------------*/
static int FindByClassIterator(hid_t groupID,
	const char *name,
	const H5L_info_t *info, void *op_data)
{
	findByClassData *fbcb = (findByClassData *)op_data;
	H5O_info_t obj_info;
	hid_t attrID;
	char nxClass[132];

	H5Oget_info_by_name(groupID, name, &obj_info,H5P_DEFAULT);
	if(obj_info.type == H5O_TYPE_GROUP){
			/*
				work the NX_class attribute
			*/
			attrID = H5LTget_attribute_string(groupID,name,
				"NX_class", nxClass);
			if(attrID >= 0){
				if(strcmp(nxClass,fbcb->nxClass) == 0){
					fbcb->name = strdup(name);
					return 1;
				}
			}
  }
	return 0;
}
/*--------------------------------------------------------------
 Finding groups is hideous:
 * They may be specified by name. This seems easy but is complicated
   by the fact that the group name can either be a name attribute or
	 an attribute field.  A design flaw of NXDL, IMHO.
 * They may be specified by type and I need to search by NX_class.
---------------------------------------------------------------*/
static hid_t findGroup(pNXVcontext self, hid_t parentGroup, xmlNodePtr groupNode)
{
	xmlChar *name = NULL, *nxClass = NULL, *nodePath = NULL;
	xmlNodePtr cur = NULL;
	findByClassData fbcd;
	hid_t gid;
	hsize_t idx = 0;

  name = xmlGetProp(groupNode,(xmlChar *)"name");
	if(name == NULL){
		cur = groupNode->xmlChildrenNode;
		while(cur != NULL){
			if(xmlStrcmp(cur->name,(xmlChar *)"attribute") == 0){
				name = xmlGetProp(cur,(xmlChar *)"name");
				if(name != NULL){
					break;
				}
			}
			cur = cur->next;
		}
	}
	if(name != NULL){
		if(H5LTpath_valid(parentGroup,(char *)name, 1)){
			return H5Gopen(parentGroup,(char *)name,H5P_DEFAULT);
		} else {
			return -1;
		}
	}

	/*
		no name to be found: search by type
	*/
	nxClass = xmlGetProp(groupNode,(xmlChar *)"type");
	if(nxClass == NULL){
		NXVsetLog(self,"sev","error");
		nodePath = xmlGetNodePath(cur);
		NXVsetLog(self,"nxdlPath", (char *)nodePath);
		NXVsetLog(self,"message","Malformed group entry, type missing");
		NXVlog(self);
		xmlFree(nodePath);
		self->errCount++;
		return -1;
	}

	fbcd.nxClass = (char *)nxClass;
	fbcd.name = NULL;
	H5Literate(parentGroup, H5_INDEX_NAME, H5_ITER_INC, &idx,
		FindByClassIterator, &fbcd);
	if(fbcd.name != NULL){
		gid = H5Gopen(parentGroup,fbcd.name,H5P_DEFAULT);
		free(fbcd.name);
		return gid;
	}

	return -1;
}
/*--------------------------------------------------------------*/
static void validateLink(pNXVcontext self, xmlChar* target)
{
	/* TODO */
}
/*--------------------------------------------------------------*/
static void validateDependsOn(pNXVcontext self, hid_t groupID,
	hid_t fieldID)
{
	/* TODO */
}
/*---------------------------------------------------------------
We need two passes:

* In the first pass we try to find all the stuff in the NXDL
  group
* In the second pass we scan the HDF5 group in order to
  locate extra stuff

I keep the names already seen in the first pass in a
hash table

TODO: may be refactor this into processgroup, processfield?
This becomes to big for a function...... This is to tightly
coupled by data. Think first how to untangle this. Data is:
  * namesSeen
	* childPath
	* myPath
-----------------------------------------------------------------*/
int NXVvalidateGroup(pNXVcontext self, hid_t groupID,
	xmlNodePtr groupNode)
{
		hash_table namesSeen;
		xmlNodePtr cur = NULL;
		xmlChar *name = NULL;
		char childPath[512] ,childName[132], nxdlChildPath[512],
			groupPath[512];
		hid_t childID;
		char fName[256];
		char *myPath;
		char *myGroup;

		name = xmlGetProp(groupNode,(xmlChar *)"type");
		snprintf(groupPath,sizeof(groupPath),"%s/%s",
			self->nxdlPath,(char *)name);
		xmlFree(name);
		myGroup = self->nxdlPath;
		self->nxdlPath = groupPath;

		H5Iget_name(groupID,fName,sizeof(fName));
		NXVsetLog(self,"sev","debug");
		NXVsetLog(self,"message","Validating group");
		NXVsetLog(self,"nxdlPath",self->nxdlPath);
		NXVsetLog(self,"dataPath",fName);
		NXVlog(self);
		myPath = self->dataPath;

		validateGroupAttributes(self, groupID, groupNode);
		hash_construct_table(&namesSeen,100);

		/* first pass */
		cur = groupNode->xmlChildrenNode;
		while(cur != NULL){
			if(xmlStrcmp(cur->name,(xmlChar *) "group") == 0){
					childID = findGroup(self, groupID, cur);
					if(childID >= 0){
							H5Iget_name(childID, childName,sizeof(childName));
							snprintf(childPath,sizeof(childPath),"%s/%s",
								fName,childName);
							hash_insert(childName,strdup(""),&namesSeen);
							NXVvalidateGroup(self,childID,cur);
					} else {
						if(!isOptional(cur)){
							name = xmlGetProp(cur,(xmlChar *)"type");
							snprintf(nxdlChildPath,sizeof(nxdlChildPath),"%s/%s",
								self->nxdlPath, (char *)name);
							xmlFree(name);
							NXVsetLog(self,"sev","error");
							NXVsetLog(self,"dataPath",fName);
							NXVsetLog(self,"nxdlPath", nxdlChildPath);
							NXVsetLog(self,"message","Required group missing");
							NXVlog(self);
							self->errCount++;
						}
					}
			}
			if(xmlStrcmp(cur->name,(xmlChar *) "field") == 0){
					name = xmlGetProp(cur,(xmlChar *)"name");
					if(H5LTfind_dataset(groupID,(char *)name) ) {
						childID = H5Dopen(groupID,(char *)name,H5P_DEFAULT);
					} else {
						childID = -1;
					}
					snprintf(childPath,sizeof(childPath),"%s/%s",
						fName,name);
					if(childID < 0){
						if(!isOptional(cur)){
									NXVsetLog(self,"sev","error");
									NXVsetLog(self,"dataPath",childPath);
									snprintf(nxdlChildPath,sizeof(nxdlChildPath),
										"%s/%s", self->nxdlPath, name);
									NXVsetLog(self,"nxdlPath", nxdlChildPath);
									NXVsetLog(self,"message","Required field missing");
									NXVlog(self);
									self->errCount++;
						}
					} else {
						if(xmlStrcmp(name,(xmlChar *)"depends_on") == 0){
							validateDependsOn(self,groupID,childID);
						} else {
							self->dataPath = childPath;
							NXVvalidateField(self,childID, cur);
						}
						hash_insert((char *)name,strdup(""),&namesSeen);
					}
			}
			if(xmlStrcmp(cur->name,(xmlChar *) "link") == 0){
				name = xmlGetProp(cur,(xmlChar *)"name");
				hash_insert((char *)name,strdup(""),&namesSeen);
				name = xmlGetProp(cur,(xmlChar *)"target");
				validateLink(self,name);
			}
			cur = cur->next;
		}

		/*
			TODO: second pass Do we want this? We will warn on this
			anyway only. Defer for the time being...
		*/
		hash_free_table(&namesSeen,free);
		self->dataPath = myPath;
		self->nxdlPath = myGroup;
		return 0;
	}
