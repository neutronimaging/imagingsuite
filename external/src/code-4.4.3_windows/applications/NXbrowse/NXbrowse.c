/*-----------------------------------------------------------------------------
 NeXus - Neutron & X-ray Common Data Format
  
 NeXus Browser

 Copyright (C) 2000-2014, NIAC

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2 of the License, or (at your option) any
 later version.

 This library is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along
 with this library; if not, write to the Free Software Foundation, Inc., 59
 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Contact : R. Osborn <ROsborn@anl.gov>
           Materials Science Division
           Argonne National Laboratory
           Argonne, IL 60439-4845
           USA

 For further information, see <http://www.nexusformat.org>

!----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "napi.h"
#include "napi_internal.h" /* for NXM{En,Dis}ableErrorReporting */
#include "napiconfig.h"
static char *my_readline(const char *prompt)
{
	char inputText[256];
	char *stringPtr;
	fprintf(stdout, "%s", prompt);
	if (fgets(inputText, sizeof(inputText), stdin) == NULL) {
		return NULL;
	}
	if ((stringPtr = strchr(inputText, '\n')) != NULL) {
		*stringPtr = '\0';
	}
	return strdup(inputText);
}

#if HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
#define rl_completion_matches(a,b) NULL
#define rl_outstream stdout
#define readline my_readline
#endif				/* HAVE_LIBREADLINE */

#define StrEq(s1, s2) (strcmp((s1), (s2)) == 0)

#ifdef _WIN32
#define STRNCASECMP strnicmp
#else
#define STRNCASECMP strncasecmp
#endif				/* _WIN32 */

int NXBdir(NXhandle fileId);
int NXBread(NXhandle fileId, NXname dataName, char *dimensions);
int NXBdump(NXhandle fileId, NXname dataName, char *fileName);
void ConvertUpperCase(char *string);
void PrintAttributes(NXhandle fileId);
void PrintDimensions(int rank, int dimensions[]);
void PrintType(int dataType);
void PrintData(void *data, int dataType, int numElements);
void DumpData(FILE * fd, int rank, int dimensions[], int dataType, void *data);
void WriteData(FILE * fd, char *data, int dataType, int numElements);
int FindData(NXhandle fileId, char *dataName);
void parsepath(const char* pathspec, char* absolutedir, char* lastcomponent);
static int nxTypeSize(int dataType);

/* if iByteAsChar, NX_INT8 and NX_UINT8 are treated as characters */
static int iByteAsChar = 1;	/* Assume global attributes are all characters */
static NXhandle the_fileId;
static char nxFile[256];
static char path[256];

/* 
 * Freddie Akeroyd 18/10/2009 
 *
 * Add in support for readline and completion (nxbrowse_complete) of commands
 * (command_generator) and on data/group names (field_generator)
 */

typedef struct {
	const char *name;
	const char *doc;
} COMMAND;

COMMAND commands[] = {
	{"cd", "Move into to a group"},
	{"close", "Move out of a group"},
	{"dir", ""},
	{"pwd", ""},
	{"ls", ""},
	{"read", ""},
	{"open", ""},
	{"help", ""},
	{"info", ""},
	{"exit", ""},
	{"quit", ""},
	{"dump", ""},
	{"bytesaschar", ""},
	{NULL, NULL}
};

#if HAVE_LIBREADLINE
static char *command_generator(const char *text, int state)
{
	static int len, list_index;
	const char *name;
	if (!state) {
		list_index = 0;
		len = strlen(text);
	}
	while ((name = commands[list_index].name) != NULL) {
		++list_index;
		if (STRNCASECMP(name, text, len) == 0) {
			return strdup(name);
		}
	}
	return NULL;
}

struct name_item {
	char *name;
	struct name_item *next;
};

