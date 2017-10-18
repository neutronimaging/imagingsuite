/* =============================================================================
  File Name : 	nxingest_parse.h
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
#ifndef _NXING_PARSE_HEADER_
#define _NXING_PARSE_HEADER_

#include "nxingest.h"
#include "nxingest_debug.h"
#include "nxingest_utils.h"
#include "nxingest_time.h"

#include "nxingest_nexus.h"
#define NXING_UNEXPECTED_PARAM_VALUE	NXING_ERR_BASE_PARSE -1

mxml_node_t *parseXml(mxml_node_t *inNode, mxml_node_t *topNode, mxml_node_t **outNode, NxClass nx);

mxml_node_t * readRecord(mxml_node_t *inNode, mxml_node_t **outNode, NxClass nx);

mxml_node_t * readKeyword(mxml_node_t *inNode, mxml_node_t **outNode, NxClass nx);

mxml_node_t * readParam(mxml_node_t *inNode, mxml_node_t **outNode, NxClass nx, int paramType);

char* parseMix(char* in, char* str, NxClass nx);
char* parseSpecial(char* in, char* str, NxClass nx);

char* getValue(mxml_node_t *inNode, NxClass nx, char* str);

#endif
