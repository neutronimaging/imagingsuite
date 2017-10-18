/* =============================================================================
  File Name : 	nxingest_utils.h
  Version   : 	1.7
  Component : 	nxingest
  Developer : 	Laurent Lerusse 
				e-Science Center - Facility Support - Data Management Group
				       
  Purpose   : 	nxingest extract the metadata from a NeXus file to create an 
				XML file according to a mapping file. 

  Revision History : 
		Version 1.0		05/06/2007		First version. 
		
		Version 1.3		31/08/2007		
			Bug correction in test. (Logical OR and not bitwise OR)
			Add a filesize calculation function. 
		
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
#ifndef _NXING_UTILS_HEADER_
#define _NXING_UTILS_HEADER_

#include "nxingest.h"
#include "nxingest_debug.h"
#include "nxingest_nexus.h"

#include <sys/types.h> 
#include <sys/stat.h> 

#define NXING_ERR_TOO_LONG			NXING_ERR_BASE_UTILS -1
#define NXING_ERR_INVALID_INPUTS	NXING_ERR_BASE_UTILS -2

long fileSize( const string &fname ); 

char* mxmlGetItem(mxml_node_t *node, char* str);
char* mxmlGetItem(mxml_node_t *node);

inline float f_rand(float a = 1.)
{
	return (a * 2.0 * (.5 - ( (float)rand()/(float)RAND_MAX ) ) );
}

inline float abs_rand(float a = 1.)
{
	return (a * (float)rand()/(float)RAND_MAX );
}


#endif 