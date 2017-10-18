/* =============================================================================
  File Name : 	nxingest.h
  Version   : 	1.7
  Component : 	nxingest
  Developer : 	Laurent Lerusse 
				e-Science Center - Facility Support - Data Management Group
				       
  Purpose   : 	nxingest extract the metadata from a NeXus file to create an 
				XML file according to a mapping file. 

  Revision History : 
		Version 1.0		05/06/2007		First version. 

		Version 1.1		13/06/2007		Remove trailing white space around string. 
		
		Version 1.2		16/08/2007		
			Bug correction: if the string is too short for the expected transformation, 
			an out of bound exception would have been raised by substr  
					
		Version 1.3		31/08/2007		
			Bug correction in test. (Logical OR and not bitwise OR)
			Add a filesize calculation function. 

		Version 1.4		04/09/2007	
			The parameters will not be created if there is no value attached to it. 

		Version 1.5		5/09/2007
			To avoid wrapping the xml document, 
			set constnty 'MXML_WRAP' to a high value in mxml.h (under linux) or 
			under windows, add function mxmlSetWrapMargin(0);

		Version 1.6		06/09/2007	
			Bug correction. Connected to changes of version 1.4. 
			Bug Correction. Difference of path between linux / and windows \
		
		Version 1.7		12/09/2007
			Bug correction in parseMix function when parseSpecial return a null string.	
					
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

#ifndef _NXING_HEADER_
#define _NXING_HEADER_

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include "napi.h"
#include "mxml.h"


#define NXING_OK					NX_OK
#define NXING_ERR_BASE_DEBUG		-1000
#define NXING_ERR_BASE_UTILS		-2000
#define NXING_ERR_BASE_MAIN 		-3000
#define NXING_ERR_BASE_PARSE		-4000
#define NXING_ERR_BASE_NEXUS		-5000

#define NXING_SHORT_SIZE	64
#define NXING_MED_SIZE		256
#define NXING_BIG_SIZE		1024
#define NXING_HUGE_SIZE		3999
#define NXING_MAX_DIM		32


// #define NXING_ERR					-1000
// #define NXING_NOVAL					-1101
// #define NXING_INVALID_DATATYPE		-1102
// #define NXING_WRONG_INPUT			-1105
// #define NXING_ERR_INVALID_DATATYPE	-1106
// #define NXING_ERR_NO_TBL_TYPE		-1107
// #define NXING_UNEXPECTED_PARAM_VALUE -1108
// #define NXING_ERR_WRONG_INPUT		-1001
// #define NXING_ERR_CANT_OPEN_PARAM	-1002
// #define NXING_ERR_CANT_OPEN_OUTPUT	-1003
// #define NXING_ERR_CANT_OPEN_NEXUS	-1004
// #define NXING_ERR_NEXUS				-2000


using namespace std;
 


#endif 
