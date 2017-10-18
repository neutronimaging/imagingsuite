/*
  This implements a handle management module. Sometimes it is useful to
  protect the user of some software module from messing with complicated
  datastructures. In such cases it is useful  to use an integer handle
  which can be translated into a pointer when needed by the code implementing
  the module. Such a scheme is implemented in this module.

  Mark Koennecke, October 2000
*/
#ifndef HANDLEHANDLE
#define HANDLEHANDLE

/* The maximum number of handles. */
#define MAXHANDLE 8192

  int HHMakeHandle(void *pData);
  void  *HHGetPointer(int handle);
  void HHChangeHandle(void *pData, int currentHandle);
  int HHRemoveHandle(int handle);  
  int HHMakeLinkHandle(void *pData, int currentHandle); 
  int HHCheckIfHandleExists(int check);
#endif
 
