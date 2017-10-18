/*
   This is the implementation file for the native methods used by the NeXus
   Java API.

   Mark Koennecke, 2000 -- 2011

   IMPLEMENTATION NOTES

   The NAPI uses a handle type for hiding the NeXus file datastructure.
   This handle is essentially a pointer. Now, dealing with pointers in
   Java is hideous. Usually a a pointer is just an integer but depending
   on the system this can be 4 byte, 8 byte or other. In order to get rid of 
   this problem we manage the pointers ourselves. The handle module maps
   integer handles to the NeXus handle for us. All the java code sees is
   the integer. But any routine in here has to retrieve the NXhandle for
   the integer first before it can do useful work.

*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "org_nexusformat_NexusFile.h"
#include <napi.h>
#include "handle.h"

#ifdef WIN32
/* commented  away for MINGW 
#include <mapiwin.h> 
*/
#endif

/* #define DEBUG */

static JavaVM *jvm;  // Global variable

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jclass nexusException;  

    jint ret = (*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_1);

    assert(ret == JNI_OK);

    jvm = vm;

    nexusException = (*env)->FindClass(env,"org/nexusformat/NexusException");
    if (nexusException == NULL) {
	fprintf(stderr, "cannot find NexusException - this will not work. Terminating.");
	assert(nexusException);
    }

    return JNI_VERSION_1_1;
}

/*---------------------------------------------------------------------------
                              ERROR TREATMENT

  The NAPI posts any errors to a customisable function. 
  We construct and throw a NexusException with the message received.
  --------------------------------------------------------------------------*/
static void JapiError(void *pData, char *text) {
    JNIEnv *env = pData;
    jclass nexusException;  

#ifdef DEBUG
    fprintf(stderr,"JapiError called with: %s\n", text); 
#endif

    /* ignore env passed in seems safer */
    (*jvm)->AttachCurrentThread (jvm, (void **) &env, NULL);

    if (env == NULL) {
	// if there is no thread environment we do not need to throw an exception
	return;
    }

    // Find and store the NexusException class for use in JapiError
    nexusException = (*env)->FindClass(env,"org/nexusformat/NexusException");
    (*env)->ThrowNew(env, nexusException, text);
} 

/*------------------------------------------------------------------------
            init or NXopen
-------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_org_nexusformat_NexusFile_init
  (JNIEnv *env, jobject obj, jstring filename, jint access)
{
    NXhandle handle;
    char *fileName;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* extract the filename as a C char* */
    fileName = (char *) (*env)->GetStringUTFChars(env, filename, 0);    
    
    /* call NXopen */
#ifdef DEBUG
    fprintf(stderr,"Calling NXopen on %s, with %d\n", fileName, access);
#endif
    iRet = NXopen(fileName,access,&handle);

#ifdef DEBUG
    fprintf(stderr,"Handle allocated for %s\n", fileName);
#endif

    /* release the filename string */
    (*env)->ReleaseStringUTFChars(env,filename, fileName);

    /* error return */
    if(iRet != NX_OK)
    {
      return -1;
    }

    /* convert the NXhandle to a integer handle */
    return HHMakeHandle(handle);
}
/*-----------------------------------------------------------------------
                     nxflush
------------------------------------------------------------------------*/ 
JNIEXPORT jint JNICALL Java_org_nexusformat_NexusFile_nxflush
  (JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* kill handle */
    HHRemoveHandle(handle);

    /* call NXflush */
    iRet = NXflush(&nxhandle);

    /* error return */
    if(iRet != NX_OK)
    {
      return -1;
    }

    /* convert the NXhandle to a integer handle */
    return HHMakeHandle(nxhandle);
}
/*-----------------------------------------------------------------------
                     close or NXclose
------------------------------------------------------------------------*/ 
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_close
  (JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);
#ifdef DEBUG
    fprintf(stderr,"closing handle %d, nxhandle %d\n", handle, nxhandle);
