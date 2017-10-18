
/****************************************************************************
 * NCSA HDF                                                                 *
 * National Comptational Science Alliance                                   *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/

/*
 *  This is a utility program used by the HDF Java-C wrapper layer to
 *  generate exceptions.  This may be called from any part of the
 *  Java-C interface.
 *
 */

#include <stdio.h>
#include "jni.h"
#include "hdfexceptionImp.h"

jboolean buildException( JNIEnv *env, jint HDFerr)
{
jmethodID jm;
jclass jc;
int args[2];
jobject ex;
int rval;


	jc = (*env)->FindClass(env, "ncsa/hdf/hdflib/HDFLibraryException");
	if (jc == NULL) {
		return JNI_FALSE;
	}
	jm = (*env)->GetMethodID(env, jc, "<init>", "(I)V");
	if (jm == NULL) {
		return JNI_FALSE;
	}
	args[0] = HDFerr;
	args[1] = 0;

	ex = (*env)->NewObjectA ( env, jc, jm, (jvalue *)args );

	rval = (*env)->Throw(env, ex );

	return JNI_TRUE;
}

jboolean NotImplemented( JNIEnv *env, char *functName)
{
jmethodID jm;
jclass jc;
char * args[2];
jobject ex;
jstring str;
int rval;


	jc = (*env)->FindClass(env, "ncsa/hdf/hdflib/HDFNotImplementedException");
	if (jc == NULL) {
		return JNI_FALSE;
	}
	jm = (*env)->GetMethodID(env, jc, "<init>", "(Ljava/lang/String;)V");
	if (jm == NULL) {
		return JNI_FALSE;
	}

	str = (*env)->NewStringUTF(env,functName);
	args[0] = (char *)str;
	args[1] = 0;
	ex = (*env)->NewObjectA ( env, jc, jm, (jvalue *)args );

	rval = (*env)->Throw(env, ex );

	return JNI_TRUE;
}

jboolean outOfMemory( JNIEnv *env, char *functName)
{
jmethodID jm;
jclass jc;
char * args[2];
jobject ex;
jstring str;
int rval;

	jc = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
	if (jc == NULL) {
		return JNI_FALSE;
	}
	jm = (*env)->GetMethodID(env, jc, "<init>", "(Ljava/lang/String;)V");
	if (jm == NULL) {
		return JNI_FALSE;
	}

	str = (*env)->NewStringUTF(env,functName);
	args[0] = (char *)str;
	args[1] = 0;
	ex = (*env)->NewObjectA ( env, jc, jm, (jvalue *)args );

	rval = (*env)->Throw(env, ex );

	return JNI_TRUE;
}


/*
 *  A fatal error in a JNI call
 */
jboolean JNIFatalError( JNIEnv *env, char *functName)
{
jmethodID jm;
jclass jc;
char * args[2];
jobject ex;
jstring str;
int rval;

	jc = (*env)->FindClass(env, "java/lang/InternalError");
	if (jc == NULL) {
		return JNI_FALSE;
	}
	jm = (*env)->GetMethodID(env, jc, "<init>", "(Ljava/lang/String;)V");
	if (jm == NULL) {
		return JNI_FALSE;
	}

	str = (*env)->NewStringUTF(env,functName);
	args[0] = (char *)str;
	args[1] = 0;
	ex = (*env)->NewObjectA ( env, jc, jm, (jvalue *)args );

	rval = (*env)->Throw(env, ex );

	return JNI_TRUE;
}

jboolean raiseException( JNIEnv *env, char *message)
{
jmethodID jm;
jclass jc;
char * args[2];
jobject ex;
jstring str;
int rval;

	jc = (*env)->FindClass(env, "ncsa/hdf/hdflib/HDFLibraryException");
	if (jc == NULL) {
		return JNI_FALSE;
	}
	jm = (*env)->GetMethodID(env, jc, "<init>", "(Ljava/lang/String;)V");
	if (jm == NULL) {
		return JNI_FALSE;
	}

	str = (*env)->NewStringUTF(env,message);
	args[0] = (char *)str;
	args[1] = 0;
	ex = (*env)->NewObjectA ( env, jc, jm, (jvalue *)args );

	rval = (*env)->Throw(env, ex );

	return JNI_TRUE;
}
