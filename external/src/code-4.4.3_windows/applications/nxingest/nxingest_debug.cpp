/* =============================================================================
  File Name : 	nxingest_debug.cpp
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
#include "nxingest_debug.h"

int Log::level = NXING_VERBOSE;

/* ***********************************************

	Function : Constructor and Destructor
	
	Description :  
		
*********************************************** */
Log::Log()
{
	source[0] 	= 0;
	msg1[0]	= 0;
	msg2[0]	= 0;
	msg3[0]	= 0;
	error		= NXING_OK;
}

Log::~Log()
{
	;
}

/* ***********************************************

	Function : Constructor and Destructor
	
	Description :  
		
*********************************************** */
Log& Log::set( const char* src, const char* msg_1, const char* msg_2, const char* msg_3, const int status)
{
	strncpy(source, src, NXING_BIG_SIZE);
	if(strlen(src) >= NXING_BIG_SIZE) source[NXING_BIG_SIZE-1] = 0;
	strncpy(msg1, msg_1, NXING_BIG_SIZE);
	if(strlen(msg1) >= NXING_BIG_SIZE) msg1[NXING_BIG_SIZE-1] = 0;
	strncpy(msg2, msg_2, NXING_BIG_SIZE);
	if(strlen(msg2) >= NXING_BIG_SIZE) msg2[NXING_BIG_SIZE-1] = 0;	
	strncpy(msg1, msg_1, NXING_BIG_SIZE);
	if(strlen(msg3) >= NXING_BIG_SIZE) msg3[NXING_BIG_SIZE-1] = 0;	
	error = status;
	return *this;
}

int& Log::getStatus(){return error;}

void Log::printLevel(int lvl)
{
	if(level >= lvl)
	{
		if(lvl == NXING_LOG_ERROR) cout << NXING_ERROR_PRECURSOR;
		else if(lvl == NXING_LOG_WARNING) cout << NXING_WARNING_PRECURSOR;
		else if(lvl == NXING_LOG_DEBUG) cout << NXING_DEBUG_PRECURSOR;
		else cout << NXING_LOG_PRECURSOR;
		print(); 
	}
	return;
}

void Log::print()
{
	cout << source << "\t" ;
	if(strlen(msg1) > 0) cout <<"'" << msg1 << "'\t" ;
	if(strlen(msg2) > 0) cout <<"'" << msg2 << "'\t" ;
	if(strlen(msg3) > 0) cout <<"'" << msg3 << "'\t" ;
	if(error != NXING_OK)
	{ 
		cout << "\t status = " << error ;
	}
	cout << endl; 
	return;
}

