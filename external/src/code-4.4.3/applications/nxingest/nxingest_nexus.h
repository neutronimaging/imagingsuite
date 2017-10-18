/* =============================================================================
  File Name : 	nxingest_nexus.h
  Version   : 	1.7
  Component : 	nxingest
  Developer : 	Laurent Lerusse 
				e-Science Center - Facility Support - Data Management Group
				       
  Purpose   : 	nxingest extract the metadata from a NeXus file to create an 
				XML file according to a mapping file. 

  Revision History : 
		Version 1.0		05/06/2007		First version. 
		
  Copyright : 
		nxingest  - extraction of metadata from NeXus files into a xml document.
		Copyright (C) 2007  STFC e-Science Center
		
		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; version 2 of the License.
	
		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.
============================================================================= */
#ifndef _NXING_NEXUS_HEADER_
#define _NXING_NEXUS_HEADER_

#include "nxingest.h"
#include "nxingest_debug.h"
#include "nxingest_utils.h"

#define		NXING_MAX_USERS		64

#define		NXING_ERR_NEGATIVE_NUMVAL	NXING_ERR_BASE_NEXUS - 100

class NxClass
{
	private:
		NXhandle	nxFp;
		char entry[NX_MAXNAMELEN];					// Replace {NXentry}
		char inst[NX_MAXNAMELEN];					// Replace {NXinstrument}
		char users[NXING_MAX_USERS][NX_MAXNAMELEN];	// Replace {NXuser}
		int numUsers;
		int currentUser;
		
	public : 
		int status;
	
	public:
		NxClass(const char* filename); 
		~NxClass();
		char* readTag(char *input, char *value, int user);
		char* getLocation(char *value);

		int maxUsers(){return numUsers;};
		int user(){return currentUser;};
		int nextUser(){ if((currentUser+1)<numUsers) { return (++currentUser); } else { return -1; } };
		int setUser(int u = 0){ if(u>=0 && u < numUsers) { currentUser = u; return currentUser; } else { return -1;} };
		bool isOK(){ if(status == NXING_OK) return true; else return false; }
		bool isNotOK(){ if(status != NXING_OK) return true; else return false; }

};

#endif