static char *field_generator(const char *text, int state)
{
	struct name_item *item, *t_item;
	static struct name_item *names = NULL, *last_item = NULL;
	char *res;
	int status, dataType;
	NXname name, nxclass;
	if (!state) {
		item = names;
		while (item != NULL) {
			if (item->name != NULL) {
				free(item->name);
				item->name = NULL;
			}
			t_item = item;
			item = item->next;
			t_item->next = NULL;
			free(t_item);
		}
		last_item = names = NULL;

		char matchtext[256], absdir[256], tmppath[256], prefix[256];

		prefix[0] = '\0';
		char *ptr = rindex(text+1, '/');
		if (ptr != NULL) {
			strncpy(prefix, text, ptr-text+1);
			prefix[ptr-text+1] = '\0';
		}

		parsepath(text, absdir, matchtext);

		/* check if we've got a dir already */
		NXMDisableErrorReporting();
		strcpy(tmppath, absdir);
		strcat(tmppath, "/");
		strcat(tmppath, matchtext);
		status = NXopengrouppath(the_fileId, tmppath);
		if (status == NX_OK) {
			strcpy(absdir, tmppath);
			strcpy(matchtext, "");
			if (strlen(text) > 0 && text[strlen(text)-1] != '/') {
				strcpy(prefix, text);
				strcat(prefix, "/");
			}
		} else {
			/* if not go into the dir part */
			status = NXopengrouppath(the_fileId, absdir);
		}
		NXMEnableErrorReporting();

		if (status == NX_ERROR)
			return NULL;

		if (NXinitgroupdir(the_fileId) != NX_OK) 
			return NULL;

		do {
			status =
			    NXgetnextentry(the_fileId, name, nxclass, &dataType);
			if (status == NX_ERROR)
				break;
			if (status == NX_OK) {
				if (strncmp(nxclass, "CDF", 3) == 0) {
					;
				} else
				    if (strncmp(name, matchtext, strlen(matchtext)) == 0) {
					item = (struct name_item *)
					    malloc(sizeof(struct name_item));
					item->name = (char *)calloc(256, 1);
					strcpy(item->name, prefix);
					strcat(item->name, name);
					
					if (strcmp(nxclass, "SDS") != 0) {
						strcat(item->name, "/");
					}
					item->next = NULL;
					if (last_item == NULL) {
						names = item;
					} else {
						last_item->next = item;
					}
					last_item = item;
				}
			}
		} while (status == NX_OK);
		NXopengrouppath(the_fileId, path);
		last_item = names;
	}
	if (last_item != NULL) {
		res = strdup(last_item->name);
		last_item = last_item->next;
	} else {
		res = NULL;
	}
	return res;
}

static char **nxbrowse_complete(const char *text, int start, int end)
{
	char **matches = NULL;
	static char line[512];
	strncpy(line, text + start, end - start);
	line[end - start] = '\0';

	if (start == 0) {
		matches = rl_completion_matches(text, command_generator);
		rl_completion_append_character = ' ';
	} else {
		rl_completion_entry_function = field_generator; 
		matches = rl_completion_matches(text, field_generator);
		rl_completion_append_character = '\0';
	}

	return matches;
}
#endif /* HAVE_LIBREADLINE */

