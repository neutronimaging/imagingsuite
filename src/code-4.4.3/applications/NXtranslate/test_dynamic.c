/*
 * $Id$
 * 
 * test/example program for NXtranslate DynamicRetriever class
 * implements functions required by dynamic_retriever.h
 *
 * build shared library with 
 *
 *                 gcc -shared -o test_dynamic.so test_dynamic.c
 *
 * and then use
 *                 "dynamic/test_dynamic.so"
 *
 * as the NXtranslate NXS:mime_type to activate it.
 *
 * Freddie Akeroyd, CCLRC ISIS Facility <F.A.Akeroyd@rl.ac.uk>
 *
 */
#include <string.h>
#include "napi.h"

// this is called when a DynamicRetriever instance is created
void* nxtinit(const char* source)
{
    return strdup(source); // this will be passed back as "ref" argument to 
                           // other functions. It can be anything e.g. a 
                           // file pointer or handle
}

// this is called when a DynamicRetriever instance is destroyed
int nxtcleanup(void* ref)
{
    free(ref);
    return 0;
}

// retrieves data from location specified in arg
void* nxtgetdata(void* ref, const char* arg, int* data_type, int* dims_array, 
              int* ndims, int* free_data)
{
    char* result;
    result = strdup(arg);
    *free_data = 1; // we want NXtranslate to call freedata() on "result" for us
    *ndims = 1;
    dims_array[0] = strlen(result);
    *data_type = NX_CHAR;
    return result;
}

// free up any memory returned by a previous "getdata" when we have
// specified the "free_data" option
int nxtfreedata(void* ref, void* arg)
{
    free(arg);
    return 0;
}