#endif

    iRet = NXclose(&nxhandle);

    /* kill handle */
    HHRemoveHandle(handle);

    if (iRet != NX_OK) {
      JapiError(env, "NXclose failed");
    }
}
/*------------------------------------------------------------------------
                     nxmakegroup
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxmakegroup
  (JNIEnv *env, jobject obj, jint handle, jstring name, jstring nxclass)
{
    char *Name, *Nxclass;
    NXhandle nxhandle;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    
    Nxclass = (char *) (*env)->GetStringUTFChars(env,nxclass,0);    

    iRet = NXmakegroup(nxhandle, Name, Nxclass);

    /* release strings */
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseStringUTFChars(env,nxclass, Nxclass);

    if (iRet != NX_OK) {
      JapiError(env, "NXmakegroup failed");
    }
}
/*------------------------------------------------------------------------
                     nxopengroup
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxopengroup
  (JNIEnv *env, jobject obj, jint handle, jstring name, jstring nxclass)
{
    char *Name, *Nxclass;
    NXhandle nxhandle;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    
    Nxclass = (char *) (*env)->GetStringUTFChars(env,nxclass,0);    

    iRet = NXopengroup(nxhandle, Name, Nxclass);

#ifdef DEBUG
    if(iRet != NX_OK)
    {
      fprintf(stderr,"Cleanup code called after raising Exception\n");
    }
#endif
    /* release strings */
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseStringUTFChars(env,nxclass, Nxclass);

    if (iRet != NX_OK) {
      JapiError(env, "NXopengroup failed");
    }
}
/*------------------------------------------------------------------------
                     nxopenpath
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxopenpath
  (JNIEnv *env, jobject obj, jint handle, jstring path)
{
    char *nxpath;
    NXhandle nxhandle;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    nxpath = (char *) (*env)->GetStringUTFChars(env,path,0);    

    iRet = NXopenpath(nxhandle, nxpath);

#ifdef DEBUG
    if(iRet != NX_OK)
    {
      fprintf(stderr,"Cleanup code called after raising Exception\n");
    }
#endif
    /* release strings */
    (*env)->ReleaseStringUTFChars(env,path, nxpath);

    if (iRet != NX_OK) {
      JapiError(env, "NXopenpath failed");
    }
}
/*------------------------------------------------------------------------
                     nxopengrouppath
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxopengrouppath
  (JNIEnv *env, jobject obj, jint handle, jstring path)
{
    char *nxpath;
    NXhandle nxhandle;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    nxpath = (char *) (*env)->GetStringUTFChars(env,path,0);    

    iRet = NXopengrouppath(nxhandle, nxpath);

#ifdef DEBUG
    if(iRet != NX_OK)
    {
      fprintf(stderr,"Cleanup code called after raising Exception\n");
    }
#endif
    /* release strings */
    (*env)->ReleaseStringUTFChars(env,path, nxpath);

    if (iRet != NX_OK) {
      JapiError(env, "NXopengrouppath failed");
    }
}
/*-----------------------------------------------------------------------*/
JNIEXPORT jstring JNICALL Java_org_nexusformat_NexusFile_nxgetpath
  (JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;
    char path[1024];

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    if (NXgetpath(nxhandle, path,1024) != NX_OK) {
      JapiError(env, "NXgetpath failed");
    }

    return (*env)->NewStringUTF(env,path);
}

/*------------------------------------------------------------------------
                     nxclosegroup
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxclosegroup
  (JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    if (NXclosegroup(nxhandle) != NX_OK) {
      JapiError(env, "NXclosegroup failed");
    }
}
/*------------------------------------------------------------------------
                               nxmakedata
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxmakedata
  (JNIEnv *env, jobject obj, jint handle, jstring name, jint type, 
  jint rank, jintArray dim)
{
   char *Name;
   NXhandle nxhandle;
   jint *iDim;
   int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    /* access dim array */
    iDim = (*env)->GetIntArrayElements(env,dim,0);

    iRet = NXmakedata(nxhandle,Name,type,rank,iDim);

    /* clean up */ 
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseIntArrayElements(env,dim,iDim,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXmakedata failed");
    }
}
/*------------------------------------------------------------------------
                               nxmakedata64
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxmakedata64
  (JNIEnv *env, jobject obj, jint handle, jstring name, jint type, 
  jint rank, jlongArray dim)
{
   char *Name;
   NXhandle nxhandle;
   jlong *iDim;
   int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    /* access dim array */
    iDim = (*env)->GetLongArrayElements(env,dim,0);

    iRet = NXmakedata64(nxhandle,Name,type,rank,iDim);

    /* clean up */ 
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseLongArrayElements(env,dim,iDim,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXmakedata failed");
    }
}
/*-----------------------------------------------------------------------
                               nxcompmakedata
-------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxmakecompdata
  (JNIEnv *env, jobject obj, jint handle, jstring name, jint type, 
   jint rank, jintArray dim, jint compression_type, jintArray chunk)
{
   char *Name;
   NXhandle nxhandle;
   jint *iDim, *iChunk;
   int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    /* access dim array */
    iDim = (*env)->GetIntArrayElements(env,dim,0);

    /* access the chunksize array */
    iChunk = (*env)->GetIntArrayElements(env,chunk,0);

    iRet = NXcompmakedata(nxhandle,Name,type,rank,iDim,
                          compression_type,iChunk);

    /* clean up */ 
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseIntArrayElements(env,dim,iDim,0);  
    (*env)->ReleaseIntArrayElements(env,chunk,iChunk,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXcompmakedata failed");
    }
}

