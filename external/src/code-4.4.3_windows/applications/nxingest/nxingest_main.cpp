/* =============================================================================
  File Name : 	nxingest_main.cpp
  Version   : 	1.7
  Component : 	nxingest
  Developer : 	Laurent Lerusse 
				e-Science Center - Facility Support - Data Management Group
				       
  Purpose   : 	nxingest extract the metadata from a NeXus file to create an 
				XML file according to a mapping file. 

  Revision History : 
		Version 1.0		05/06/2007		First version. 
		Version 1.1		13/08/2007
			Windows compatibility : Remove stpttime function from Date class. 
			Modify some Declaration in NeXus class.

		Version 1.5		5/09/2007
			To avoid wrapping the xml document, 
			set constnty 'MXML_WRAP' to a high value in mxml.h (under linux) or 
			under windows, add function mxmlSetWrapMargin(0);
		
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
#include "nxingest_main.h"

  /*
     * 'whitespace_cb()' - Let the mxmlSaveFile() function know when to insert
     *                     newlines and tabs...
     */

    const char *                            /* O - Whitespace string or NULL */
    whitespace_cb(mxml_node_t *node,        /* I - Element node */
        	  int         where)        /* I - Open or close tag? */
{
	if (strncmp(node->value.element.name, "?xml", 4) == 0)
		return(NULL);
	if (where == MXML_WS_BEFORE_OPEN )
		return ("\n");
	if (where == MXML_WS_AFTER_OPEN )
		return (NULL);
	if (where == MXML_WS_BEFORE_CLOSE )
		return (NULL);
	if (where == MXML_WS_AFTER_CLOSE )
		return (NULL);
	return (NULL);
}

int main (int argc, char *argv[])
{
	Log log;
	FILE *inFp = 0;
	FILE *outFp = 0; 
	mxml_node_t 	*outTree = 0;	// hold the xml input to ICAT
	mxml_node_t 	*inTree = 0;	// Hold the parameter list
	try
	{
		// Check if my assumption of type size are correct. 
		// ************************************************

		if(sizeof(short) != 2 || sizeof(int) != 4 )
			log.set("Compiler Test", "The integer sizes are not as expected", "(short 2 bytes; int 4 bytes)").printLevel(NXING_LOG_WARNING);
		if(sizeof(float) != 4 ||sizeof(double) != 8)
			log.set("Compiler Test", "The float sizes are not as expected", "(float 4 bytes; double 8 bytes)").printLevel(NXING_LOG_WARNING);

		char mappingFl[NXING_BIG_SIZE] = "";	
		char nexusFl[NXING_BIG_SIZE]   = "";
		char outputFl[NXING_BIG_SIZE]  = "";	
		
		if(argc < 3)
		{
			throw log.set("main", "Not enough input parameters!", "Needs 2 input files, mapping file and the NeXus file.", "And one output file.", NXING_ERR_WRONG_INPUT);
		}
		else 
		{
			if(argv[1] != 0) strcpy(mappingFl, argv[1]);	
			if(argv[2] != 0) strcpy(nexusFl, argv[2]);
			if(argv[3] != 0) strcpy(outputFl, argv[3]);
			else strcpy(outputFl, "output.xml");	
		}
		log.set("main", "input - mapping", mappingFl).printLevel(NXING_LOG_NORMAL);
		log.set("main", "input - nexus  ", nexusFl).printLevel(NXING_LOG_NORMAL);
		log.set("main", "input - output ", outputFl).printLevel(NXING_LOG_NORMAL);
		
		
		// Read input XML Parameters. 
		// **************************'
#ifndef MXML_WRAP
		mxmlSetWrapMargin(0);
#endif
		inFp = fopen(mappingFl, "r");
		if( inFp == 0 ) throw log.set("main", "Can't open the parameter file!", mappingFl, "", NXING_ERR_CANT_OPEN_PARAM);
		mxml_node_t 	*inNode;	// hold the node to read from.
		inTree = mxmlLoadFile(NULL, inFp, MXML_TEXT_CALLBACK);
		fclose(inFp);
		inFp = 0;
		if(inTree != 0) log.set("main", "The mapping file has been read!", mappingFl).printLevel(NXING_LOG_ALL);

		
		// Create XML output to ICAT. 
		// **************************
		outFp = fopen(outputFl, "w");
		if( outFp == 0 ) throw log.set("main", "Can't open the output file!", outputFl, "", NXING_ERR_CANT_OPEN_OUTPUT);
	
		if(inTree->type == MXML_ELEMENT  && (strncmp(inTree->value.element.name, "?xml", 4) == 0)){	
			outTree = mxmlNewElement(MXML_NO_PARENT,  inTree->value.element.name);
		}
		
		log.set("main", "Output Created, first tag added!", inTree->value.element.name).printLevel(NXING_LOG_DEBUG);
		//

		// Open the neXus file.
		// ********************
		NxClass nx(nexusFl);
		//if(nx.isNotOK) throw log.set("Can't open the neXus file!", nexusFl, nx.status);
		log.set("main", "NeXµs file read!", nexusFl, "", nx.status).printLevel(NXING_LOG_DEBUG);

		/*
	 	 * Parse the parameter file, read the neXus file and populate the XML output. 
		 */
		 log.set("main", "Parsing the mapping file!").printLevel(NXING_LOG_DEBUG);
		 inNode = mxmlWalkNext(inTree, inTree, MXML_DESCEND);
		 parseXml(inNode, inTree, &outTree, nx);

	
		/* 
		 * Save the output file
		 */  	
		mxmlSaveFile(outTree, outFp, whitespace_cb); 
		log.set("main", "Output file Saved!", outputFl).printLevel(NXING_LOG_DEBUG);

		/* 
		 * Close the files
		 */
		fclose(outFp);
		/*
	     * Delete the xml Trees
	     */
		mxmlDelete(inTree);
	 	mxmlDelete(outTree);
		exit(0);
	}
	catch(Log log)
	{
		log.printLevel(NXING_LOG_ERROR);
		if(inFp  != 0) fclose(inFp);		
		if(outFp != 0) fclose(outFp);
		
		if(inTree  != 0) mxmlDelete(inTree);
	 	if(outTree != 0) mxmlDelete(outTree);
		exit(0);
	}	
}
	
