/* =============================================================================
  File Name : 	nxingest_main.h
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
#ifndef _NXING_MAIN_HEADER_
#define _NXING_MAIN_HEADER_

#include "nxingest.h"
#include "nxingest_debug.h"
#include "nxingest_utils.h"
#include "nxingest_parse.h"

#define NXING_ERR_WRONG_INPUT		NXING_ERR_BASE_MAIN -1
#define NXING_ERR_CANT_OPEN_PARAM	NXING_ERR_BASE_MAIN -2
#define NXING_ERR_CANT_OPEN_OUTPUT	NXING_ERR_BASE_MAIN -3

#endif