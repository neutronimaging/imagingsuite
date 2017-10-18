/*-----------------------------------------------------------------------------
  NeXus - Neutron & X-ray Common Data Format
   
  Utility to convert a NeXus file into HDF4/HDF5/XML/...
 
  Author: Freddie Akeroyd, Ray Osborn
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
  For further information, see <http://www.nexusformat.org>
 
 $Id: nxconvert.c 991 2008-03-19 19:30:03Z Freddie Akeroyd $
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#ifdef _MSC_VER
#else
#include <unistd.h>
#endif
#include "napi.h"
#include "NeXusFile.hpp"
#include "NeXusStream.hpp"
#include "nxconvert_common.h"

#include <vector>

static int WriteGroup (int is_definition);
static int WriteAttributes (int is_definition, int is_group);

static bool is_valid_sds_name(const char* name)
{
    static const char* invalid_sds_names[] = { "Dim0.0", "UDim0.0", "CDF0.0", "Var0.0", "RIG0.0", "RI0.0" };
    for(unsigned i=0; i<sizeof(invalid_sds_names)/sizeof(const char*); ++i)
    {
	if (!strcmp(name, invalid_sds_names[i]))
	{
	    return false;
	}
    }
    return true;
}


static void clean_string(void* dataBuffer, int dataRank, int dataDimensions[])
{
	int i, n = 1;
    	for(i=0; i<dataRank; ++i)
    	{
	    n *= dataDimensions[i];
 	}
	for(i=0; i<n; ++i)
	{
	    if (!isprint(((const unsigned char*)dataBuffer)[i]))
	    {
		((char*)dataBuffer)[i] = '?';
	    }
	}
}

struct link_to_make 
{
    char from[1024];   /* path of directory with link */
    char name[256];    /* name of link */
    char to[1024];     /* path of real item */
    link_to_make(const char* _from, const char* _name, const char* _to)
    {
	strcpy(from, _from);
	strcpy(name, _name);
	strcpy(to, _to);
    }
    link_to_make(const link_to_make& val)
    {
	strcpy(from, val.from);
	strcpy(name, val.name);
	strcpy(to, val.to);
    }
    link_to_make& operator=(const link_to_make& val)
    {
	if (this != &val)
	{
	    strcpy(from, val.from);
	    strcpy(name, val.name);
	    strcpy(to, val.to);
	}
	return *this;
    }
};

static std::vector<link_to_make> links_to_make;

static char current_path[1024];

static int add_path(const char* path)
{
    int i;
    if (path != NULL)
    {
        i = strlen(current_path);
        sprintf(current_path + i, "/%s", path);
    }
    return 0;
}

static int remove_path(const char* path)
{
    char *tstr; 
    tstr = strrchr(current_path, '/');
    if (path != NULL && tstr != NULL && !strcmp(path, tstr+1))
    {
	*tstr = '\0';
    }
    else
    {
	printf("path error\n");
    }
    return 0;
}

static NXhandle inId, outId;
static const char* definition_name = NULL;

int convert_file(int nx_format, const char* inFile, int nx_read_access, const char* outFile, int nx_write_access, const char* definition_name_)
{
   int nx_is_definition = 0;
   if (definition_name_ != NULL && definition_name_[0] == '\0') {
     definition_name = NULL;
   } else {
     definition_name = definition_name_;
   }
   char* tstr;
   links_to_make.clear();
   links_to_make.reserve(2000);
   current_path[0] = '\0';
   NXlink link;
   if (nx_format == NX_DEFINITION)
   {
	nx_is_definition = 1;
	char env_var[] = "NX_IS_DEFINITION=1";
	putenv(env_var);
   }
/* Open NeXus input file and NeXus output file */
   if (NXopen (inFile, nx_read_access, &inId) != NX_OK) {
      printf ("NX_ERROR: Can't open %s\n", inFile);
      return NX_ERROR;
   }

   if (NXopen (outFile, nx_write_access, &outId) != NX_OK) {
      printf ("NX_ERROR: Can't open %s\n", outFile);
      return NX_ERROR;
   }

/* Output global attributes */
   if (WriteAttributes (nx_is_definition, 1) != NX_OK)
   {
	return NX_ERROR;
   }
/* Recursively cycle through the groups printing the contents */
   if (WriteGroup (nx_is_definition) != NX_OK)
   {
	return NX_ERROR;
   }
/* close input */
   if (NXclose (&inId) != NX_OK)
   {
	return NX_ERROR;
   }
/* now create any required links */
       for(size_t i=0; i<links_to_make.size(); i++)
       {
	    if (NXopenpath(outId, links_to_make[i].to) != NX_OK) return NX_ERROR;
	    if (NXgetdataID(outId, &link) == NX_OK  || NXgetgroupID(outId, &link) == NX_OK)
	    {	
	        if (NXopenpath(outId, links_to_make[i].from) != NX_OK) return NX_ERROR;
	        tstr = strrchr(links_to_make[i].to, '/');
		if (tstr != NULL)
		{
	            if (!strcmp(links_to_make[i].name, tstr+1))
	            {
	                if (NXmakelink(outId, &link) != NX_OK) return NX_ERROR;
	            }
	            else
	            {
	                if (NXmakenamedlink(outId, links_to_make[i].name, &link) != NX_OK) return NX_ERROR;
	            }
		}
	    }
	    else
	    {
	        return NX_ERROR;
	    }
	}
/* Close the input and output files */
   if (NXclose (&outId) != NX_OK)
   {
	return NX_ERROR;
   }
   return NX_OK;
}