/*-----------------------------------------------------------------------
                               nxcompmakedata64
-------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxmakecompdata64
  (JNIEnv *env, jobject obj, jint handle, jstring name, jint type, 
   jint rank, jlongArray dim, jint compression_type, jlongArray chunk)
{
   char *Name;
   NXhandle nxhandle;
   jlong *iDim, *iChunk;
   int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    /* access dim array */
    iDim = (*env)->GetLongArrayElements(env,dim,0);

    /* access the chunksize array */
    iChunk = (*env)->GetLongArrayElements(env,chunk,0);

    iRet = NXcompmakedata64(nxhandle,Name,type,rank,iDim,
                          compression_type,iChunk);

    /* clean up */ 
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseLongArrayElements(env,dim,iDim,0);  
    (*env)->ReleaseLongArrayElements(env,chunk,iChunk,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXcompmakedata failed");
    }
}

/*------------------------------------------------------------------------
                               nxopendata
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxopendata
  (JNIEnv *env, jobject obj, jint handle , jstring name)
{
   char *Name;
   NXhandle nxhandle;
   int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    iRet = NXopendata(nxhandle,Name);

    /* clean up */ 
    (*env)->ReleaseStringUTFChars(env,name, Name);

    if (iRet != NX_OK) {
      JapiError(env, "NXopendata failed");
    }
}
/*------------------------------------------------------------------------
                               nxclosedata
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxclosedata
  (JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    if (NXclosedata(nxhandle) != NX_OK) {
      JapiError(env, "NXclosedata failed");
    }
}
/*------------------------------------------------------------------------
                               nxcompress
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxcompress
  (JNIEnv *env, jobject obj, jint handle , jint comp_type)
{
    NXhandle nxhandle;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

#ifdef DEBUG
    fprintf(stderr,"Compressing at %d with type %d\n", nxhandle, comp_type);
#endif

    if (NXcompress(nxhandle,comp_type) != NX_OK) {
      JapiError(env, "NXcompress failed");
    }
}
/*------------------------------------------------------------------------
                               nxputdata
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxputdata
  (JNIEnv *env, jobject obj, jint handle, jbyteArray data)
{
    NXhandle nxhandle;
    jbyte *bdata;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* convert jbteArray to C byte array */
    bdata = (*env)->GetByteArrayElements(env,data,0);

    iRet = NXputdata(nxhandle, bdata);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    if(iRet != NX_OK)
    {
#ifdef DEBUG
      HEprint(stderr,0);
#else
      JapiError(env, "NXputdata failed");
#endif
    }
}
/*------------------------------------------------------------------------
                               nxputslab
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxputslab
  (JNIEnv *env, jobject obj, jint handle, jbyteArray data, 
   jintArray start, jintArray end)
{
    NXhandle nxhandle;
    jbyte *bdata;
    jint *iStart, *iEnd;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* convert arrays to C types  */
    bdata = (*env)->GetByteArrayElements(env,data,0);
    iStart = (*env)->GetIntArrayElements(env,start,0);
    iEnd = (*env)->GetIntArrayElements(env,end,0);


    iRet = NXputslab(nxhandle, bdata, iStart, iEnd);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    (*env)->ReleaseIntArrayElements(env,start,iStart,0);  
    (*env)->ReleaseIntArrayElements(env,end,iEnd,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXputslab failed");
    }
}
/*------------------------------------------------------------------------
                               nxputslab64
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxputslab64
  (JNIEnv *env, jobject obj, jint handle, jbyteArray data, 
   jlongArray start, jlongArray end)
{
    NXhandle nxhandle;
    jbyte *bdata;
    jlong *iStart, *iEnd;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* convert arrays to C types  */
    bdata = (*env)->GetByteArrayElements(env,data,0);
    iStart = (*env)->GetLongArrayElements(env,start,0);
    iEnd = (*env)->GetLongArrayElements(env,end,0);


    iRet = NXputslab64(nxhandle, bdata, iStart, iEnd);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    (*env)->ReleaseLongArrayElements(env,start,iStart,0);  
    (*env)->ReleaseLongArrayElements(env,end,iEnd,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXputslab failed");
    }
}
/*------------------------------------------------------------------------
                               nxputattr
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxputattr
  (JNIEnv *env, jobject obj, jint handle , jstring name, 
           jbyteArray data, jint type)
{
    NXhandle nxhandle;
    jbyte *bdata;
    char *Name;
    int iRet, iDataLen, div = 1;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);
   
    /* convert java types to C types*/
    bdata = (*env)->GetByteArrayElements(env,data,0);
    iDataLen = (*env)->GetArrayLength(env,data);
    switch(type)
    {
        case NX_INT8:
        case NX_UINT8:
        case NX_CHAR:
             div = 1;
             break;
        case NX_UINT16:
        case NX_INT16:
             div = 2;
             break;
        case NX_INT32:
        case NX_UINT32:
        case NX_FLOAT32:
             div = 4;
             break;
        case NX_FLOAT64:
        case NX_INT64:
        case NX_UINT64:
             div = 8;
             break;
        default:
	  JapiError(env, "Bad data type in NXputattr");
	  return;
    }
    iDataLen /=  div;
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    
   
    iRet = NXputattr(nxhandle,Name, bdata, iDataLen, type);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    (*env)->ReleaseStringUTFChars(env,name, Name);

    if (iRet != NX_OK) {
      JapiError(env, "NXputattr failed");
    }
}
/*------------------------------------------------------------------------
                               nxgetdata
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetdata
  (JNIEnv *env, jobject obj, jint handle, jbyteArray data)
{
    NXhandle nxhandle;
    jbyte *bdata;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* convert jbteArray to C byte array */
    bdata = (*env)->GetByteArrayElements(env,data,0);

    iRet = NXgetdata(nxhandle, bdata);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    if(iRet != NX_OK)
    {
#ifdef DEBUG
      HEprint(stderr,0);
#else
      JapiError(env, "NXgetdata failed");
#endif
    }
}
/*------------------------------------------------------------------------
                               nxgetslab
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetslab
  (JNIEnv *env, jobject obj, jint handle, jintArray start, 
   jintArray end, jbyteArray data)
{
    NXhandle nxhandle;
    jbyte *bdata;
    jint *iStart, *iEnd;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* convert arrays to C types  */
    bdata = (*env)->GetByteArrayElements(env,data,0);
    iStart = (*env)->GetIntArrayElements(env,start,0);
    iEnd = (*env)->GetIntArrayElements(env,end,0);

    iRet = NXgetslab(nxhandle, bdata, iStart, iEnd);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    (*env)->ReleaseIntArrayElements(env,start,iStart,0);  
    (*env)->ReleaseIntArrayElements(env,end,iEnd,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXgetslab failed");
    }
}
/*------------------------------------------------------------------------
                               nxgetslab64
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetslab64
  (JNIEnv *env, jobject obj, jint handle, jlongArray start, 
   jlongArray end, jbyteArray data)
{
    NXhandle nxhandle;
    jbyte *bdata;
    jlong *iStart, *iEnd;
    int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* convert arrays to C types  */
    bdata = (*env)->GetByteArrayElements(env,data,0);
    iStart = (*env)->GetLongArrayElements(env,start,0);
    iEnd = (*env)->GetLongArrayElements(env,end,0);

    iRet = NXgetslab64(nxhandle, bdata, iStart, iEnd);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    (*env)->ReleaseLongArrayElements(env,start,iStart,0);  
    (*env)->ReleaseLongArrayElements(env,end,iEnd,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXgetslab failed");
    }
}
/*------------------------------------------------------------------------
                               nxgetattr
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetattr
  (JNIEnv *env, jobject obj, jint handle, jstring name, 
    jbyteArray data, jintArray args)
{
    NXhandle nxhandle;
    jbyte *bdata;
    char *Name;
    int iRet;
    jint *iargs;
    int iLen, iType;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* convert java types to C types*/
    bdata = (*env)->GetByteArrayElements(env,data,0);
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    
    iargs = (*env)->GetIntArrayElements(env,args,0);
