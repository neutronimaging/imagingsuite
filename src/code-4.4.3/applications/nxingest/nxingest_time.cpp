/* =============================================================================
  File Name : 	nxingest_time.cpp
  Version   : 	1.7
  Component : 	nxingest
  Developer : 	Laurent Lerusse 
				e-Science Center - Facility Support - Data Management Group
				       
  Purpose   : 	nxingest extract the metadata from a NeXus file to create an 
				XML file according to a mapping file. 

  Revision History : 
		Version 1.0		05/06/2007		
			First version. 
			
		Version 1.1		13/08/2007		
			Replace the strptime function because it is not available under windows.  
			  
		Version 1.2		16/08/2007		
			Bug correction: if the string is too short for the expected transformation, 
			an out of bound exception would have been raised by substr    
		
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

#include "nxingest_time.h"




// *****************************************************************************
//	Function : 		Time();
// *****************************************************************************
Time::Time()
{
	
	strcpy(format[IS08601],  "%Y-%m-%dT%H:%M:%S");	// ISO8601		'2007-05-23T12:48:05'
	strcpy(format[MUON_CREA],  "%Y-%m-%d %H:%M:%S+0000");	// MUON_CREA		'2007-05-23 12:48:05+0000'

	strcpy(format[ISO_Date], "%Y-%m-%d");			// ISO_Date		'2007-05-23'
	strcpy(format[ISO_Time], "%H:%M:%S");			// ISO_Time		'12:48:05'
	strcpy(format[CMP_Date], "%Y%m%d");				// CMP_Date		'20070523'
	strcpy(format[CMP_YM],   "%Y%m");				// CMP_YM		'200705'
	strcpy(format[CMP_Year], "%Y");					// CMP_Year		'2007'
	strcpy(format[NRM_Date], "%d/%m/%Y");			// NRM_Date		'23/05/2007'
	return;
}
// *****************************************************************************
//	Function : 		~Time();
// *****************************************************************************
Time::~Time()
{
	return;
}

// *****************************************************************************
//	Function : 		Time& defCustomTime(char* ct);
// *****************************************************************************

Time& Time::defCustomTime(char* ct)
{
	if(ct != 0) strncpy(format[Custom],  ct, NXING_MED_SIZE);
	return *this;
}

// *****************************************************************************
//	Function : 		Time& set(char* t, int timeType);
//	strcpy(format[IS08601],  "%Y-%m-%dT%H:%M:%S");	// ISO8601		'2007-05-23T12:48:05'
//	strcpy(format[MUON_CREA],  "%Y-%m-%d %H:%M:%S+0000");	// MUON_CREA		'2007-05-23 12:48:05+0000'

//	strcpy(format[ISO_Date], "%Y-%m-%d");			// ISO_Date		'2007-05-23'
//	strcpy(format[ISO_Time], "%H:%M:%S");			// ISO_Time		'12:48:05'
//	strcpy(format[CMP_Date], "%Y%m%d");				// CMP_Date		'20070523'
//	strcpy(format[CMP_YM],   "%Y%m");				// CMP_YM		'200705'
//	strcpy(format[CMP_Year], "%Y");					// CMP_Year		'2007'
//	strcpy(format[NRM_Date], "%d/%m/%Y");			// NRM_Date		'23/05/2007'
// *****************************************************************************
Time&	Time::set( char* tt, int timeType)
{
	now();
	string time = tt;
	tm.tm_year  = 2000  - 1900; // year since 1900
	tm.tm_mon   = 07	-1; // mon from 0 - 11
	tm.tm_mday  = 01;
	tm.tm_hour  = 00;
	tm.tm_min   = 00;
	tm.tm_sec   = 00;
	tm.tm_isdst = -1;
	if( ((timeType == IS08601 || timeType == MUON_CREA) && time.size() < 19) ||
	    ((timeType == ISO_Date || timeType == NRM_Date) && time.size() < 10) ||
		((timeType == ISO_Time || timeType == CMP_Date) && time.size() <  8) ||
		 (timeType == CMP_YM   && time.size() < 6) || (timeType == CMP_Year && time.size() < 4) )
		return *this; // string too short, exception may be raised by substr
		
	switch(timeType)
	{ 
		case IS08601 :
		case MUON_CREA :
			tm.tm_year  = atoi(time.substr( 0, 4).c_str()) -1900;
			tm.tm_mon   = atoi(time.substr( 5, 2).c_str()) -1;
			tm.tm_mday  = atoi(time.substr( 8, 2).c_str());
			tm.tm_hour  = atoi(time.substr(11, 2).c_str());
			tm.tm_min   = atoi(time.substr(14, 2).c_str());
			tm.tm_sec   = atoi(time.substr(17, 2).c_str());
			
			break;
			
		case ISO_Date :
			tm.tm_year  = atoi(time.substr( 0, 4).c_str()) -1900;
			tm.tm_mon   = atoi(time.substr( 5, 2).c_str()) -1;
			tm.tm_mday  = atoi(time.substr( 8, 2).c_str());
			break;
			
		case ISO_Time :
			tm.tm_hour  = atoi(time.substr(0, 2).c_str());
			tm.tm_min   = atoi(time.substr(3, 2).c_str());
			tm.tm_sec   = atoi(time.substr(6, 2).c_str());
			break;

		case CMP_Date :
			tm.tm_mday  = atoi(time.substr( 6, 2).c_str());
		case CMP_YM :
			tm.tm_mon   = atoi(time.substr( 4, 2).c_str()) -1;		
		case CMP_Year :
			tm.tm_year  = atoi(time.substr( 0, 4).c_str()) -1900;
			break;
			
		case NRM_Date :
			tm.tm_year  = atoi(time.substr( 6, 4).c_str())-1900;
			tm.tm_mon   = atoi(time.substr( 3, 2).c_str()) -1;
			tm.tm_mday  = atoi(time.substr( 0, 2).c_str());
			break;
	}
	return *this;		
}

// *****************************************************************************
//	Function : 		Time& set(double sec, char* d = 0, int timeType = 0);
// *****************************************************************************
Time& Time::set(double sec, char* d, int timeType)
{
	Time date;
	if(d != 0) date.set(d, timeType);
	else date.now();
	time_t tt = date.getMidnight();
	tt += (time_t) sec;
	struct tm* tmPtr = localtime(&tt);
	tm = *tmPtr;
	return *this;		
}

// *****************************************************************************
//	Function : 		Time& set(time_t tt);
// *****************************************************************************
Time& Time::set(time_t tt)
{
	struct tm* tmPtr = localtime(&tt);
	tm = *tmPtr;
	return *this;
}

// *****************************************************************************
//	Function : 		Time& now();
// *****************************************************************************
Time& Time::now()
{
	time_t tt = time (NULL);
	struct tm* tmPtr =  localtime(&tt);
	tm = *tmPtr;
	return *this;
}

// *****************************************************************************
//	Function : 		char* getTime( char* buff, int timeType);
// *****************************************************************************
char* Time::getTime( char* buff, int timeType)
{
	
	int timeLength = NXING_MED_SIZE;
	if(timeType == Custom && strlen(format[Custom]) <= 0) return buff;
	
	if(timeType >= 0 )
	{
		strftime( buff, timeLength, format[timeType], &tm);	
	}
	else if(timeType == unix_t || timeType == secInDay )
	{
		int t = this->getTime(timeType);	
		sprintf(buff, "%d", t);
	}
	else
	{
		strcpy(buff, "");
	}
	return buff;
}

// *****************************************************************************
//	Function : 		time_t getTime(int timeType);
// *****************************************************************************
time_t Time::getTime(int timeType)
{
	if(timeType >= 0)
		return -1;	
	time_t tt = mktime(&tm);	
	if(timeType == secInDay)
	{
		tt -= this->getMidnight();
	}
	return tt;
}

// *****************************************************************************
//	Function : 		time_t getTime(int timeType);
// *****************************************************************************
time_t Time::getMidnight()
{
	struct tm tm_midnight;
	time_t tt_midnight = 0;
	tm_midnight.tm_year  = tm.tm_year;
	tm_midnight.tm_mon   = tm.tm_mon;
	tm_midnight.tm_mday  = tm.tm_mday;
	tm_midnight.tm_isdst = tm.tm_isdst;
	tm_midnight.tm_sec  = 0;
	tm_midnight.tm_hour = 0;
	tm_midnight.tm_min  = 0;
	
	tt_midnight = mktime(&tm_midnight);	
	return tt_midnight;
}