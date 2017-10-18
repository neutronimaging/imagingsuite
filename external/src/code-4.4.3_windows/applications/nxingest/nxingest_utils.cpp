/* =============================================================================
  File Name : 	nxingest_utils.cpp
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
#include "nxingest_utils.h"

/*	Version using stat.h which is not portable. 

	long fileSize( const string &fname ) 
	{ 
	  struct stat fileStat; 
	  int err = stat( fname.c_str(), &fileStat ); 
	  if (0 != err) return 0; 
	  return fileStat.st_size; 
	}
*/

#include <fstream>
long fileSize( const string &fname ) 
{ 
/* 
 Function taken from : http://www.codeproject.com/file/filesize.asp
 Reported pitfals : 
    * The file size may be bigger than can be represented by an int.
    * The size of the file may be larger than what is reported. 
*/
  std::ifstream f;
  f.open(fname.c_str(), std::ios_base::binary | std::ios_base::in);
  if (!f.good() || f.eof() || !f.is_open()) { return 0; }
  f.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f.tellg();
  f.seekg(0, std::ios_base::end);
  return static_cast<int>(f.tellg() - begin_pos);
}



// *****************************************************************************
//	Function : mxmlGetItem
//		
//		The function will read the text of an xml tag and return it. 
//		If a string is provided, multiple word will be concatenated 
//		before returning the string.
//
// *****************************************************************************
char* mxmlGetItem(mxml_node_t *node, char* str){
	mxml_node_t 	*topNode;
	topNode = node;	
	
	strcpy(str, "");
	while( (node = mxmlWalkNext(node, topNode, MXML_DESCEND)) != NULL  && node->type == MXML_TEXT)
	{
		strcat(str, node->value.text.string);
		strcat(str, " ");
	}
	while(strlen(str) > 0 && str[strlen(str)-1] == ' ')
		str[strlen(str)-1] = 0;
	return str;
}

char* mxmlGetItem(mxml_node_t *node){
	mxml_node_t 	*topNode;
	topNode = node;	
	while( (node = mxmlWalkNext(node, topNode, MXML_DESCEND)) != NULL )
	{
		if(node->type == MXML_TEXT && strlen(node->value.text.string) > 0)
		{
			return node->value.text.string; 
		}
	}
	return 0;
}