int main(int argc, char *argv[])
{
	char fileName[256], oldwd[256], *command, *dimensions, *stringPtr;
	char prompt[512];
	char *inputText;
	NXname dataName;
	int status;

#if HAVE_LIBREADLINE
	rl_readline_name = "NXbrowse";
	rl_attempted_completion_function = nxbrowse_complete;
#if READLINE_VERSION >= 0x500
	rl_catch_signals = 0;
#else
#define rl_crlf() fprintf(rl_outstream, "\r\n");
#define rl_on_new_line() 1
#endif
	using_history();
#else
#define rl_crlf()
#define rl_on_new_line()
#define add_history(a)
#endif

	printf("NXBrowse %s Copyright (C) 2009-2014 NeXus Data Format\n",
	       NEXUS_VERSION);
#if HAVE_LIBREADLINE
	printf
	    ("Built with readline support - use <TAB> to complete commands and paths\n");
#endif				/* HAVE_LIBREADLINE */

	/* if there is a filename given on the command line use that,
	      else ask for a filename */
	if (argc < 2) {
		printf("Give name of NeXus file : ");
		if (fgets(fileName, sizeof(fileName), stdin) == NULL) {
			printf("Failed to open %s\n", fileName);
			return NX_ERROR;
		}
		if ((stringPtr = strchr(fileName, '\n')) != NULL)
			*stringPtr = '\0';
	} else {
		strcpy(fileName, argv[1]);
	}
	strcpy(nxFile, fileName);

	/* Open input file and output global attributes */
	if (NXopen(fileName, NXACC_READ, &the_fileId) != NX_OK) {
		printf("NX_ERROR: Can't open %s\n", fileName);
		return NX_ERROR;
	}
	PrintAttributes(the_fileId);
	iByteAsChar = 0;	/* Display remaining NX_INT8 and NX_UINT8 variables as integers by default */
	/* Input commands until the EXIT command is given */
	strcpy(oldwd, "/");
	strcpy(path, "/");
	do {
		sprintf(prompt, "NX%s> ", path);
		if (getenv("NO_READLINE") != NULL) {
			inputText = my_readline(prompt);
		} else {
			inputText = readline(prompt);
		}
		if (inputText == NULL) {
			inputText = strdup("EXIT");
		}
		if (*inputText) {
			add_history(inputText);
		}
		command = strtok(inputText, " ");
		/* Check if a command has been given */
		if (command == NULL)
			command = " ";
		/* Convert it to upper case characters */
		ConvertUpperCase(command);

		if (StrEq(command, "PWD")) {
			fprintf(rl_outstream, "%s\n", path);
		}

		if (StrEq(command, "TEST")) {
			char a[256], b[256];
			stringPtr = strtok(NULL, " ");
			if (stringPtr != NULL) {
				parsepath(stringPtr, a, b);
				fprintf(rl_outstream," you entered >%s< - i think the full path is >%s< and the final component looks like this >%s<.\n", stringPtr, a, b);
			} else {
				fprintf(rl_outstream," you entered nothing\n");
			}
		}

		/* Command is to print a directory of the current group */
		if (StrEq(command, "DIR") || StrEq(command, "LS")) {
			stringPtr = strtok(NULL, " ");
			if (stringPtr != NULL) {
				char a[256], b[256];
				parsepath(stringPtr, a, b);
				strcat(a, "/"); 
				strcat(a, b); 
				NXopengrouppath(the_fileId, a);
				NXBdir(the_fileId);
				NXopengrouppath(the_fileId, path);
			} else {
				NXBdir(the_fileId);
			}
		}

		/* Command is to open the specified group */
		if (StrEq(command, "OPEN") || StrEq(command, "CD")) {
			stringPtr = strtok(NULL, " ");
			if (stringPtr != NULL) {
				char a[256], b[256];

				if (StrEq(stringPtr, "-")) {
					stringPtr = oldwd;
				} 

				parsepath(stringPtr, a, b);
				strcat(a, "/"); 
				strcat(a, b); 

				status = NXopengrouppath(the_fileId, a);

				if (status == NX_OK) {
					strcpy(oldwd, path);
					strcpy(path, a);
				} else {
					fprintf(rl_outstream, "NX_ERROR: cannot change into %s\n", stringPtr);
					NXopengrouppath(the_fileId, path); /* to be sure */
				}

			} else {
				fprintf(rl_outstream, "NX_ERROR: Specify a group\n");
			}
		}

		/* Command is to dump data values to a file */
		if (StrEq(command, "DUMP")) {
			stringPtr = strtok(NULL, " ");
			if (stringPtr != NULL) {
				strcpy(dataName, stringPtr);
				stringPtr = strtok(NULL, " ");
				if (stringPtr != NULL) {
					strcpy(fileName, stringPtr);
					status = NXBdump(the_fileId, dataName, fileName);
				} else {
					fprintf(rl_outstream, "NX_ERROR: Specify a dump file name \n");
				}
			} else {
				fprintf(rl_outstream, "NX_ERROR: Specify a data item\n");
			}
		}
		/* Command is to print the values of the data */
		if (StrEq(command, "READ") || StrEq(command, "CAT")) {
			stringPtr = strtok(NULL, " [");
			if (stringPtr != NULL) {
				strcpy(dataName, stringPtr);
				dimensions = strtok(NULL, "[]");
				status =
				    NXBread(the_fileId, dataName, dimensions);
			} else {
				fprintf(rl_outstream,
					"NX_ERROR: Specify a data item\n");
			}
		}
		/* Command is to close the current group */
		if (StrEq(command, "CLOSE")) {
			if (strlen(path) > 1) {
				if (NXclosegroup(the_fileId) == NX_OK) {
					/* Remove the group from the prompt string */
					strcpy(oldwd, path);
					stringPtr = strrchr(path, '/');	/* position of last group delimiter */
					if (stringPtr != NULL)
						*stringPtr = '\0';	/* terminate the string there */
				}
			} else {
				fprintf(rl_outstream,
					"NX_WARNING: Already at root level of file\n");
			}
		}
		/* Command is to print help information */
		if (StrEq(command, "HELP") || StrEq(command, "INFO")) {
			printf("NXbrowse commands : DIR\n");
			printf("                    LS\n");
			printf("                    OPEN <groupName>\n");
			printf("                    CD <groupName>\n");
			printf("                    READ <dataName>\n");
			printf("                    READ <dataName>[<dimension indices...>]\n");
			printf("                    DUMP <dataName> <fileName> \n");
			printf("                    CLOSE\n");
			printf("                    BYTEASCHAR\n");
			printf("                    HELP\n");
			printf("                    EXIT\n");
			printf("\n");
#if HAVE_LIBREADLINE
			printf("Pressing <TAB> after a command or partial nexus object name will complete\n");
			printf("possible names. For example:\n");
			printf("\n");
			printf("    cd ent<TAB KEY PRESSED>     # all items starting with ent are listed\n");
			printf("\n");
#endif
		}
		/* Command is to print byte as char information */
		if (StrEq(command, "BYTEASCHAR")) {
			if (iByteAsChar == 1)
				iByteAsChar = 0;
			else
				iByteAsChar = 1;
		}
		/* Command is to exit the program */
		if (StrEq(command, "EXIT") || StrEq(command, "QUIT")) {
			/* for (i = groupLevel; i > 0; i--) NXclosegroup (the_fileId); */
			NXclose(&the_fileId);
			return NX_OK;
		}
		status = NX_OK;
		free(inputText);
	} while (status == NX_OK);
	return NX_OK;
}