#ifdef DEBUG
    fprintf(stderr,"nxgetattr converted types \n");
#endif

    iLen = iargs[0];
    iType = iargs[1];
#ifdef DEBUG
    fprintf(stderr,"nxgetattr: iLen %d, iType: %d\n",iLen, iType);
#endif

    iRet = NXgetattr(nxhandle, Name, bdata, &iLen, &iType);
    iargs[0] = iLen;
    iargs[1] = iType;
#ifdef DEBUG
    fprintf(stderr,"nxgetattr cleaning up \n");
#endif

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseIntArrayElements(env,args,iargs,0);  

    if (iRet != NX_OK) {
      JapiError(env, "NXgetattr failed");
    }
}
/*------------------------------------------------------------------------
                               nxgetgroupid
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetgroupid
  (JNIEnv *env, jobject obj, jint handle, jobject linki)
{
    NXhandle nxhandle;
    NXlink myLink;
    int iRet;
    jclass cls;
    jfieldID fid;
    jstring jstr;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    iRet = NXgetgroupID(nxhandle, &myLink);
    if(iRet == NX_OK)
    {
	/* put the link info from our link structure into the object */
        cls = (*env)->GetObjectClass(env, linki);
        if(cls == NULL)
	{
	    JapiError(env,
	       "ERROR: failed to locate class in nxgetgroupid");
            return;
        }
        fid = (*env)->GetFieldID(env,cls,"tag","I");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate fieldID in nxgetgroupid");
            return;
        }
        (*env)->SetIntField(env,linki,fid,myLink.iTag);

        fid = (*env)->GetFieldID(env,cls,"ref","I");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate fieldID in nxgetgroupid");
            return;
        }
        (*env)->SetIntField(env,linki,fid,myLink.iRef);

