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

static void **pointerArray = NULL;

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
       return i;
     }
  }
  return -1;
}
/*---------------------------------------------------------------------*/
void  *HHGetPointer(int handle)
{
  assert(handle < MAXHANDLE && handle >= 0);
  checkArray();
  return pointerArray[handle];
}
/*---------------------------------------------------------------------*/
void HHRemoveHandle(int handle)
{
  assert(handle < MAXHANDLE && handle >= 0);
  checkArray();
  pointerArray[handle] = NULL;
}