/* Checks for attributes and outputs their values */
void PrintGroupAttributes(NXhandle fileId, char *groupname)
{
	int status, attrLen, attrType;
	NXname attrName;
	void *attrBuffer;

	do {
		status = NXgetnextattr(fileId, attrName, &attrLen, &attrType);
		if (status == NX_ERROR)
			return;
		if (status == NX_OK) {
			attrLen++;	/* Add space for string termination */
			if (NXmalloc((void **)&attrBuffer, 1, &attrLen, attrType) != NX_OK)
				return;
			if (NXgetattr(fileId, attrName, attrBuffer, &attrLen, &attrType) != NX_OK)
				return;
			printf("             %s attribute: %s = ", groupname, attrName);
			PrintData(attrBuffer, attrType, attrLen);
			printf("\n");
			if (NXfree((void **)&attrBuffer) != NX_OK)
				return;
		}
	} while (status != NX_EOD);
	return;
}

/* Outputs the contents of a NeXus group */
int NXBdir(NXhandle fileId)
{
	int status, dataType, dataRank, dataDimensions[NX_MAXRANK], length;
	NXname name, nxclass, nxurl;

	if (NXinitgroupdir(fileId) != NX_OK)
		return NX_ERROR;
	do {
		status = NXgetnextentry(fileId, name, nxclass, &dataType);
		if (status == NX_ERROR)
			break;
		if (status == NX_OK) {
			if (strncmp(nxclass, "CDF", 3) == 0) {
				;
			} else if (strcmp(nxclass, "SDS") == 0) {
				printf("  NX Data  : %s", name);
				if (NXopendata(fileId, name) != NX_OK)
					return NX_ERROR;
				if (NXgetinfo (fileId, &dataRank, dataDimensions, &dataType) != NX_OK)
					return NX_ERROR;
				if (NXclosedata(fileId) != NX_OK)
					return NX_ERROR;
				PrintDimensions(dataRank, dataDimensions);
				printf(" ");
				PrintType(dataType);
				printf("\n");
			} else {
				length = sizeof(nxurl);
				if (NXisexternalgroup(fileId, name, nxclass, nxurl, length) == NX_OK) {
					printf("  NX external Group: %s (%s), linked to: %s \n",
					     name, nxclass, nxurl);
				} else {
					printf("  NX Group : %s (%s)\n", name, nxclass);
					if ((status = NXopengroup(fileId, name, nxclass)) != NX_OK) {
						return status;
					}
					PrintGroupAttributes(fileId, name);
					if ((status = NXclosegroup(fileId)) != NX_OK) {
						return status;
					}
				}
			}
		}
	} while (status == NX_OK);
	return status;
}