#ifdef HDF5
        /* 
         set HDF-5 String variables
	*/
        fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate targetPath in nxgetgroupid");
            return;
        }
	jstr = (*env)->NewStringUTF(env,myLink.targetPath);
        (*env)->SetObjectField(env, linki, fid, jstr);

        fid = (*env)->GetFieldID(env,cls,"linkType","I");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate linkType in nxgetgroupid");
            return;
        }
        (*env)->SetIntField(env,linki,fid,myLink.linkType);

#endif        
        fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate targetPath in nxgetgroupid");
            return;
        }
	jstr = (*env)->NewStringUTF(env,myLink.targetPath);
        (*env)->SetObjectField(env, linki, fid, jstr);
    }
}
/*------------------------------------------------------------------------
                               nxgetgroupid
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetdataid
  (JNIEnv *env, jobject obj, jint handle, jobject linki)
{
    NXhandle nxhandle;
    NXlink myLink;
    int iRet;
    jclass cls;
    jfieldID fid;
    jstring jstr;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    iRet = NXgetdataID(nxhandle, &myLink);
    if(iRet == NX_OK)
    {
	/* put the link info from our link structure into the object */
        cls = (*env)->GetObjectClass(env, linki);
        if(cls == NULL)
	{
	    JapiError(env,
	       "ERROR: failed to locate class in nxgetdataid");
            return;
        }
        fid = (*env)->GetFieldID(env,cls,"tag","I");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate fieldID in nxgetdataid");
            return;
        }
        (*env)->SetIntField(env,linki,fid,myLink.iTag);
        fid = (*env)->GetFieldID(env,cls,"ref","I");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate fieldID in nxgetdataid");
            return;
        }
        (*env)->SetIntField(env,linki,fid,myLink.iRef);

#ifdef HDF5
        /* 
         set HDF-5 String variables
	*/
        fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate targetPath in nxgetgroupid");
            return;
        }
	jstr = (*env)->NewStringUTF(env,myLink.targetPath);
        (*env)->SetObjectField(env, linki, fid, jstr);

        fid = (*env)->GetFieldID(env,cls,"linkType","I");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate linkType in nxgetgroupid");
            return;
        }
        (*env)->SetIntField(env,linki,fid,myLink.linkType);

#endif
        fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
        if(fid == 0)
	{
	    JapiError(env,
	       "ERROR: failed to locate targetPath in nxgetdataid");
            return;
        }
	jstr = (*env)->NewStringUTF(env,myLink.targetPath);
        (*env)->SetObjectField(env, linki, fid, jstr);

    }
}
/*------------------------------------------------------------------------
                               nxmakelink
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxmakelink
  (JNIEnv *env, jobject obj, jint handle, jobject target)
{
    NXhandle nxhandle;
    NXlink myLink;
    jclass cls;
    jfieldID fid;
    jstring jstr;
    const char *cData;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    // convert target object data to myLink structure */
    cls = (*env)->GetObjectClass(env, target);
    if(cls == NULL)
    {
	 JapiError(env,
	       "ERROR: failed to locate class in nxmakelink");
         return;
     }
     fid = (*env)->GetFieldID(env,cls,"tag","I");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate fieldID in nxmakelink");
          return;
     }
     myLink.iTag = (*env)->GetIntField(env,target,fid);
     fid = (*env)->GetFieldID(env,cls,"ref","I");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate fieldID in nxmakelink");
          return;
     }
     myLink.iRef = (*env)->GetIntField(env,target,fid);

#ifdef HDF5    
     /*
       get the HDF-5 Strings
     */
     fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate targetPath in nxmakelink");
          return;
     }
     jstr = (*env)->GetObjectField(env, target, fid);
     cData = (*env)->GetStringUTFChars(env, jstr, 0);          
     strcpy(myLink.targetPath,cData);
     (*env)->ReleaseStringUTFChars(env, jstr, cData);

     fid = (*env)->GetFieldID(env,cls,"linkType","I");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate linkType in nxmakelink");
          return;
     }
     myLink.linkType = (*env)->GetIntField(env,target,fid);

#endif
     fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate targetPath in nxmakelink");
          return;
     }
     jstr = (*env)->GetObjectField(env, target, fid);
     cData = (*env)->GetStringUTFChars(env, jstr, 0);          
     strcpy(myLink.targetPath,cData);
     (*env)->ReleaseStringUTFChars(env, jstr, cData);

     // do actually link
     if (NXmakelink(nxhandle, &myLink) != NX_OK) {
       JapiError(env, "NXmakelink failed");
     }
}
/*------------------------------------------------------------------------
                               nxmakenamedlink
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxmakenamedlink
  (JNIEnv *env, jobject obj, jint handle, jstring name, jobject target)
{
    NXhandle nxhandle;
    NXlink myLink;
    jclass cls;
    jfieldID fid;
    jstring jstr;
    const char *cData;
    char *Name; 

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* get link name */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    // convert target object data to myLink structure */
    cls = (*env)->GetObjectClass(env, target);
    if(cls == NULL)
    {
	 JapiError(env,
	       "ERROR: failed to locate class in nxmakelink");
         return;
     }
     fid = (*env)->GetFieldID(env,cls,"tag","I");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate fieldID in nxmakelink");
          return;
     }
     myLink.iTag = (*env)->GetIntField(env,target,fid);
     fid = (*env)->GetFieldID(env,cls,"ref","I");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate fieldID in nxmakelink");
          return;
     }
     myLink.iRef = (*env)->GetIntField(env,target,fid);

