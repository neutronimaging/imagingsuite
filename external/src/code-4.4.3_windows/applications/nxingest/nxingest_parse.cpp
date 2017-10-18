/* =============================================================================
  File Name : 	nxingest_parse.cpp
  Version   : 	1.8
  Component : 	nxingest
  Developer : 	Laurent Lerusse 
				e-Science Center - Facility Support - Data Management Group
				       
  Purpose   : 	nxingest extract the metadata from a NeXus file to create an 
				XML file according to a mapping file. 

  Revision History : 
		Version 1.0		05/06/2007		First version. 
		
		Version 1.1		13/06/2007		Remove trailing white space around string. 
		
		Version 1.3		31/08/2007		
			Bug correction in test. (Logical OR and not bitwise OR)
			Add a filesize calculation function. 

		Version 1.4		04/09/2007	
			The parameters will not be created if there is no value attached to it. 
			
		Version 1.6		06/09/2007	
			Bug correction. Connected to changes of version 1.4. 
			Bug Correction. Difference of path between linux / and windows \
		
		Version 1.7		12/09/2007
			Bug correction in parseMix function when parseSpecial return a null string.
		
		Version 1.8		12/09/2007
			Add the capability to split a string according to ' ', ',' and ';' 
			to store it in several tag with the same name e.g. keyword. 
			The mapping is like that : 
				<keyword type="keyword_tag">
					<icat_name> name </icat_name>
					<value type="mix">  nexus:/{NXentry}/title | fix: , | nexus:/{NXentry}/notes </value>
				</keyword>			
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
#include "nxingest_parse.h"

enum param_type { STR, NUM};
// *****************************************************************************
//	Function : parseXml
//		
//		This function will parse the mapping file to find the different elements.
//		It may be the name of a sub table, the user tables that need to be 
//		parsed once for each user in the neXus file. a tag or a parameter.
//		The tag and the parameters are the element which will be holding the 
//		metadata. 
// 
// *****************************************************************************

mxml_node_t *parseXml(mxml_node_t *inNode, mxml_node_t *topNode, mxml_node_t **outNode, NxClass nx)
{
	
	Log log;
	mxml_node_t 	*inNextNode = 0;
	mxml_node_t 	*outNextNode;
	mxml_node_t 	*userInNode;
	mxml_node_t 	**userOutNode;

	int type_descent = MXML_DESCEND;		
	const char 		*type;
		
	while( (inNode = mxmlWalkNext(inNode, topNode, type_descent)) != NULL )
	{
		inNextNode = inNode; // Get the last inNode to pass back to the calling function
		if(inNode->type == MXML_ELEMENT)
		{
			type_descent = MXML_DESCEND;

			type = mxmlElementGetAttr(inNode, "type");
			if(type != 0) 
			{	
				log.set("parseXml", "Found element : ", inNode->value.element.name).printLevel(NXING_LOG_DEBUG);
				if(strcmp(type, "tbl") == 0 ) // Simple table.
				{
					log.set("parseXml", "Get new table", inNode->value.element.name).printLevel(NXING_LOG_DEBUG);
					outNextNode = mxmlNewElement(*outNode, inNode->value.element.name );
					// add the attributes of the input node : 
					if(inNode->value.element.num_attrs > 1)
						for(int i = 0; i< inNode->value.element.num_attrs; i++)
							if(strcmp(inNode->value.element.attrs[i].name, "type") != 0)
								mxmlElementSetAttr(outNextNode, inNode->value.element.attrs[i].name, inNode->value.element.attrs[i].value);
					inNode = parseXml(inNode, inNode, &outNextNode, nx);
				} 
				else if(strcmp(type, "user_tbl") == 0 )	// User table. There may be several user. Need to loop over the different NXusers.
				{
					userInNode = inNode;
					userOutNode = outNode;
					do{
						userInNode = inNode;

						log.set("parseXml", type, "Start", inNode->value.element.name).printLevel(NXING_LOG_DEBUG);
						outNextNode = mxmlNewElement(*userOutNode, userInNode->value.element.name );
						if(userInNode->value.element.num_attrs > 1)
							for(int i = 0; i< userInNode->value.element.num_attrs; i++)
								if(strcmp(userInNode->value.element.attrs[i].name, "type") != 0)
									mxmlElementSetAttr(outNextNode, userInNode->value.element.attrs[i].name, userInNode->value.element.attrs[i].value);

						log.set("parseXml", type, "Element added",userInNode->value.element.name).printLevel(NXING_LOG_DEBUG);
						parseXml(userInNode, topNode, &outNextNode, nx);
						log.set("parseXml", type, "Parsed", inNode->value.element.name).printLevel(NXING_LOG_DEBUG);

					}while(nx.nextUser() != -1); 
					inNode = mxmlWalkNext(inNode, topNode, MXML_NO_DESCEND); 
				}
				else if(strcmp(type, "keyword_tag") == 0 ) // simple Tag record
				{
					log.set("parseXml", type, "Read",inNode->value.element.name).printLevel(NXING_LOG_DEBUG);
					inNode = readKeyword(inNode, outNode, nx);
					type_descent = MXML_NO_DESCEND;
				}
				else if(strcmp(type, "tag") == 0 ) // simple Tag record
				{
					log.set("parseXml", type, "Read",inNode->value.element.name).printLevel(NXING_LOG_DEBUG);
					inNode = readRecord(inNode, outNode, nx);
					type_descent = MXML_NO_DESCEND;
				}
				else if(strcmp(type, "param_str") == 0 ) // Parameter with a string value
				{
					log.set("parseXml", type, "Read", inNode->value.element.name).printLevel(NXING_LOG_DEBUG);
					inNode = readParam(inNode, outNode, nx, STR);
					type_descent = MXML_NO_DESCEND;
				}
				else if(strcmp(type, "param_num") == 0 ) // Parameter with a numeric value
				{
					log.set("parseXml", type, "Read",inNode->value.element.name).printLevel(NXING_LOG_DEBUG);
					inNode = readParam(inNode, outNode, nx, NUM);
					type_descent = MXML_NO_DESCEND;

				}
			}
		}
		if(inNode->type == MXML_TEXT && strlen(inNode->value.text.string) > 0)
		{
			log.set("parseXml", "Unexpected text (may be comments)", inNode->value.text.string).printLevel(NXING_LOG_DEBUG);
		}
	}
	return (inNextNode);
}

// *****************************************************************************
//	Function : readRecord
//		
//		This function will read a Tag that will end up as <name> value </name>
//		where name is the icat_name value from the mapping file and value
//		either the default value or the value from the neXus file 
//		or a special value (still to be implemented).
// 
// *****************************************************************************
mxml_node_t * readRecord(mxml_node_t *inNode, mxml_node_t **outNode, NxClass nx)
{
	// Check that this is the correct inNode for the function. 
	Log log;
	mxml_node_t 	*inNextNode = inNode;
	mxml_node_t 	*topNode	= inNode;
	mxml_node_t 	*nextNode;
	char name[NXING_BIG_SIZE]  = "";
	char value[NXING_BIG_SIZE] = "";

	// 
	while( (inNode = mxmlWalkNext(inNode, topNode, MXML_DESCEND)) != NULL )
	{
		if(inNode->type == MXML_ELEMENT)
		{
			if(strcmp(inNode->value.element.name, "icat_name") == 0) 
			{				
				log.set("readRecord", "ICAT name",mxmlGetItem(inNode, name)).printLevel(NXING_LOG_DEBUG);
				mxmlGetItem(inNode, name);
				log.set("readRecord", "ICAT name",name).printLevel(NXING_LOG_DEBUG);
			} 
			else if(strcmp(inNode->value.element.name, "value") == 0) 
			{
				getValue(inNode, nx, value);
			}
		}
	}
	if(strlen(name) > 0 && strlen(value) > 0)
	{
		nextNode = mxmlNewElement(*outNode, name );	
		mxmlNewText( nextNode, 0, value);
	}
	
	return inNextNode;
}
mxml_node_t * readKeyword(mxml_node_t *inNode, mxml_node_t **outNode, NxClass nx)
{
	// Check that this is the correct inNode for the function. 
	Log log;
	mxml_node_t 	*inNextNode = inNode;
	mxml_node_t 	*topNode	= inNode;
	mxml_node_t 	*nextNode;
	char name[NXING_BIG_SIZE]  = "";
	char value[NXING_BIG_SIZE] = "";
	char keywordStr[NXING_BIG_SIZE] = "";
	char keys[] = " ,;";

	// 
	strcpy(keywordStr , inNode->value.element.name);
	
	while( (inNode = mxmlWalkNext(inNode, topNode, MXML_DESCEND)) != NULL )
	{
		if(inNode->type == MXML_ELEMENT)
		{
			if(strcmp(inNode->value.element.name, "icat_name") == 0) 
			{				
				log.set("readRecord", "ICAT name",mxmlGetItem(inNode, name)).printLevel(NXING_LOG_DEBUG);
				mxmlGetItem(inNode, name);
				log.set("readRecord", "ICAT name",name).printLevel(NXING_LOG_DEBUG);
			} 
			else if(strcmp(inNode->value.element.name, "value") == 0) 
			{
				getValue(inNode, nx, value);
			}
		}
	}
	
	if(strlen(name) > 0 && strlen(value) > 0)
	{
		char* str;
		char* nextStr;
		int i = 0;
		nextStr = &value[0];
		do
		{
			str = nextStr;
			i = strcspn (str,keys);
			if(i < strlen(str)) 
			{
				nextStr = &str[i+1];
				str[i] = 0;
			}	
			else 
				nextStr = 0;
			if(strlen(str) > 0 && strcmp(str, "=") != 0)
			{	
				nextNode = mxmlNewElement(*outNode, keywordStr );	
				nextNode = mxmlNewElement(nextNode, name );	
				mxmlNewText( nextNode, 0, str);
			}
		} while( nextStr != 0 );
	}
	return inNextNode;
}


// *****************************************************************************
//	Function : readParam
//		
//		This function will read a Tag that will end up as 
//		<parameter>
//			<name> icat_name </name>
//			<string_value|numeric_value> value </string_value|numeric_value>
//			<units>	value_units	</units>
//			<description>	value_description	</description>
//		where name is the icat_name value from the mapping file and value
//		either the default value or the value from the neXus file 
//		or a special value (still to be implemented).
// 
// *****************************************************************************
mxml_node_t * readParam(mxml_node_t *inNode, mxml_node_t **outNode, NxClass nx, int paramType)
{
	// Check that this is the correct inNode for the function. 
	Log log;
	mxml_node_t 	*inNextNode = inNode;
	mxml_node_t 	*topNode	= inNode;
	mxml_node_t 	*paramNode;
	mxml_node_t 	*nextNode;
	char str[NXING_BIG_SIZE] = "";
	char* buff = 0;
	
	string name;
	string value;
	string units;
	string description;
	
	while( (inNode = mxmlWalkNext(inNode, topNode, MXML_DESCEND)) != NULL )
	{
		if(inNode->type == MXML_ELEMENT)
		{
			if(strcmp(inNode->value.element.name, "icat_name") == 0) 
			{				
				log.set("readParam", "ICAT name", mxmlGetItem(inNode, str)).printLevel(NXING_LOG_DEBUG);
				buff = mxmlGetItem(inNode, str);					
				if(buff != 0) name = buff;
				else name = "";			} 
			else if(strcmp(inNode->value.element.name, "value") == 0) 
			{
				log.set("readParam", "Value ", mxmlGetItem(inNode, str)).printLevel(NXING_LOG_DEBUG);
				buff = getValue(inNode, nx, str);
				if(buff != 0) value = buff;
				else value = "";
			}
			else if(strcmp(inNode->value.element.name, "units") == 0) 
			{
				log.set("readParam", "Units", mxmlGetItem(inNode, str)).printLevel(NXING_LOG_DEBUG);
				buff = getValue(inNode, nx, str);
				if(buff != 0) units = buff;
				else units = "";
			} 
			else if(strcmp(inNode->value.element.name, "description") == 0) 
			{
				log.set("readParam", "Description", mxmlGetItem(inNode, str)).printLevel(NXING_LOG_DEBUG);
				buff = getValue(inNode, nx, str);
				if(buff != 0) description = buff;
				else description = "";
			} 
		}
	}
	if(value.size() > 0 && name.size() > 0) 
	{ 
		paramNode = mxmlNewElement(*outNode, "parameter" );	

		// Name
		nextNode = mxmlNewElement(paramNode, "name" );
		mxmlNewText( nextNode, 0, name.c_str());
		// Value
		if(paramType == STR)
			nextNode = mxmlNewElement(paramNode, "string_value" );	
		else 
			nextNode = mxmlNewElement(paramNode, "numeric_value" );	
		mxmlNewText( nextNode, 0, value.c_str());
		// Units	
		nextNode = mxmlNewElement(paramNode, "units" );	
		if(units.size() > 0)
			mxmlNewText( nextNode, 0, units.c_str());
		else
			mxmlNewText( nextNode, 0, "N/A");
		// Description
		if(description.size() > 0)
		{
			nextNode = mxmlNewElement(paramNode, "description" );	
			mxmlNewText( nextNode, 0, description.c_str());
		}
	}
	return inNextNode;
}
// *****************************************************************************
//	Function : getValue
//		
// *****************************************************************************
char* getValue(mxml_node_t *inNode, NxClass nx, char* str)
{
	char buff[NXING_BIG_SIZE] = "";
	const char *type;
	type = mxmlElementGetAttr(inNode, "type");
	if(strncmp(type, "nexus", 5) == 0) 
		str = nx.readTag(mxmlGetItem(inNode, (char*)buff), str, -1) ;
	else if(strncmp(type, "fix", 3) == 0) 
		str = mxmlGetItem(inNode, str) ;
	else if(strncmp(type, "special", 6) == 0) 
		str = parseSpecial( mxmlGetItem(inNode, buff), str , nx) ;
	else if(strncmp(type, "mix", 3) == 0) 
		str =  parseMix( mxmlGetItem(inNode, buff), str , nx);
	else strcpy(str, "Unknown type");
	// Remove trailing white spaces.
	while(str != 0 && str[strlen(str)-1] == ' ') str[strlen(str)-1] = 0;
	return str;
}

// *****************************************************************************
//	Function : parseSpecial
//		
// *****************************************************************************
char* parseSpecial(char* in, char* str, NxClass nx)
{
	Log log;
	char buff[NXING_MED_SIZE];
	try 
	{
		if(strncmp(in, "time:", 5) == 0)
		{
			Time tt;
			char * pch;
			char timeCntr[3][NXING_MED_SIZE] = { "", "0", "0"};
			int i=0;
			pch = strtok (&in[5],"; ");
			while (pch != NULL)
			{
				if(i<3) strncpy(timeCntr[i], pch, NXING_MED_SIZE);
				i++;
				pch = strtok (NULL, ";");
			}
			int inType = atoi(timeCntr[1]);		// Type in which the time is read
			int outType = atoi(timeCntr[2]);	// Time in which the time is written

			if(strcmp(timeCntr[0], "now") == 0)
			{
				str = tt.now().getTime(str, inType);
			}
			else if(strncmp(timeCntr[0], "nexus(", 6) == 0)
			{
				timeCntr[0][strlen(timeCntr[0])-1] = 0;
				char * nxStr = 0;
				nxStr = nx.readTag(&timeCntr[0][6], (char*)buff, -1);
				if( nxStr != 0)
					str = tt.set(nxStr, inType).getTime(str, outType);
				else
					str = 0;
			}

		}
		else if (strncmp(in, "fix:", 4) == 0)
		{
			return(&in[4]);
		}
		else if (strncmp(in, "nexus:", 6) == 0)
		{
			str = nx.readTag(&in[6], str, -1);
		}
		else if (strncmp(in, "sys:", 4) == 0)
		{
			if(strncmp(&in[4], "filename", 8) == 0)
			{
				string location = nx.getLocation(str);;
				size_t found = location.find_last_of("/\\");
				if(found == string::npos) found = 0;
				strcpy(str, location.substr(found+1).c_str());
			}
			else if(strncmp(&in[4], "location", 8) == 0)
			{
				str = nx.getLocation(str);
			}
			else if(strncmp(&in[4], "size", 4) == 0)
			{
				str = nx.getLocation(str);
				long fSize = fileSize(str);
				sprintf( str, "%ld", fSize);
			}			
		}
		else
			str = 0;
		
		return str;
	}	
	catch(Log log)
	{
		log.printLevel(NXING_LOG_ERROR);
		return 0;
	}
}



// *****************************************************************************
//	Function : parseMix
//		
//		This function will read a Tag that will end up as 
//		<parameter>
//			<name> icat_name </name>
//			<string_value|numeric_value> value </string_value|numeric_value>
//			<units>	value_units	</units>
//			<description>	value_description	</description>
//		where name is the icat_name value from the mapping file and value
//		either the default value or the value from the neXus file 
//		or a special value (still to be implemented).
// 
// *****************************************************************************
char* parseMix(char* in, char* str, NxClass nx)
{
	Log log;
	char *buff = new char[NXING_MED_SIZE];
	char hardpch[32][NXING_MED_SIZE];
	int numMix = 0;
	strcpy(str, "");
	char * pch = new char[NXING_BIG_SIZE];
	pch = strtok (in, "|");
	numMix = 0;
	while (pch != NULL)
	{
		while(pch[0] != 0 && pch[0] == ' ') pch = &pch[1];
		while(strlen(pch) > 0 && pch[strlen(pch)-1] == ' ') pch[strlen(pch)-1] = 0;
		strcpy(hardpch[numMix], pch);
		numMix++;
		pch = strtok (NULL, "|");
	}
	for(int i = 0 ; i < numMix; i++)
	{
		buff = parseSpecial(hardpch[i], (char*)buff, nx);
		if(buff != 0 )
			strcat(str, buff);
	}
	return str;
}