/* Outputs requested data */
int NXBread(NXhandle fileId, NXname dataName, char *dimensions)
{
	int dataRank, dataDimensions[NX_MAXRANK], dataType, start[NX_MAXRANK],
	    size[NX_MAXRANK], i, j, total_size;
	char dimString[80], *subString;
	void *dataBuffer;

	/* Check the specified data item exists */
	if (FindData(fileId, dataName) != NX_OK)
		return NX_ERROR;
	/* Open the data and obtain its type and rank details */
	if (NXopendata(fileId, dataName) != NX_OK)
		return NX_ERROR;
	if (NXgetinfo(fileId, &dataRank, dataDimensions, &dataType) != NX_OK)
		return NX_ERROR;
	/* Check if a single element has been specified */
	/* If so, read in the indices */
	if (dimensions != NULL) {
		strcpy(dimString, dimensions);
		subString = strtok(dimString, ",");
		for (i = 0; subString != NULL && i < NX_MAXRANK; i++) {
			if (i >= dataRank) {
				printf("NX_ERROR: Data rank = %d\n", dataRank);
				return NX_ERROR;
			}
			sscanf(subString, "%d", &j);
			if (j > dataDimensions[i] || j < 1) {
				printf("NX_ERROR: Data dimension %d = %d\n",
				       (i + 1), dataDimensions[i]);
				return NX_ERROR;
			}
			start[i] = j - 1;
			size[i] = 1;
			subString = strtok(NULL, ",");
		}
		if (i != dataRank) {
			printf("NX_ERROR: Data rank = %d\n", dataRank);
			return NX_ERROR;
		}
	} else {
		/* Otherwise, allocate enough space for the first 3 elements of each dimension */
		for (i = 0; i < dataRank; i++) {
			if (dataDimensions[i] > 3 && dataType != NX_CHAR) {
				start[i] = 0;
				size[i] = 3;
			} /* unless it's a character string */
			else {
				start[i] = 0;
				size[i] = dataDimensions[i];
			}
		}
	}
	total_size = 1;
	for (i = 0; i < dataRank; i++) {
		total_size *= dataDimensions[i];
	}
	if (NXmalloc((void **)&dataBuffer, dataRank, size, dataType) != NX_OK)
		return NX_ERROR;
	/* Read in the data with NXgetslab */
	if (dataType == NX_CHAR) {
		if (NXgetdata(fileId, dataBuffer) != NX_OK)
			return NX_ERROR;
	} else {
		if (NXgetslab(fileId, dataBuffer, start, size) != NX_OK)
			return NX_ERROR;
	}
	/* Output data name, dimensions and type */
	printf("  %s", dataName);
	if (dimensions == NULL)
		PrintDimensions(dataRank, dataDimensions);
	else
		printf("[%s]", dimensions);
	printf(" ");
	PrintType(dataType);
	printf(" = ");
	/* Output the data according to data type */
	if (dimensions == NULL) {	/* Print the first few values (max 3) */
		if (dataType == NX_CHAR) {	/* If the data is a string, output the whole buffer */
			/* this prints the first line of an array; could print more */
		  if(dataDimensions[dataRank-1] == 0) {
		    PrintData("No data in string\n",NX_CHAR,
			      sizeof("No data in string\n"));
		  } else {
			for (i = 0;
			     i < total_size / dataDimensions[dataRank - 1];
			     i++) {
				PrintData((char *)dataBuffer +
					  i * dataDimensions[dataRank - 1],
					  dataType,
					  dataDimensions[dataRank - 1]);
				PrintData("\n", NX_CHAR, 1);
			}
		  }
		} else {
			if (dataRank == 1 && dataDimensions[0] == 1) {	/* It's a scalar */
				PrintData(dataBuffer, dataType, 1);
			} else {	/* It's an array */
				printf("[ ");
				/* Determine total size of input buffer */
				for (i = 0, j = 0; i < dataRank; i++)
					j += dataDimensions[i];
				/* Output at least 3 values */
				if (j > 3) {
					PrintData(dataBuffer, dataType, 3);
					printf("...");
				}
				/* unless the total size is smaller */
				else {
					PrintData(dataBuffer, dataType, j);
				}
				printf("]");
			}
		}
	} else {		/* Print the requested item */
		PrintData(dataBuffer, dataType, 1);
	}
	printf("\n");
	if (NXfree((void **)&dataBuffer) != NX_OK)
		return NX_ERROR;

	/* Check for attributes unless a single element is specified */
	if (dimensions == NULL)
		PrintAttributes(fileId);

	/* Close data set */
	if (NXclosedata(fileId) != NX_OK)
		return NX_ERROR;

	return NX_OK;
}