#ifdef HDF5    
     /*
       get the HDF-5 Strings
     */
     fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate targetPath in nxmakelink");
          return;
     }
     jstr = (*env)->GetObjectField(env, target, fid);
     cData = (*env)->GetStringUTFChars(env, jstr, 0);          
     strcpy(myLink.targetPath,cData);
     (*env)->ReleaseStringUTFChars(env, jstr, cData);

     fid = (*env)->GetFieldID(env,cls,"linkType","I");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate linkType in nxmakelink");
          return;
     }
     myLink.linkType = (*env)->GetIntField(env,target,fid);

#endif
     fid = (*env)->GetFieldID(env,cls,"targetPath","Ljava/lang/String;");
     if(fid == 0)
     {
	  JapiError(env,
	       "ERROR: failed to locate targetPath in nxmakelink");
          return;
     }
     jstr = (*env)->GetObjectField(env, target, fid);
     cData = (*env)->GetStringUTFChars(env, jstr, 0);          
     strcpy(myLink.targetPath,cData);
     (*env)->ReleaseStringUTFChars(env, jstr, cData);

     // do actually link
     if (NXmakenamedlink(nxhandle, Name,  &myLink) != NX_OK) {
       JapiError(env, "NXmakenamedlink failed");
     }
}

/*------------------------------------------------------------------------
                     nxopensourcepath
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxopensourcegroup
  (JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    if (NXopensourcegroup(nxhandle) != NX_OK) {
       JapiError(env, "NXopensourcegroup failed");
     }
}

/*----------------------------------------------------------------------
                           nxsetnumberformat
-----------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxsetnumberformat
    (JNIEnv *env, jobject obj, jint handle, jint type, jstring format)
{
    NXhandle nxhandle;
    char *cformat;
    int iRet;

   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /*
      extract format string
    */
    cformat = (char *) (*env)->GetStringUTFChars(env,format,0);

    /*
      call
    */
    iRet = NXsetnumberformat(nxhandle,type,cformat);
    /*
      release format string
    */ 
    (*env)->ReleaseStringUTFChars(env,format, cformat);

    if (iRet != NX_OK) {
      JapiError(env, "NXsetnumberformat failed");
    }
}
/*------------------------------------------------------------------------
                               nxgetinfo
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetinfo
    (JNIEnv *env, jobject obj, jint handle, jintArray dim, jintArray args)
{
    int rank, type, iRet, iDim[NX_MAXRANK], i;
    NXhandle nxhandle;
    jint *jdata;

   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* call */
    iRet = NXgetinfo(nxhandle, &rank, iDim, &type);

    /* copy data to Java types */
    if(iRet == NX_OK)
    {
	jdata = (*env)->GetIntArrayElements(env,dim,0);
        for(i = 0; i < rank; i++)
	{
           jdata[i] = iDim[i];
        }
        (*env)->ReleaseIntArrayElements(env,dim,jdata,0);
	jdata = (*env)->GetIntArrayElements(env,args,0);
        jdata[0] = rank;
        jdata[1] = type;
        (*env)->ReleaseIntArrayElements(env,args,jdata,0);
    }
}
/*------------------------------------------------------------------------
                               nxgetinfo64
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetinfo64
    (JNIEnv *env, jobject obj, jint handle, jlongArray dim, jintArray args)
{
    int rank, type, iRet, i;
    jlong iDim[NX_MAXRANK];
    NXhandle nxhandle;
    jlong *jdata;
    jint *jargsdata;

   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* call */
    iRet = NXgetinfo64(nxhandle, &rank, iDim, &type);

    /* copy data to Java types */
    if(iRet == NX_OK)
    {
	jdata = (*env)->GetLongArrayElements(env,dim,0);
        for(i = 0; i < rank; i++)
	{
           jdata[i] = iDim[i];
        }
        (*env)->ReleaseLongArrayElements(env,dim,jdata,0);

	jargsdata = (*env)->GetIntArrayElements(env,args,0);
        jargsdata[0] = rank;
        jargsdata[1] = type;
        (*env)->ReleaseIntArrayElements(env,args,jargsdata,0);
    }
}
/*------------------------------------------------------------------------
                               nextentry
--------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_org_nexusformat_NexusFile_nextentry
  (JNIEnv *env, jobject obj, jint handle, jobjectArray jnames)
{
    NXhandle nxhandle;
    NXname pName, pClass;
    int iRet, iType;
    jstring rstring;

   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    iRet = NXgetnextentry(nxhandle,pName, pClass,&iType);
    if(iRet != NX_ERROR)
    {
	/* convert C strings to Java Strings */
        rstring = (*env)->NewStringUTF(env,pName);
        (*env)->SetObjectArrayElement(env,jnames,0,(jobject)rstring);
        rstring = (*env)->NewStringUTF(env,pClass);
        (*env)->SetObjectArrayElement(env,jnames,1,(jobject)rstring);
    }
    return iRet;
}
/*------------------------------------------------------------------------
                               nextattr
--------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_org_nexusformat_NexusFile_nextattr
  (JNIEnv *env, jobject obj, jint handle, jobjectArray jnames, jintArray args)
{
    NXhandle nxhandle;
    NXname pName;
    int iRet, iType, iLen;
    jstring rstring;
    jint *jarray;
   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    iRet = NXgetnextattr(nxhandle, pName, &iLen, &iType);
    if(iRet == NX_OK)
    {
        /* copy C types to Java */
        rstring = (*env)->NewStringUTF(env,pName);
        (*env)->SetObjectArrayElement(env,jnames,0,(jobject)rstring);
	jarray = (*env)->GetIntArrayElements(env,args,0);
        jarray[0] = iLen;
        jarray[1] = iType;
        (*env)->ReleaseIntArrayElements(env,args,jarray,0);
    }
    return iRet;
}
/*-----------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxinquirefile(JNIEnv *env, 
				      jobject obj , jint handle, jobjectArray jnames){
    NXhandle nxhandle;
    int status;
    char filename[1024];
    jstring rstring;

   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);
    status = NXinquirefile(nxhandle,filename,1023);
    if(status == NX_OK){
      rstring = (*env)->NewStringUTF(env,filename);
      (*env)->SetObjectArrayElement(env,jnames,0,(jobject)rstring);
    }
}
/*------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxlinkexternal
(JNIEnv *env, jobject obj, jint handle, jstring name, 
 jstring nxclass, jstring nxurl){
    int iRet;
    NXhandle nxhandle;
    char *Name, *Nxclass, *Nxurl;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);
  
    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    
    Nxclass = (char *) (*env)->GetStringUTFChars(env,nxclass,0);    
    Nxurl = (char *) (*env)->GetStringUTFChars(env,nxurl,0);    
    iRet = NXlinkexternal(nxhandle,Name,Nxclass,Nxurl);
    
    /* release strings */
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseStringUTFChars(env,nxclass, Nxclass);
    (*env)->ReleaseStringUTFChars(env,nxurl, Nxurl);

    if (iRet != NX_OK) {
      JapiError(env, "NXlinkexternal failed");
    }
}
/*------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxlinkexternaldataset
(JNIEnv *env, jobject obj, jint handle, jstring name, jstring nxurl){
    int iRet;
    NXhandle nxhandle;
    char *Name, *Nxurl;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);
  
    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    
    Nxurl = (char *) (*env)->GetStringUTFChars(env,nxurl,0);    
    iRet = NXlinkexternaldataset(nxhandle,Name,Nxurl);
    
    /* release strings */
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseStringUTFChars(env,nxurl, Nxurl);

    if (iRet != NX_OK) {
      JapiError(env, "NXlinkexternaldataset failed");
    }
}
/*------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_org_nexusformat_NexusFile_nxisexternalgroup
(JNIEnv *env, jobject obj, jint handle, jstring name, jstring nxclass, 
 jobjectArray jnames){
    int status, length = 1024;
    NXhandle nxhandle;
    char *Name, *Nxclass, nxurl[1024];
    jstring rstring;
    
    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);
  
    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    
    Nxclass = (char *) (*env)->GetStringUTFChars(env,nxclass,0);    

    status = NXisexternalgroup(nxhandle,Name,Nxclass,nxurl,length);

    /* release strings */
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseStringUTFChars(env,nxclass, Nxclass);
    
    if(status == NX_OK){
      rstring = (*env)->NewStringUTF(env,nxurl);
      (*env)->SetObjectArrayElement(env,jnames,0,(jobject)rstring);
    }
    return status;
}
/*------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_org_nexusformat_NexusFile_nxisexternaldataset
(JNIEnv *env, jobject obj, jint handle, jstring name, jobjectArray jnames){
    int status, length = 1024;
    NXhandle nxhandle;
    char *Name, nxurl[1024];
    jstring rstring;
    
    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);
  
    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    status = NXisexternaldataset(nxhandle,Name,nxurl,length);

    /* release strings */
    (*env)->ReleaseStringUTFChars(env,name, Name);
    
    if(status == NX_OK){
      rstring = (*env)->NewStringUTF(env,nxurl);
      (*env)->SetObjectArrayElement(env,jnames,0,(jobject)rstring);
    }
    return status;
}
/*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_initattrdir
(JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    if (NXinitattrdir(nxhandle) != NX_OK) {
      JapiError(env, "NXinitattrdir failed");
    }
}
/*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_initgroupdir
(JNIEnv *env, jobject obj, jint handle)
{
    NXhandle nxhandle;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    if (NXinitgroupdir(nxhandle) != NX_OK) {
      JapiError(env, "NXinitgroupdir failed");
    }
}
/*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxputattra
(JNIEnv *env, jobject obj, jint handle, jstring name, jbyteArray data, jint rank, jintArray dim, jint type)
{
   char *Name;
   NXhandle nxhandle;
   jbyte *bdata;
   jint *iDim;
   int iRet;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    /* extract the name and class to char * */
    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    /* access dim array */
    iDim = (*env)->GetIntArrayElements(env,dim,0);

    /* convert jbteArray to C byte array */
    bdata = (*env)->GetByteArrayElements(env,data,0);

    iRet = NXputattra(nxhandle, Name, bdata, rank, iDim, type);

    /* clean up */ 
    (*env)->ReleaseStringUTFChars(env,name, Name);
    (*env)->ReleaseIntArrayElements(env,dim,iDim,0);  

    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    if(iRet != NX_OK) {
      JapiError(env, "NXputattra failed");
    }
}
/*---------------------------------------------------------------------*/
JNIEXPORT jint JNICALL Java_org_nexusformat_NexusFile_nxgetnextattra
(JNIEnv *env, jobject obj, jint handle, jobjectArray jnames, jintArray dim, jintArray args)
{
    NXhandle nxhandle;
    NXname pName;
    int iRet, iType, rank, iDim[NX_MAXRANK], i;
    jstring rstring;
    jint *jdata;

   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    iRet = NXgetnextattra(nxhandle, pName, &rank, iDim, &iType);
    if(iRet == NX_OK) {
        /* copy C types to Java */
        rstring = (*env)->NewStringUTF(env,pName);
        (*env)->SetObjectArrayElement(env,jnames,0,(jobject)rstring);
	jdata = (*env)->GetIntArrayElements(env,dim,0);
        for(i = 0; i < rank; i++) {
           jdata[i] = iDim[i];
        }
        (*env)->ReleaseIntArrayElements(env,dim,jdata,0);
	jdata = (*env)->GetIntArrayElements(env,args,0);
        jdata[0] = rank;
        jdata[1] = iType;
        (*env)->ReleaseIntArrayElements(env,args,jdata,0);
    }
    return iRet;
}
/*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetattra
  (JNIEnv *env, jobject obj, jint handle, jstring name, jbyteArray data)
{
    NXhandle nxhandle;
    jbyte *bdata;
    int iRet;
    char* Name;

    /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    /* convert jbteArray to C byte array */
    bdata = (*env)->GetByteArrayElements(env,data,0);

    iRet = NXgetattra(nxhandle, Name, bdata);

    /* cleanup */
    (*env)->ReleaseByteArrayElements(env,data,bdata,0);   
    if(iRet != NX_OK)
    {
#ifdef DEBUG
      HEprint(stderr,0);
#else
      JapiError(env, "NXgetdata failed");
#endif
    }
}
/*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_nxgetattrainfo
    (JNIEnv *env, jobject obj, jint handle, jstring name, jintArray dim, jintArray args)
{
    int rank, type, iRet, iDim[NX_MAXRANK], i;
    NXhandle nxhandle;
    jint *jdata;
    char* Name;

   /* set error handler */
    NXMSetTError(env,JapiError);

    /* exchange the Java handler to a NXhandle */
    nxhandle =  (NXhandle)HHGetPointer(handle);

    Name = (char *) (*env)->GetStringUTFChars(env,name,0);    

    /* call */
    iRet = NXgetattrainfo(nxhandle, Name, &rank, iDim, &type);

    /* copy data to Java types */
    if(iRet == NX_OK)
    {
	jdata = (*env)->GetIntArrayElements(env,dim,0);
        for(i = 0; i < rank; i++)
	{
           jdata[i] = iDim[i];
        }
        (*env)->ReleaseIntArrayElements(env,dim,jdata,0);
	jdata = (*env)->GetIntArrayElements(env,args,0);
        jdata[0] = rank;
        jdata[1] = type;
        (*env)->ReleaseIntArrayElements(env,args,jdata,0);
    }
}
/*------------------------------------------------------------------------
                               debugstop
--------------------------------------------------------------------------*/

JNIEXPORT void JNICALL Java_org_nexusformat_NexusFile_debugstop
  (JNIEnv *env, jobject obj)
{
   int iStop = 1;

   while(iStop)
   {
/*       sleep(2); */
   }
}
