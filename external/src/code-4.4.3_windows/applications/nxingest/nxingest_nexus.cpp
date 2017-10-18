/* =============================================================================
  File Name : 	nxingest_nexus.cpp
  Version   : 	1.3
  Component : 	nxingest
  Developer : 	Laurent Lerusse 
				e-Science Center - Facility Support - Data Management Group
				       
  Purpose   : 	nxingest extract the metadata from a NeXus file to create an 
				XML file according to a mapping file. 

  Revision History : 
		Version 1.0		05/06/2007		
			First version. 

		Version 1.1		13/08/2007		
			Replace the uint and ushort by unsigned int and unsigned short.
			Declare loop variable before switch. (Windows portability)    

		Version 1.3		31/08/2007		
			Bug correction in test. (Logical OR and not bitwise OR)
			Add a filesize calculation function. 

		Version 1.9		08/09/2007		
			Make sure that the location string will use the linxu separator '/'
		
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
#include "nxingest_nexus.h"

#include <string>
#include <stdint.h>
#include <sstream>
#include <vector>

using std::string;

string buff2str(const void* buff, int rank, const int dim[], int data_type, const char* vector);

// Enumeration for the different calculation available for arrays. 
// Positives number are used for specific value of the array. 
enum Calculation { Direct = 0, Avg = -1, Min = -2, Max = -3, Std = -4, Sum = -5};

// *****************************************************************************
//	Function : NxClass Constructor
//		
//		The constructor will initialise the class by opening the NeXus file and 
//		Searching for the class name for the class NXentry, NXinstrument and 
//		NXuser. More than 1 NXuser class may be present.
// 
// *****************************************************************************
 
NxClass::NxClass(const char* filename)
{
	Log log;
	numUsers = 0;
	currentUser = -1;
	status = NXING_OK;
	try
	{		
		status = NXopen (filename, NXACC_READ, &nxFp );
		if( status != NXING_OK ) throw log.set("NxClass", "Can't open the neXus file!", filename, "", NXING_ERR_BASE_NEXUS-status);
		char nx_name[NX_MAXNAMELEN];
		char nx_group[NX_MAXNAMELEN];
		int data_type;
		
		// Find the NXentry and open the group
		status = NXinitgroupdir(nxFp);
		while( (status = NXgetnextentry (nxFp, nx_name, nx_group, &data_type)) == NXING_OK && strcmp(nx_group, "NXentry") != 0 ){;}
		if(strcmp(nx_group, "NXentry") == 0) strcpy(entry, nx_name);
		status = NXopengroup(nxFp, nx_name, nx_group);
		if( status != NXING_OK ) throw log.set("NxClass", "Can't open the neXus group!", nx_name, nx_group, NXING_ERR_BASE_NEXUS-status);

		// Find the NXuser and NXinstrument class 
		status = NXinitgroupdir(nxFp);
		while( (status = NXgetnextentry (nxFp, nx_name, nx_group, &data_type)) == NXING_OK)
		{
			if(strcmp(nx_group, "NXinstrument") == 0) 
			{
				strcpy(inst, nx_name);
			}
			if(strcmp(nx_group, "NXuser") == 0)  
			{
				strcpy(users[numUsers], nx_name);
				numUsers++;
				currentUser = 0;
			}
		}
		char str[NXING_MED_SIZE];
		sprintf(str, "%d Users found!",numUsers); 
		log.set("NxClass", entry, inst, str).printLevel(NXING_LOG_DEBUG);
		status = NXING_OK;
		return;
	}
	catch(Log log)
	{
		log.printLevel(NXING_LOG_ERROR);
		return;
	}
}

NxClass::~NxClass()
{
	numUsers = 0;
	currentUser = -1;
	return;
}

// *****************************************************************************
//	Function : NxClass::readTag
//		
//		The function will parse the input NeXus path to replace the NXentry, 
//		NXinstrument and NXuser by their proper value. 
//		After that it will check if it need to read 
//		- an attribute (separated from the path with a '.') or 
//		- an array (which ends with [i], [AVG], [STD], [MIN] or [MAX])
//		Read the data and ask the function buff2str to transform it into 
//		a string that the function will return. 
// 
// *****************************************************************************
char* NxClass::readTag(char *input, char *value, int user)
{
	Log log;
	try 
	{
		if(user != -1 && (user >= 0 && user < numUsers)) currentUser = user;
			
		// Replace the {NXentry}, {NXinstrument} and {NXuser} 
		// by their values read at initialisation.
		// Store the modified string into nx_name
		// ***************************************************
		char nx_name[NXING_BIG_SIZE] = "";
		char *pch = strtok (input,"{}");
		while (pch != NULL)
		{
			if(strcmp(pch, "NXentry") == 0) strcat(nx_name, entry);
			else if(strcmp(pch, "NXinstrument") == 0 ) strcat(nx_name, inst);
			else if(strcmp(pch, "NXuser") == 0) strcat(nx_name, users[currentUser]);
			else strcat(nx_name, pch);
			pch = strtok (NULL, "{}");
			
		}
		while(nx_name[strlen(nx_name)-1] == ' ') nx_name[strlen(nx_name)-1] = 0;
		log.set("readTag","Path transformed into", nx_name).printLevel(NXING_LOG_DEBUG);

		// Some declaration
		unsigned int nd = 0;				// Position of special character
		int length = 0;			
		int rank = 1;						// Number of dimension of the data
		int dim[NXING_MAX_DIM];				// size of each dimension
		int data_type;						// data type
		char nexus_path[NXING_BIG_SIZE] = ""; // String to hold the neXus path while it is split in its different part.
		char name[NXING_MED_SIZE] = "";		// string to hold the name of the various attribute while looking for the type of the attribute.
		char *vector = 0;					// End of the neXus path that describe what to do with array data.
		char *attr_name;					// Name of the attribute to be read
		void * buff;						// Buffer to store the data from the neXus API.
		
		status = NXING_OK;
		
		// check if it is an attribute. (presence of '.')
		// ****************************
		
		if((nd = strcspn(nx_name, ".") ) < strlen(nx_name)) 
		{
			strncpy(nexus_path, nx_name, nd);
			nexus_path[nd] = 0;
			attr_name = &nx_name[nd+1];
			status = NXopenpath(nxFp, nexus_path);
			if(status != NXING_OK) 
				throw log.set( "readTag", "Can't open the path to element", nexus_path, attr_name);
			status = NXinitattrdir (nxFp);
			while( ((status = NXgetnextattr (nxFp, name, &length, &data_type)) == NXING_OK) && (strcmp(name, attr_name) != 0 ));
			if(status != NXING_OK) 
				throw log.set("readTag", "No attribute with the following name found!", attr_name);
			else 
			{
				dim[0] = length+2;
				status = NXmalloc (&buff, 1, dim, data_type);
				if(status != NXING_OK) throw log.set("readTag", "Can't allocate the buffer", nexus_path, attr_name ,NXING_ERR_BASE_NEXUS-status);
				status = NXgetattr (nxFp, attr_name, buff, &dim[0], &data_type);
				if(status != NXING_OK) throw log.set("readTag", "Can't read the attribute", attr_name, "",NXING_ERR_BASE_NEXUS-status);
			}
		}
		else
		{			
			// Check if is is an element part of an array (presence of '[]' or [x]
			if((nd = strcspn(nx_name, "[") ) < strlen(nx_name))
			{
				strncpy(nexus_path, nx_name, nd);
				nexus_path[nd] = 0;
				vector = &nx_name[nd];
			}
			else
			{
				strcpy(nexus_path,nx_name);
				vector = &nx_name[strlen(nx_name)];
			}
			log.set("readTag", "Get Data ", nexus_path, vector).printLevel(NXING_LOG_DEBUG);
			status = NXopenpath(nxFp, nexus_path);
			if(status != NXING_OK) throw log.set( "readTag", "Can't open the path to element", nexus_path, "");
			status = NXgetinfo (nxFp, &rank, dim, &data_type);
			if(status != NXING_OK) throw log.set("readTag", "Can't collect the info on element", nexus_path, "",NXING_ERR_BASE_NEXUS-status);
			status = NXmalloc (&buff, rank, dim, data_type);
			if(status != NXING_OK) throw log.set("readTag", "Can't allocate the buffer", nexus_path, "",NXING_ERR_BASE_NEXUS-status);
			status = NXgetdata (nxFp, buff); 
			if(status != NXING_OK) throw log.set("readTag", "Can't get the data", nexus_path, "",NXING_ERR_BASE_NEXUS-status);
		}
		const string strval = buff2str(buff,  rank, dim, data_type, vector);
		strcpy(value, strval.c_str());
		log.set("readTag", nx_name, "Return", value).printLevel(NXING_LOG_DEBUG);

		return value;
	}	
	catch(Log log)
	{
		log.printLevel(NXING_LOG_WARNING);
		status = NXING_OK;
		return 0;
	}
}
char* NxClass::getLocation(char *value)
{
	NXinquirefile(nxFp, value, NXING_MED_SIZE); 
	char keys[] = "\\";
	int i= strcspn (value,keys);
	while(i < strlen(value))
	{
		value[i] = '/';
		i = strcspn (value,keys);
	}
	return value;
}


// *****************************************************************************
//	Function : buff2str
//		
//		The function will parse the data buffer according to the content of 
//		vector and return a string.
//		if data_type is NX_CHAR, return the string.
//		if vector is empty, transform the buffer into a number.
//		else transform the buffer into an array of double and apply the 
//		calculation. [AVG] Average, [STD] Standard Deviation, 
//		[MIN]/[MAX] Minimum/Maximum value.
//
// *****************************************************************************
string buff2str(const void* buff, int rank, const int dim[], int data_type, const char* vector)
{
	Log log;
	try 
	{
		// If type NX_CHAR, return the buffer
		// **********************************
		if(data_type == NX_CHAR)
		{
			return string(static_cast<const char*>(buff));
		}
		int vec = 0;
		std::stringstream stream;
		// Parse the vector to check what has to be done with the buffer
		// **************************************************************
		if(vector == 0  || strlen(vector) == 0) vec = Direct;	
		else if ( strcmp(vector, "[]") == 0 || strcmp(vector, "[AVR]") == 0 ) vec = Avg; // Average
		else if (  strcmp(vector, "[MIN]") == 0) vec = Min;		// Min value
		else if (  strcmp(vector, "[MAX]") == 0) vec = Max;		// Max value
		else if (  strcmp(vector, "[STD]") == 0) vec = Std;		// Standard Deviation
		else if (  strcmp(vector, "[SUM]") == 0) vec = Sum;		// Sum of all values
		else 
		{
			vec = atoi(&vector[1]); 
		}	
		// Single Value
		// ************
		if(vec >= 0)
		{
			switch(data_type)
			{
				case NX_INT8	:
					stream << static_cast<int>(static_cast<const char*>(buff)[vec]);
					break;			
				case NX_INT16	:
					stream << static_cast<const int16_t*>(buff)[vec];
					break;
				case NX_INT32	:
					stream << static_cast<const int32_t*>(buff)[vec];
					break;
				case NX_UINT16	:
					stream << static_cast<const uint16_t*>(buff)[vec];
					break;
				case NX_UINT32	:
					stream << static_cast<const uint32_t*>(buff)[vec];
					break;
				case NX_FLOAT32	:
					stream << static_cast<const float*>(buff)[vec];
					break;
				case NX_FLOAT64	:
					stream << static_cast<const double*>(buff)[vec];
					break;
				default	:
					break;
			}	
		} 
		// Array from which a value need to be extracted
		// *********************************************
		else if (vec < Direct)
		{
			// Don't need to look for multi-dimension array. 
			// All data is 1 dimension only.
			// **********************************************
			long num_val = dim[0];
			for(int i =1; i< rank; i++) num_val *= dim[i];
			if(num_val <= 0) { 
				throw log.set("buff2str", "The number of value of the array is null or negative.", "", "" ,NXING_ERR_NEGATIVE_NUMVAL);
			}		// Big mistake
			std::vector<double> dblBuff = std::vector<double>(num_val, 0.0);
			// Transform the array of unknown type into an array of double
			// ***********************************************************
			int i = 0;
			switch(data_type)
			{
				case NX_INT8	:
					for( i = 0; i< num_val; i++)
						dblBuff[i] = static_cast<const char*>(buff)[i];
					break;			
				case NX_INT16	:
					for( i = 0; i< num_val; i++)
						dblBuff[i] = static_cast<const int16_t*>(buff)[i];
					break;			
				case NX_INT32	:
					for( i = 0; i< num_val; i++)
						dblBuff[i] = static_cast<const int32_t*>(buff)[i];
					break;			
				case NX_UINT16	:
					for( i = 0; i< num_val; i++)
						dblBuff[i] = static_cast<const uint16_t*>(buff)[i];
					break;			
				case NX_UINT32	:
					for( i = 0; i< num_val; i++)
						dblBuff[i] = static_cast<const uint32_t*>(buff)[i];
					break;
				case NX_FLOAT32	:
					for( i = 0; i< num_val; i++)
						dblBuff[i] = static_cast<const float*>(buff)[i];
					break;		
				case NX_FLOAT64	:
					for( i = 0; i< num_val; i++)
						dblBuff[i] = static_cast<const double*>(buff)[i];
					break;		
				default	:
					break;
			}
			// Perform the caclulations.
			// *************************
			double num_value = 0.0;
			double avrg = 0.0;	
			int j = 0;			
			switch (vec){
				case Avg :
					for( j = 0; j< num_val; j++) num_value +=  dblBuff[j];
					num_value /= (double)num_val;
					break; 
					
				case Std :
					for( j = 0; j< num_val; j++) avrg +=  dblBuff[j];
					avrg /= (double)num_val;
					num_value = 0;
					for( j = 0; j< num_val; j++) num_value +=  pow((avrg - dblBuff[j]),2);
						num_value = sqrt(num_value/(double)num_val);				
					break; 
					
				case Min : 
					num_value = dblBuff[0];
					for( j = 1; j< num_val; j++) 
						if( dblBuff[j] < num_value)
							num_value = dblBuff[j];				
						break; 
					
				case Max :
					num_value = dblBuff[0];
					for( j = 1; j< num_val; j++) 
						if( dblBuff[j] > num_value) 
							num_value = dblBuff[j];
						break; 				
				case Sum :
					for( j = 0; j< num_val; j++) num_value +=  dblBuff[j];
					break; 
					
				default : 
					break; 
			}
			if((vec == Min || vec == Max || vec == Sum ) && (data_type == NX_INT8 || data_type == NX_INT16 || data_type == NX_INT32 || data_type == NX_UINT16 || data_type == NX_UINT32))
			        stream << static_cast<long long int>(num_value);
			else 
			        stream << num_value;
		}
		return(stream.str());
	}	
	catch(Log log)
	{
		log.printLevel(NXING_LOG_ERROR);
		throw;
	}
	
}