/* Dumps requested data */
int NXBdump(NXhandle fileId, NXname dataName, char *fileName)
{
	int dataRank, dataDimensions[NX_MAXRANK], dataType, i;
	FILE *fd = NULL;
	void *dataBuffer;

	/* Check the specified data item exists */
	if (FindData(fileId, dataName) != NX_OK)
		return NX_ERROR;

	/* Open the data and obtain its type and rank details */
	if (NXopendata(fileId, dataName) != NX_OK)
		return NX_ERROR;
	if (NXgetinfo(fileId, &dataRank, dataDimensions, &dataType) != NX_OK)
		return NX_ERROR;

	/* Open the file */
	fd = fopen(fileName, "w");
	if (!fd) {
		printf("ERROR: failed to open--> %s <-- for writing\n",
		       fileName);
		return NX_ERROR;
	}

	/* Allocate data space */
	if (NXmalloc(&dataBuffer, dataRank, dataDimensions, dataType) != NX_OK)
		return NX_ERROR;

	/* Read the lot */
	if (NXgetdata(fileId, dataBuffer) != NX_OK)
		return NX_ERROR;

	if (NXclosedata(fileId) != NX_OK)
		return NX_ERROR;

	/* Print a header */
	fprintf(fd, "File : %s, DataSet: %s \n", nxFile, dataName);
	for (i = 0; i < dataRank; i++) {
		fprintf(fd, " %d ", dataDimensions[i]);
	}
	fprintf(fd, "\n");

	/* Dump the data */
	DumpData(fd, dataRank, dataDimensions, dataType, dataBuffer);

	/* Clean up */
	fclose(fd);
	NXfree(&dataBuffer);
	return NX_OK;
}

/* Converts command string to upper case */
void ConvertUpperCase(char *string)
{
	int i;

	for (i = 0; string[i] != 0; i++) {
		if (string[i] >= 97 && string[i] <= 122) {
			string[i] = string[i] - 32;
		}
	}
}

/* Checks for attributes and outputs their values */
void PrintAttributes(NXhandle fileId)
{
	int status, attrLen = 0, attrType;
	NXname attrName;
	void *attrBuffer;

	do {
		status = NXgetnextattr(fileId, attrName, &attrLen, &attrType);
		if (status == NX_ERROR)
			return;
		if (status == NX_OK) {
			attrLen++;	/* Add space for string termination */
			if (NXmalloc
			    ((void **)&attrBuffer, 1, &attrLen,
			     attrType) != NX_OK)
				return;
			if (NXgetattr
			    (fileId, attrName, attrBuffer, &attrLen,
			     &attrType) != NX_OK)
				return;
			printf("    %s = ", attrName);
			PrintData(attrBuffer, attrType, attrLen);
			printf("\n");
			if (NXfree((void **)&attrBuffer) != NX_OK)
				return;
		}
	} while (status != NX_EOD);
	return;
}

