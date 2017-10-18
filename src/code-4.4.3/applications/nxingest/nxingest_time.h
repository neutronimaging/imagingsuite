/* =============================================================================
  File Name : 	nxingest_time.h
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
#ifndef _NXING_TIME_HEADER_
#define _NXING_TIME_HEADER_

#include <string>
#include "nxingest.h"
#include "nxingest_debug.h"
#include "nxingest_utils.h"

enum timeType { IS08601 = 0,  MUON_CREA, ISO_Date, ISO_Time, CMP_Date, CMP_YM, CMP_Year, NRM_Date, Custom, unix_t = -1, secInDay = -2 };

class Time
{
	private:
		char timeStr[NXING_MED_SIZE];
		struct tm tm;
		char format[Custom+1][NXING_MED_SIZE] ;
		
	 public : 
		Time();
		~Time();
		Time&  set(char* t, int timeType);
		Time&  set(double sec, char* d = 0, int timeType = 0);
		Time&  set(time_t tt);
		Time&  now();
		Time&  defCustomTime(char* ct);
		char*  getTime( char* buff, int timeType);
		time_t getTime(int timeType);
		time_t getMidnight();

};

#endif



