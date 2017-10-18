/**
  * This is the command line application for nxvalidate
	* implemented in C
	*
	* Copyright: GPL
	*
	* Mark Koennecke, mark.koennecke@psi.ch, and the NIAC
	*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <nxvalidate.h>

int main(int argc, char *argv[])
{
	pNXVcontext nxvContext = NULL;
  char *defDir = strdup("/Users/konnecke/src/nexus_definitions");
	char *appDef = NULL;
	int warnOpt = 0, warnBase = 0, warnUndefined = 0;
  char c;
  int status = 0;

	while ((c = getopt (argc, argv, "a:l:obu")) != -1) {
    switch (c)
    {
      case 'o':
        warnOpt = 1;
        break;
      case 'b':
        warnBase = 1;
        break;
      case 'u':
        warnUndefined = 1;
        break;
			case 'a':
				appDef = strdup(optarg);
				break;
			case 'l':
				free(defDir);
				defDir = strdup(optarg);
				break;
      case '?':
        if (optopt == 'a' ||  optopt == 'l')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
    }
	}

	if(argc <= optind){
		fprintf(stderr,"ERROR: no data file to validate specified\n");
		fprintf(stderr,"Usage:\n\tnxvvalidate -a appdef -l appdefdir -o -b -u datafile\n");
		exit(1);
	}

	nxvContext = NXVinit(defDir);
	if(nxvContext == NULL){
		fprintf(stdout,"ERROR: failed to allocate validation context\n");
		exit(1);
	}
	NXDVsetOutputFlags(nxvContext, warnOpt, warnBase, warnUndefined);
  status =  NXVvalidate(nxvContext, argv[optind], appDef, NULL);
	NXVkill(nxvContext);
	return(status);
}