/* Outputs the specified dimensions as a formatted string */
void PrintDimensions(int rank, int dimensions[])
{
	int i;

	/* if (rank > 1 || dimensions[0] != 1) { */
	printf("[");
	for (i = 0; i < rank; i++) {
		if (i > 0)
			printf(",");
		printf("%d", dimensions[i]);
	}
	printf("]");
	/* } */
}

/* Converts the NeXus data type into a character string */
void PrintType(int dataType)
{
	switch (dataType) {
	case NX_CHAR:
		printf("(NX_CHAR)");
		break;
	case NX_FLOAT32:
		printf("(NX_FLOAT32)");
		break;
	case NX_FLOAT64:
		printf("(NX_FLOAT64)");
		break;
	case NX_INT8:
		printf("(NX_INT8)");
		break;
	case NX_UINT8:
		printf("(NX_UINT8)");
		break;
	case NX_INT16:
		printf("(NX_INT16)");
		break;
	case NX_UINT16:
		printf("(NX_UINT16)");
		break;
	case NX_INT32:
		printf("(NX_INT32)");
		break;
	case NX_UINT32:
		printf("(NX_UINT32)");
		break;
	case NX_INT64:
		printf("(NX_INT64)");
		break;
	case NX_UINT64:
		printf("(NX_UINT64)");
		break;
	default:
		printf("(UNKNOWN)");
		break;
	}
}

/* Dumps data to a file. Uses recursion because of unknown number of dimensions. */
void DumpData(FILE * fd, int rank, int dimensions[], int dataType, void *data)
{
	char *dataPtr;
	int i, dimSize, dataSize, lineSize;

	if (rank > 1) {		/* Recursively call DumpData until rank = 1 */
		for (i = 1, dimSize = 1; i < rank; i++) {
			dimSize *= dimensions[i];
		}
		for (i = 0; i < dimensions[0]; i++) {
			dataPtr =
			    (char *)data + i * dimSize * nxTypeSize(dataType);
			DumpData(fd, rank - 1, &dimensions[1], dataType,
				 dataPtr);
		}
		return;
	} else {		/* Actually print the data */
		dataSize = dimensions[0];
		dataPtr = (char *)data;
		while (dataSize > 0) {
			if (dataSize > 10) {
				lineSize = 10;
			} else {
				lineSize = dataSize;
			}
			WriteData(fd, dataPtr, dataType, lineSize);
			fprintf(fd, "\n");
			dataSize -= lineSize;
			dataPtr += lineSize * nxTypeSize(dataType);
		}
	}
}

/* Writes data items with the requested type */
void WriteData(FILE * fd, char *data, int dataType, int numElements)
{
	int i;

	for (i = 0; i < numElements; i++) {
		switch (dataType) {
		case NX_CHAR:
			if (data[i] == '\0') {
				return;
			}
			fprintf(fd, "%c", ((char *)data)[i]);
			break;
		case NX_INT8:
			if (iByteAsChar) {
				fprintf(fd, "%c", ((char *)data)[i]);
			} else {
				fprintf(fd, "%d ", ((char *)data)[i]);
			}
			break;
		case NX_UINT8:
			if (iByteAsChar) {
				fprintf(fd, "%c", ((unsigned char *)data)[i]);
			} else {
				fprintf(fd, "%d ", ((unsigned char *)data)[i]);
			}
			break;
		case NX_INT16:
			fprintf(fd, "%d ", ((short *)data)[i]);
			break;
		case NX_UINT16:
			fprintf(fd, "%d ", ((unsigned short *)data)[i]);
			break;
		case NX_INT32:
			fprintf(fd, "%d ", ((int *)data)[i]);
			break;
		case NX_UINT32:
			fprintf(fd, "%d ", ((unsigned *)data)[i]);
			break;
		case NX_INT64:
			fprintf(fd, "%lld ", (long long)((int64_t *) data)[i]);
			break;
		case NX_UINT64:
			fprintf(fd, "%llu ",
				(unsigned long long)((uint64_t *) data)[i]);
			break;
		case NX_FLOAT32:
			fprintf(fd, "%f ", ((float *)data)[i]);
			break;
		case NX_FLOAT64:
			fprintf(fd, "%f ", ((double *)data)[i]);
			break;
		default:
			printf("WriteData: invalid type");
			break;
		}
	}
}

