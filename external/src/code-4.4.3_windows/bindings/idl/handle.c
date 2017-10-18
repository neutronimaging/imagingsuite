/*
  This implements a handle management module. Sometimes it is useful to
  protect the user of some software module from messing with complicated
  datastructures. In such cases it is useful  to use an integer handle
  which can be translated into a pointer when needed by the code implementing
  the module. Such a scheme is implemented in this module.

  Mark Koennecke, October 2000
*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "handle.h"

#define MAX_NUMBEROFLINKS 200

static void **pointerArray = NULL;

/* This is a 2 dimensional dynamic array assosiates the link handles with their
 * file id handles so they can be removed with in nxclose.
 */
static int (*listOfNXlinks)[MAXHANDLE] = NULL;

static int iscreated = 0;

/*----------------------------------------------------------------------*/
static void checkArray()
{
  if(pointerArray == NULL)
  {
      pointerArray = (void **)malloc(MAXHANDLE*sizeof(void *));
      assert(pointerArray != NULL);
      memset(pointerArray,0,MAXHANDLE*sizeof(void *));
  }
}
/*--------------------------------------------------------------------*/
int HHMakeHandle(void *pData)
{
  int i;

  checkArray();
  /*
    find first free slot in the pointerArray, store the pointer and
    return the index.
  */
  for(i = 0; i < MAXHANDLE; i++)
  {
     if(pointerArray[i] == NULL)
     {
	pointerArray[i] = pData;
	iscreated = 1;
	listOfNXlinks = calloc (MAX_NUMBEROFLINKS, sizeof (*listOfNXlinks));

       return i;
     }
  }
  return -1;
}
/*--------------------------------------------------------------------*/
void HHChangeHandle(void *pData, int currentHandle)
{
  int i;

	pointerArray[currentHandle] = pData;
	iscreated = 1;

}
/*--------------------------------------------------------------------*/
int HHMakeLinkHandle(void *pData, int currentHandle)
{
  int i;
  int k;

  checkArray();
  /*
    find first free slot in the pointerArray, store the pointer and
    return the index.
  */
  for(i = 0; i < MAXHANDLE; i++)
  {
     if(pointerArray[i] == NULL)
     {
       pointerArray[i] = pData;


	for(k = 0; k < MAX_NUMBEROFLINKS; k++)
	{
	if(listOfNXlinks[currentHandle][k] == NULL)
		{
		listOfNXlinks[currentHandle][k] = i;
		return i;
		}

     	}
    }
  }

   return -1;
}


/*---------------------------------------------------------------------*/
void *HHGetPointer(int handle)
{
  checkArray();
  return pointerArray[handle];
}
/*---------------------------------------------------------------------*/
int HHRemoveHandle(int handle)
{
  int k;

  assert(handle < MAXHANDLE && handle >= 0);
  checkArray();
  pointerArray[handle] = NULL;

  for(k = 0; k < MAX_NUMBEROFLINKS; k++)
	{
	if(listOfNXlinks[handle][k] == NULL)
		{
		return(0);
		}
	free(pointerArray[(listOfNXlinks[handle][k])]);
	pointerArray[(listOfNXlinks[handle][k])] = NULL;
	}
  return(-1);
}

/*---------------------------------------------------------------------*/
int HHCheckIfHandleExists(int check)
{
  if(iscreated == 1) {
	  if(pointerArray[check] == NULL)
  	  	check = -1;
  	  else check = 0;
	  }
  else check = -1;
  return check;

}