/* Test if this is really a NX_CHAR in disguise */
static int testForText(char *name)
{
  int count = 0;

  static const char *txtNames[] = {
    "name",
    "type",
    "title",
    "start_time",
    "end_time",
    "sample_name",
    "CounterMode",
    "count_mode",
    NULL
  };

  while(txtNames[count] != NULL){
    if(strcmp(txtNames[count],name) == 0) {
      return 1;
    }
    count++;
  }
  
  return 0;
}

/* Prints the contents of each group as XML tags and values */
static int WriteGroup (int is_definition)
{ 
  
  int i,  status, dataType, dataRank, dataDimensions[NX_MAXRANK], testString; 
  long dataSize;
   static const int slab_start[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   static const int MAX_DEF_ARRAY_ELEMENTS_PER_DIM = 3; /* doesn't work yet - only 1 element is written */
   NXname name, nxclass;
   void *dataBuffer;
   NXlink link;
   std::string definition;
   using namespace NeXus;
   using namespace NeXus::Stream;
   File nfile_in(inId), nfile_out(outId);
   char *ptr = NULL;

   do {
      status = NXgetnextentry (inId, name, nxclass, &dataType);
      if (status == NX_ERROR) return NX_ERROR;
      if (status == NX_OK) {
//         std::cerr << "WriteGroup: " << name << "(" << nxclass << ")" << std::endl;
         if (!strncmp(nxclass,"SDS",3)) {
	    add_path(name);
	    testString = 0;
            if (NXopendata (inId, name) != NX_OK) return NX_ERROR;
	    if (NXgetdataID(inId, &link) != NX_OK) return NX_ERROR;
	    if (!strcmp(current_path, link.targetPath))
	    {
                if (NXgetinfo (inId, &dataRank, dataDimensions, &dataType) != NX_OK) return NX_ERROR;
		if((dataType == NX_INT8 || dataType == NX_UINT8) &&  testForText(name)){
		  /*
		    This is really meant to be a NX_CHAR
		  */
		  if(dataDimensions[0] < 8) {
		    dataDimensions[0] = 8;
		  }
		  if (NXmakedata (outId, name, NX_CHAR, dataRank, dataDimensions) != NX_OK) return NX_ERROR;
		  testString = 1;
		} else {
		  for(i = 0, dataSize  = 1; i < dataRank; i++){
		    dataSize *= dataDimensions[i];
		  }
		  if(dataSize > 100){
		    if (NXcompmakedata (outId, name, dataType, dataRank, dataDimensions, NX_COMP_LZW,dataDimensions) != NX_OK) return NX_ERROR;
		  } else {
		    if (NXmakedata (outId, name, dataType, dataRank, dataDimensions) != NX_OK) return NX_ERROR;
                  }
		}
                if (NXopendata (outId, name) != NX_OK) return NX_ERROR;
		if ( is_definition && (dataType != NX_CHAR) )
		{
		    for(i=0; i<dataRank; ++i)
		    {
			if (dataDimensions[i] > MAX_DEF_ARRAY_ELEMENTS_PER_DIM)
			{
			    dataDimensions[i] = MAX_DEF_ARRAY_ELEMENTS_PER_DIM;
			}
		    }
                    if (NXmalloc (&dataBuffer, dataRank, dataDimensions, dataType) != NX_OK) return NX_ERROR;
                    if (NXgetslab (inId, dataBuffer, slab_start, dataDimensions)  != NX_OK) return NX_ERROR;
                    if (NXputslab (outId, dataBuffer, slab_start, dataDimensions) != NX_OK) return NX_ERROR;
		}
		else
		{
                    if (NXmalloc (&dataBuffer, dataRank, dataDimensions, dataType) != NX_OK) return NX_ERROR;
                    if (NXgetdata (inId, dataBuffer)  != NX_OK) return NX_ERROR;
		    /* fix potential non-UTF8 character issue */
		    if (is_definition && dataType == NX_CHAR)
		    {
			clean_string(dataBuffer, dataRank, dataDimensions);
		    }
		    /* fix quasi empty strings */
		    if(testString){
		      ptr = (char *)dataBuffer;
		      if(strlen(ptr) < 3) {
			NXfree((void**)&dataBuffer);
			dataBuffer = strdup("Unknown");
		      }
                    }
                    if (NXputdata (outId, dataBuffer) != NX_OK) return NX_ERROR;
		}
                if (WriteAttributes (is_definition, 0) != NX_OK) return NX_ERROR;
                if (NXclosedata (outId) != NX_OK) return NX_ERROR;
                if (NXfree((void**)&dataBuffer) != NX_OK) return NX_ERROR;
	        remove_path(name);
	    }
	    else
	    {
	        remove_path(name);
		links_to_make.push_back(link_to_make(current_path, name, link.targetPath));
	    }
            if (NXclosedata (inId) != NX_OK) return NX_ERROR;
         }
         /* napi4.c returns UNKNOWN for DFTAG_VH in groups */
         else if (!strcmp(nxclass, "UNKNOWN") || !is_valid_sds_name(nxclass)) {
             ;
         }
         else {
            if (NXopengroup (inId, name, nxclass) != NX_OK) return NX_ERROR;
	    add_path(name);
	    if (NXgetgroupID(inId, &link) != NX_OK) return NX_ERROR;
	    if (!strcmp(current_path, link.targetPath))
	    {
                if (NXmakegroup (outId, name, nxclass) != NX_OK) return NX_ERROR;
                if (NXopengroup (outId, name, nxclass) != NX_OK) return NX_ERROR;
                if (WriteAttributes (is_definition, 1) != NX_OK) return NX_ERROR;
		if (is_definition && !strcmp(nxclass, "NXentry"))
		{
		    if (definition_name != NULL)
		    {
		        nfile_out.putAttr("xsi:type", definition_name);
		    }
  		    else
		    {
		        try {
			    nfile_in.openData("definition");
		            definition = nfile_in.getStrData();
		            nfile_in.closeData();
		            nfile_out.putAttr("xsi:type", definition);
		        }
		        catch(std::exception& ex)
		        {
			    ; // definition not found
                        }
		    }
		}
                if (WriteGroup (is_definition) != NX_OK) return NX_ERROR;
	        remove_path(name);
	    }
	    else
	    {
	        remove_path(name);
		links_to_make.push_back(link_to_make(current_path, name, link.targetPath));
         	if (NXclosegroup (inId) != NX_OK) return NX_ERROR;
	    }
         }
      }
      else if (status == NX_EOD) {
         if (NXclosegroup (inId) != NX_OK) return NX_ERROR;
         if (NXclosegroup (outId) != NX_OK) return NX_ERROR;
         return NX_OK;
      }
   } while (status == NX_OK);
   return NX_OK;
}

static int WriteAttributes (int is_definition, int is_group)
{
   int status, i, attrLen, attrType;
   NXname attrName;
   void *attrBuffer;
   int found_napitype = 0;

   i = 0;
   do {
      status = NXgetnextattr (inId, attrName, &attrLen, &attrType);
      if (status == NX_ERROR) return NX_ERROR;
      if (status == NX_OK) {
         if (strcmp(attrName, "NAPItype") == 0)
	 {
		found_napitype = 1;
	 }
         if (strcmp(attrName, "NeXus_version") && strcmp(attrName, "XML_version") &&
             strcmp(attrName, "HDF_version") && strcmp(attrName, "HDF5_Version") && 
             strcmp(attrName, "file_name") && strcmp(attrName, "file_time")) {
            attrLen++; /* Add space for string termination */
            if (NXmalloc((void**)&attrBuffer, 1, &attrLen, attrType) != NX_OK) return NX_ERROR;
            if (NXgetattr (inId, attrName, attrBuffer, &attrLen , &attrType) != NX_OK) return NX_ERROR;
	    if (is_definition && attrType == NX_CHAR)
	    {
		clean_string(attrBuffer, 1, &attrLen);
	    }
	    if(attrType == NX_INT8 || attrType == NX_UINT8)
	    {
	      /*
		This is most likely a very old file where we used NX_INT8 or NX_UINT8 
		for strings because there was no explicit NX_CHAR type yet.
	      */
	      attrType = NX_CHAR;
            }
            if (NXputattr (outId, attrName, attrBuffer, attrLen , attrType) != NX_OK) return NX_ERROR;
            if (NXfree((void**)&attrBuffer) != NX_OK) return NX_ERROR;
         }
         i++;
      }
   } while (status != NX_EOD);
   // if we are creating a reduced file for definiiton purposes,
   // make sure we always have a NAPItype attribute
   if (is_definition && !is_group && !found_napitype)
   {
       // need to be cleverer - cannot do this
       //if (NXputattr (outId, "NAPItype", (void*)"NX_CHAR", strlen("NX_CHAR"), NX_CHAR) != NX_OK) return NX_ERROR;
   }
   return NX_OK;
}