/* Outputs data items with the requested type */
void PrintData(void *data, int dataType, int numElements)
{
	WriteData(stdout, (char *)data, dataType, numElements);
}

/* Searches group for the requested data item */
int FindData(NXhandle fileId, char *dataName)
{
	int status, dataType;
	NXname name, nxclass;

	NXinitgroupdir(fileId);
	do {
		status = NXgetnextentry(fileId, name, nxclass, &dataType);
		if (status == NX_ERROR)
			return NX_ERROR;
		if (status == NX_OK) {
			if (StrEq(dataName, name)) {
				if (!strncmp(nxclass, "SDS", 3)) {	/* Data has class "SDS" */
					return NX_OK;
				} else {
					printf("NX_ERROR: %s is not data\n", dataName);
					return NX_ERROR;
				}
			}
		}
	} while (status != NX_EOD);
	printf("NX_ERROR: %s does not exist\n", dataName);
	return NX_EOD;
}

static int nxTypeSize(int dataType)
{
#ifdef XXXHDF4
	return DFKNTsize(dataType);
#else
	int type_size = 0;
	if ((dataType == NX_CHAR) || (dataType == NX_INT8)
	    || (dataType == NX_UINT8)) {
		type_size = 1;
	} else if ((dataType == NX_INT16) || (dataType == NX_UINT16)) {
		type_size = 2;
	} else if ((dataType == NX_INT32) || (dataType == NX_UINT32)
		   || (dataType == NX_FLOAT32)) {
		type_size = 4;
	} else if (dataType == NX_FLOAT64 || dataType == NX_INT64
		   || dataType == NX_UINT64) {
		type_size = 8;
	} else {
		printf("error in type %d\n", dataType);
	}
	return type_size;
#endif
}

/* 
  convert pathspec IN (which can contain .. . be absolute or relative) 
  to an absolute path OUT and a final component OUT
  relative paths are relative to the current path static
*/
void parsepath(const char* pathspec, char* absolutedir, char* lastcomponent) 
{
	char tmppath[512];
	char* component[80];
	int i = 0, n = 0;

	tmppath[0] = '\0';
	if (pathspec[0] != '/') {
		strcat(tmppath, path);
		strcat(tmppath, "/");
	}
	strcat(tmppath, pathspec);
	/* path is now absolute */
	component[0] = strtok(tmppath, "/");

	/* chop up and remove .. references */
	while((component[++n] = strtok(NULL, "/")));
	for(i = 0; i < n; i++) {
		if (component[i] == NULL)
			continue;
		if (strcmp(component[i], ".") == 0) {
			component[i] = NULL;
			continue;
		}
		if (strcmp(component[i], "..") == 0) {
			int j = i - 1;
			component[i][0] = '\0';
			while(j>=0) {
				if (component[j] != NULL) {
					component[j] = NULL;
					break;
				}
				j--;
			}
			continue;
		}
	}
	absolutedir[0] = '\0';
	lastcomponent[0] = '\0';
	if (component[n-1] != NULL)
		strcat(lastcomponent, component[n-1]);
	for(i = 0; i < n-1; i++) {
		if (component[i] != NULL && component[i][0] != '\0') {
			strcat(absolutedir, "/");
			strcat(absolutedir, component[i]);
		}
	}
	if (strlen(absolutedir) == 0)
		strcat(absolutedir, "/");
		
	return;
}
