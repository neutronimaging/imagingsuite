/* =============================================================================
  File Name : 	nxingest_debug.h
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

#ifndef _NXING_DEBUG_HEADER_
#define _NXING_DEBUG_HEADER_

#include "nxingest.h"

#define NXING_LOG_ERROR			00
#define NXING_LOG_WARNING		01
#define NXING_LOG_IMPORTANT		10	
#define NXING_LOG_NORMAL		11
#define NXING_LOG_DETAIL		12
#define NXING_LOG_ALL			15	
#define NXING_LOG_DEBUG			20

#define NXING_VERBOSE		NXING_LOG_NORMAL

#define NXING_ERROR_PRECURSOR 	"Error : "
#define NXING_WARNING_PRECURSOR "Warn  : "
#define NXING_LOG_PRECURSOR     "Log   : "
#define NXING_DEBUG_PRECURSOR   "Debug : "


class Log
{
	private	:
		static int level;
		char source[NXING_BIG_SIZE];
		char msg1[NXING_BIG_SIZE];
		char msg2[NXING_BIG_SIZE];
		char msg3[NXING_BIG_SIZE];
		int error;
	
	public	:
		Log();
		~Log();
		Log&  set(const char*, const char* = "", const char* = "", const char* = "", const int = NXING_OK);
		int&    getStatus();
		void	print();
		void	printLevel(int);
};
#endif 

