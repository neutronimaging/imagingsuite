/**
* This module implements all functions and utilities which have to
* do with the validation context.
*
* Copyright: GPL
*
* Mark Koennecke, mark.koennecke@psi.ch, and NIAC
* November 2015
*/

#include <nxvalidate.h>
#include <hdf5.h>
#include <libxml/tree.h>

typedef struct __NXVContext {
		char *dataFile;
		char *nxdlFile;
		char *dataPath;
		char *nxdlPath;
		validateLogger logger;
		void *loggerUserData;
		RetrieveNXDL   nxdlRetriever;
		void *retrieverUserData;
		unsigned int warnOptional;
		unsigned int warnBase;
		unsigned int warnUndefined;
		unsigned int debug;
		hash_table logData;
		hid_t fileID;
		int subEntryFound;
		unsigned int errCount;
		unsigned int warnCount;
		xmlDocPtr nxdlDoc;
}NXVcontext;

/*
	* reinitialize the context for doing another file
	*/
void NXVprepareContext(pNXVcontext self, char *dataFile, char *appDef);

/*
 * write a log entry
 */
void NXVlog(pNXVcontext self);

/*
 * set an entry in the log dictionary
 */
void NXVsetLog(pNXVcontext self, char *key, char *value);
void NXVprintLog(pNXVcontext self, char *key, char *format, ...);

/*
 * implemented in nxdlutil.c
 */
int NXVloadAppDef(pNXVcontext self, char *nxdlFile);
xmlNodePtr NXVfindEntry(pNXVcontext self);

/*
 * implementd in nxvgroup.c
 */
int NXVvalidateGroup(pNXVcontext self, hid_t groupID,
	xmlNodePtr groupNode);

/*
 * implemented in nxvfield.c
 */
 int NXVvalidateField(pNXVcontext self, hid_t fieldID,
 	xmlNodePtr groupNode);
