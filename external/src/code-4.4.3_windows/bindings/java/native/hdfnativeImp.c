
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
 *  This module contains the implementation of all the native methods
 *  used for number conversion.  This is represented by the Java
 *  class HDFNativeData.
 *
 *  These routines convert one dimensional arrays of bytes into
 *  one-D arrays of other types (int, float, etc) and vice versa.
 *
 *  These routines are called from the Java parts of the Java-C
 *  interface.
 *
 *  ***Important notes:
 *
 *     1.  These routines are designed to be portable--they use the
 *         C compiler to do the required native data manipulation.
 *     2.  These routines copy the data at least once -- a serious
 *         but unavoidable performance hit.
 */

#include <stdio.h>

/* #include "hdf.h"    /* this only seems to be needed to define "intn" */
typedef int intn;

#include "jni.h"
#include "hdfexceptionImp.h"

/* returns int [] */
JNIEXPORT jintArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToInt___3B 
( JNIEnv *env,
jclass class, 
jbyteArray bdata)  /* IN: array of bytes */
{
	intn rval;
	jbyte *barr;
	jintArray rarray;
	int blen;
	jint *iarray;
	jboolean bb;
	char *bp;
	jint *iap;
	int ii;
	int len;

	if (bdata == NULL) {
		raiseException( env, "byteToInt: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToInt: pin failed");
		return NULL;
	}

	blen = (*env)->GetArrayLength(env,bdata);

	len = blen/sizeof(jint);
	rarray = (*env)->NewIntArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToInt" );
		return NULL;
	}

	iarray = (*env)->GetIntArrayElements(env,rarray,&bb);
	if (iarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToInt: pin iarray failed");
		return NULL;
	}

	bp = (char *)barr;
	iap = iarray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jint *)bp;
		iap++;
		bp += sizeof(jint);
	}

	(*env)->ReleaseIntArrayElements(env,rarray,iarray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}

/* returns float [] */
JNIEXPORT jfloatArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToFloat___3B 
( JNIEnv *env,
jclass class, 
jbyteArray bdata)  /* IN: array of bytes */
{
	jbyte *barr;
	jfloatArray rarray;
	int blen;
	jfloat *farray;
	jboolean bb;
	char *bp;
	jfloat *iap;
	int ii;
	int len;

	if (bdata == NULL) {
		raiseException( env, "byteToFloat: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToFloat: pin failed");
		return NULL;
	}
	blen = (*env)->GetArrayLength(env,bdata);

	len = blen/sizeof(jfloat);
	rarray = (*env)->NewFloatArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToFloat" );
		return NULL;
	}
	farray = (*env)->GetFloatArrayElements(env,rarray,&bb);
	if (farray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToFloat: pin farray failed");
		return NULL;
	}

	bp = (char *)barr;
	iap = farray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jfloat *)bp;
		iap++;
		bp += sizeof(jfloat);
	}

	(*env)->ReleaseFloatArrayElements(env,rarray,farray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}

/* returns short [] */
JNIEXPORT jshortArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToShort___3B 
( JNIEnv *env,
jclass class, 
jbyteArray bdata)  /* IN: array of bytes */
{
	jbyte *barr;
	jshortArray rarray;
	int blen;
	jshort *sarray;
	jboolean bb;
	char *bp;
	jshort *iap;
	int ii;
	int len;

	if (bdata == NULL) {
		raiseException( env, "byteToShort: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToShort: pin failed");
		return NULL;
	}

	blen = (*env)->GetArrayLength(env,bdata);

	len = blen/sizeof(jshort);
	rarray = (*env)->NewShortArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToShort" );
		return NULL;
	}

	sarray = (*env)->GetShortArrayElements(env,rarray,&bb);
	if (sarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToShort: pin sarray failed");
		return NULL;
	}

	bp = (char *)barr;
	iap = sarray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jshort *)bp;
		iap++;
		bp += sizeof(jshort);
	}

	(*env)->ReleaseShortArrayElements(env,rarray,sarray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}


/* returns long [] */
JNIEXPORT jlongArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToLong___3B 
( JNIEnv *env,
jclass class, 
jbyteArray bdata)  /* IN: array of bytes */
{
	jbyte *barr;
	jlongArray rarray;
	int blen;
	jlong *larray;
	jboolean bb;
	char *bp;
	jlong *iap;
	int ii;
	int len;

	if (bdata == NULL) {
		raiseException( env, "byteToLong: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToLong: pin failed");
		return NULL;
	}
	blen = (*env)->GetArrayLength(env,bdata);

	len = blen/sizeof(jlong);
	rarray = (*env)->NewLongArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToLong" );
		return NULL;
	}

	larray = (*env)->GetLongArrayElements(env,rarray,&bb);
	if (larray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToLong: pin larray failed");
		return NULL;
	}

	bp = (char *)barr;
	iap = larray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jlong *)bp;
		iap++;
		bp += sizeof(jlong);
	}
	(*env)->ReleaseLongArrayElements(env,rarray,larray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}


/* returns double [] */
JNIEXPORT jdoubleArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToDouble___3B 
( JNIEnv *env,
jclass class, 
jbyteArray bdata)  /* IN: array of bytes */
{
	jbyte *barr;
	jdoubleArray rarray;
	int blen;
	jdouble *darray;
	jboolean bb;
	char *bp;
	jdouble *iap;
	int ii;
	int len;

	if (bdata == NULL) {
		raiseException( env, "byteToDouble: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToDouble: pin failed");
		return NULL;
	}
	blen = (*env)->GetArrayLength(env,bdata);

	len = blen/sizeof(jdouble);
	rarray = (*env)->NewDoubleArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToDouble" );
		return NULL;
	}

	darray = (*env)->GetDoubleArrayElements(env,rarray,&bb);
	if (darray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToDouble: pin darray failed");
		return NULL;
	}

	bp = (char *)barr;
	iap = darray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jdouble *)bp;
		iap++;
		bp += sizeof(jdouble);
	}

	(*env)->ReleaseDoubleArrayElements(env,rarray,darray,0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;
}


/* returns int [] */
JNIEXPORT jintArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToInt__II_3B 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jbyteArray bdata)  /* IN: array of bytes */
{
	char *bp;
	jbyte *barr;
	jintArray rarray;
	int blen;
	jint *iarray;
	jint *iap;
	int ii;
	jboolean bb;

	if (bdata == NULL) {
		raiseException( env, "byteToInt: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToInt: pin failed");
		return NULL;
	}

	blen = (*env)->GetArrayLength(env,bdata);
	if ((start < 0) || ((start + (len*sizeof(jint))) > blen)) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToInt: getLen failed");
		return NULL;
	}

	bp = (char *)barr + start;

	rarray = (*env)->NewIntArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToInt" );
		return NULL;
	}

	iarray = (*env)->GetIntArrayElements(env,rarray,&bb);
	if (iarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToInt: pin iarray failed");
		return NULL;
	}

	iap = iarray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jint *)bp;
		iap++;
		bp += sizeof(jint);
	}

	(*env)->ReleaseIntArrayElements(env,rarray,iarray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}

/* returns short [] */
JNIEXPORT jshortArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToShort__II_3B 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jbyteArray bdata)  /* IN: array of bytes */
{
	char *bp;
	jbyte *barr;
	jshortArray rarray;
	int blen;
	jshort *iarray;
	jshort *iap;
	int ii;
	jboolean bb;

	if (bdata == NULL) {
		raiseException( env, "byteToShort: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToShort: getByte failed?");
		return NULL;
	}

	blen = (*env)->GetArrayLength(env,bdata);
	if ((start < 0) || ((start + (len*(sizeof(jshort)))) > blen)) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		raiseException( env, "byteToShort: start or len is out of bounds");
		return NULL;
	}

	bp = (char *)barr + start;

	rarray = (*env)->NewShortArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToShort" );
		return NULL;
	}

	iarray = (*env)->GetShortArrayElements(env,rarray,&bb);
	if (iarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToShort: getShort failed?");
		return NULL;
	}

	iap = iarray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jshort *)bp;
		iap++;
		bp += sizeof(jshort);
	}

	(*env)->ReleaseShortArrayElements(env,rarray,iarray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}

/* returns float [] */
JNIEXPORT jfloatArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToFloat__II_3B 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jbyteArray bdata)  /* IN: array of bytes */
{
	char *bp;
	jbyte *barr;
	jfloatArray rarray;
	int blen;
	jfloat *iarray;
	jfloat *iap;
	int ii;
	jboolean bb;

	if (bdata == NULL) {
		raiseException( env, "byteToFloat: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToFloat: getByte failed?");
		return NULL;
	}

	blen = (*env)->GetArrayLength(env,bdata);
	if ((start < 0) || ((start + (len*(sizeof(jfloat)))) > blen)) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		raiseException( env, "byteToFloat: start or len is out of bounds");
		return NULL;
	}

	bp = (char *)barr + start;

	rarray = (*env)->NewFloatArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToFloat" );
		return NULL;
	}

	iarray = (*env)->GetFloatArrayElements(env,rarray,&bb);
	if (iarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToFloat: getFloat failed?");
		return NULL;
	}

	iap = iarray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jfloat *)bp;
		iap++;
		bp += sizeof(jfloat);
	}

	(*env)->ReleaseFloatArrayElements(env,rarray,iarray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}

/* returns long [] */
JNIEXPORT jlongArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToLong__II_3B 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jbyteArray bdata)  /* IN: array of bytes */
{
	char *bp;
	jbyte *barr;
	jlongArray rarray;
	int blen;
	jlong *iarray;
	jlong *iap;
	int ii;
	jboolean bb;

	if (bdata == NULL) {
		raiseException( env, "byteToLong: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToLong: getByte failed?");
		return NULL;
	}

	blen = (*env)->GetArrayLength(env,bdata);
	if ((start < 0) || ((start + (len*(sizeof(jlong)))) > blen)) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		raiseException( env, "byteToLong: start or len is out of bounds");
		return NULL;
	}

	bp = (char *)barr + start;

	rarray = (*env)->NewLongArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToLong" );
		return NULL;
	}

	iarray = (*env)->GetLongArrayElements(env,rarray,&bb);
	if (iarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		JNIFatalError( env, "byteToLong: getLong failed?");
		return NULL;
	}

	iap = iarray;
	for (ii = 0; ii < len; ii++) {

		*iap = *(jlong *)bp;
		iap++;
		bp += sizeof(jlong);
	}

	(*env)->ReleaseLongArrayElements(env,rarray,iarray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;

}

/* returns double [] */
JNIEXPORT jdoubleArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToDouble__II_3B 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jbyteArray bdata)  /* IN: array of bytes */
{
	char *bp;
	jbyte *barr;
	jdoubleArray rarray;
	int blen;
	jdouble *iarray;
	jdouble *iap;
	int ii;
	jboolean bb;

	if (bdata == NULL) {
		raiseException( env, "byteToDouble: bdata is NULL?");
		return NULL;
	}
	barr = (*env)->GetByteArrayElements(env,bdata,&bb);
	if (barr == NULL) {
		JNIFatalError( env, "byteToDouble: getByte failed?");
		return NULL;
	}

	blen = (*env)->GetArrayLength(env,bdata);
	if ((start < 0) || ((start + (len*(sizeof(jdouble)))) > blen)) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		raiseException( env, "byteToDouble: start or len is out of bounds");
		return NULL;
	}

	bp = (char *)barr + start;

	rarray = (*env)->NewDoubleArray(env,len);
	if (rarray == NULL) {
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		outOfMemory( env, "byteToDouble" );
		return NULL;
	}

	iarray = (*env)->GetDoubleArrayElements(env,rarray,&bb);
	if (iarray == NULL) {
		JNIFatalError( env, "byteToDouble: getDouble failed?");
		(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);
		return NULL;
	}

	iap = iarray;
	for (ii = 0; ii < len; ii++) {
		*iap = *(jdouble *)bp;
		iap++;
		bp += sizeof(jdouble);
	}

	(*env)->ReleaseDoubleArrayElements(env,rarray,iarray, 0);
	(*env)->ReleaseByteArrayElements(env,bdata,barr,JNI_ABORT);

	return rarray;
}

/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_intToByte__II_3I 
(JNIEnv *env,
jclass class, 
jint start,
jint len,
jintArray idata)  /* IN: array of int */
{
	jint *ip;
	jint *iarr;
	int ilen;
	jbyteArray rarray;
	int blen;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ii;
	int ij;
	union things {
		int ival;
		char bytes[4];
	} u;

	if (idata == NULL) {
		raiseException( env, "intToByte: idata is NULL?");
		return NULL;
	}
	iarr = (*env)->GetIntArrayElements(env,idata,&bb);
	if (iarr == NULL) {
		JNIFatalError( env, "intToByte: getInt failed?");
		return NULL;
	}

	ilen = (*env)->GetArrayLength(env,idata);
	if ((start < 0) || (((start + len)) > ilen)) {
		(*env)->ReleaseIntArrayElements(env,idata,iarr,JNI_ABORT);
		raiseException( env, "intToByte: start or len is out of bounds");
		return NULL;
	}

	ip = iarr + start;

	blen = ilen * sizeof(jint);
	rarray = (*env)->NewByteArray(env,blen);
	if (rarray == NULL) {
		(*env)->ReleaseIntArrayElements(env,idata,iarr,JNI_ABORT);
		outOfMemory( env, "intToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		(*env)->ReleaseIntArrayElements(env,idata,iarr,JNI_ABORT);
		JNIFatalError( env, "intToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	for (ii = 0; ii < len; ii++) {
		u.ival = *ip++;
		for (ij = 0; ij < sizeof(jint); ij++) {
			*bap = u.bytes[ij];
			bap++;
		}
	}

	(*env)->ReleaseByteArrayElements(env,rarray,barray, 0);
	(*env)->ReleaseIntArrayElements(env,idata,iarr,JNI_ABORT);

	return rarray;

}

/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_shortToByte__II_3S 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jshortArray idata)  /* IN: array of short */
{
	jshort *ip;
	jshort *iarr;
	int ilen;
	jbyteArray rarray;
	int blen;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ii;
	int ij;
	union things {
		short ival;
		char bytes[4];
	} u;

	if (idata == NULL) {
		raiseException( env, "shortToByte: idata is NULL?");
		return NULL;
	}
	iarr = (*env)->GetShortArrayElements(env,idata,&bb);
	if (iarr == NULL) {
		JNIFatalError( env, "shortToByte: getShort failed?");
		return NULL;
	}

	ilen = (*env)->GetArrayLength(env,idata);
	if ((start < 0) || (((start + len)) > ilen)) {
		(*env)->ReleaseShortArrayElements(env,idata,iarr,JNI_ABORT);
		raiseException( env, "shortToByte: start or len is out of bounds");
		return NULL;
	}

	ip = iarr + start;

	blen = ilen * sizeof(jshort);
	rarray = (*env)->NewByteArray(env,blen);
	if (rarray == NULL) {
		(*env)->ReleaseShortArrayElements(env,idata,iarr,JNI_ABORT);
		outOfMemory( env, "shortToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		(*env)->ReleaseShortArrayElements(env,idata,iarr,JNI_ABORT);
		JNIFatalError( env, "shortToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	for (ii = 0; ii < len; ii++) {
		u.ival = *ip++;
		for (ij = 0; ij < sizeof(jshort); ij++) {
			*bap = u.bytes[ij];
			bap++;
		}
	}

	(*env)->ReleaseByteArrayElements(env,rarray,barray, 0);
	(*env)->ReleaseShortArrayElements(env,idata,iarr,JNI_ABORT);

	return rarray;

}

/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_floatToByte__II_3F 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jfloatArray idata)  /* IN: array of float */
{
	jfloat *ip;
	jfloat *iarr;
	int ilen;
	jbyteArray rarray;
	int blen;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ii;
	int ij;
	union things {
		float ival;
		char bytes[4];
	} u;

	if (idata == NULL) {
		raiseException( env, "floatToByte: idata is NULL?");
		return NULL;
	}
	iarr = (*env)->GetFloatArrayElements(env,idata,&bb);
	if (iarr == NULL) {
		JNIFatalError( env, "floatToByte: getFloat failed?");
		return NULL;
	}

	ilen = (*env)->GetArrayLength(env,idata);
	if ((start < 0) || (((start + len)) > ilen)) {
		(*env)->ReleaseFloatArrayElements(env,idata,iarr,JNI_ABORT);
		raiseException( env, "floatToByte: start or len is out of bounds");
		return NULL;
	}

	ip = iarr + start;

	blen = ilen * sizeof(jfloat);
	rarray = (*env)->NewByteArray(env,blen);
	if (rarray == NULL) {
		(*env)->ReleaseFloatArrayElements(env,idata,iarr,JNI_ABORT);
		outOfMemory( env, "floatToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		(*env)->ReleaseFloatArrayElements(env,idata,iarr,JNI_ABORT);
		JNIFatalError( env, "floatToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	for (ii = 0; ii < len; ii++) {
		u.ival = *ip++;
		for (ij = 0; ij < sizeof(jfloat); ij++) {
			*bap = u.bytes[ij];
			bap++;
		}
	}

	(*env)->ReleaseByteArrayElements(env,rarray,barray, 0);
	(*env)->ReleaseFloatArrayElements(env,idata,iarr,JNI_ABORT);

	return rarray;

}

/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_doubleToByte__II_3D 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jdoubleArray idata)  /* IN: array of double */
{
	jdouble *ip;
	jdouble *iarr;
	int ilen;
	jbyteArray rarray;
	int blen;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ii;
	int ij;
	union things {
		double ival;
		char bytes[8];
	} u;

	if (idata == NULL) {
		raiseException( env, "doubleToByte: idata is NULL?");
		return NULL;
	}
	iarr = (*env)->GetDoubleArrayElements(env,idata,&bb);
	if (iarr == NULL) {
		JNIFatalError( env, "doubleToByte: getDouble failed?");
		return NULL;
	}

	ilen = (*env)->GetArrayLength(env,idata);
	if ((start < 0) || (((start + len)) > ilen)) {
		(*env)->ReleaseDoubleArrayElements(env,idata,iarr,JNI_ABORT);
		raiseException( env, "doubleToByte: start or len is out of bounds");
		return NULL;
	}

	ip = iarr + start;

	blen = ilen * sizeof(jdouble);
	rarray = (*env)->NewByteArray(env,blen);
	if (rarray == NULL) {
		(*env)->ReleaseDoubleArrayElements(env,idata,iarr,JNI_ABORT);
		outOfMemory( env, "doubleToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		(*env)->ReleaseDoubleArrayElements(env,idata,iarr,JNI_ABORT);
		JNIFatalError( env, "doubleToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	for (ii = 0; ii < len; ii++) {
		u.ival = *ip++;
		for (ij = 0; ij < sizeof(jdouble); ij++) {
			*bap = u.bytes[ij];
			bap++;
		}
	}

	(*env)->ReleaseByteArrayElements(env,rarray,barray, 0);
	(*env)->ReleaseDoubleArrayElements(env,idata,iarr,JNI_ABORT);

	return rarray;

}


/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_longToByte__II_3J 
( JNIEnv *env,
jclass class, 
jint start,
jint len,
jlongArray idata)  /* IN: array of long */
{
	jlong *ip;
	jlong *iarr;
	int ilen;
	jbyteArray rarray;
	int blen;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ii;
	int ij;
	union things {
		jlong ival;
		char bytes[8];
	} u;

	if (idata == NULL) {
		raiseException( env, "longToByte: idata is NULL?");
		return NULL;
	}
	iarr = (*env)->GetLongArrayElements(env,idata,&bb);
	if (iarr == NULL) {
		JNIFatalError( env, "longToByte: getLong failed?");
		return NULL;
	}

	ilen = (*env)->GetArrayLength(env,idata);
	if ((start < 0) || (((start + len)) > ilen)) {
		(*env)->ReleaseLongArrayElements(env,idata,iarr,JNI_ABORT);
		raiseException( env, "longToByte: start or len is out of bounds?\n");
		return NULL;
	}

	ip = iarr + start;

	blen = ilen * sizeof(jlong);
	rarray = (*env)->NewByteArray(env,blen);
	if (rarray == NULL) {
		(*env)->ReleaseLongArrayElements(env,idata,iarr,JNI_ABORT);
		outOfMemory( env, "longToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		(*env)->ReleaseLongArrayElements(env,idata,iarr,JNI_ABORT);
		JNIFatalError( env, "longToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	for (ii = 0; ii < len; ii++) {
		u.ival = *ip++;
		for (ij = 0; ij < sizeof(jlong); ij++) {
			*bap = u.bytes[ij];
			bap++;
		}
	}

	(*env)->ReleaseByteArrayElements(env,rarray,barray, 0);
	(*env)->ReleaseLongArrayElements(env,idata,iarr,JNI_ABORT);

	return rarray;

}
 /******/


/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_intToByte__I 
( JNIEnv *env,
jclass class, 
jint idata)  /* IN: int */
{
	jbyteArray rarray;
	jbyte *barray;
	jbyte *bap;
	int ij;
	jboolean bb;
	union things {
		int ival;
		char bytes[sizeof(int)];
	} u;

	rarray = (*env)->NewByteArray(env,sizeof(jint));
	if (rarray == NULL) {
		outOfMemory( env, "intToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		JNIFatalError( env, "intToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	u.ival = idata;
	for (ij = 0; ij < sizeof(jint); ij++) {
		*bap = u.bytes[ij];
		bap++;
	}

	(*env)->ReleaseByteArrayElements(env,rarray,barray, 0);
	return rarray;

}

/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_floatToByte__F 
( JNIEnv *env,
jclass class, 
jfloat idata)  /* IN: int */
{
	jbyteArray rarray;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ij;
	union things {
		float ival;
		char bytes[sizeof(float)];
	} u;

	rarray = (*env)->NewByteArray(env,sizeof(jfloat));
	if (rarray == NULL) {
		outOfMemory( env, "floatToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		JNIFatalError( env, "floatToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	u.ival = idata;
	for (ij = 0; ij < sizeof(jfloat); ij++) {
		*bap = u.bytes[ij];
		bap++;
	}

	(*env)->ReleaseByteArrayElements(env,rarray,(jbyte *)barray, 0);
	return rarray;

}

/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_shortToByte__S 
( JNIEnv *env,
jclass class, 
jshort idata)  /* IN: short */
{
	jbyteArray rarray;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ij;
	union things {
		short ival;
		char bytes[sizeof(short)];
	} u;

	rarray = (*env)->NewByteArray(env,sizeof(jshort));
	if (rarray == NULL) {
		outOfMemory( env, "shortToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		JNIFatalError( env, "shortToByte: getShort failed?");
		return NULL;
	}

	bap = barray;
	u.ival = idata;
	for (ij = 0; ij < sizeof(jshort); ij++) {
		*bap = u.bytes[ij];
		bap++;
	}

	(*env)->ReleaseByteArrayElements(env,rarray,(jbyte *)barray, 0);

	return rarray;

}


/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_doubleToByte__D 
( JNIEnv *env,
jclass class, 
jdouble idata)  /* IN: double */
{
	jbyteArray rarray;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ij;
	union things {
		double ival;
		char bytes[sizeof(double)];
	} u;

	rarray = (*env)->NewByteArray(env,sizeof(jdouble));
	if (rarray == NULL) {
		outOfMemory( env, "doubleToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		JNIFatalError( env, "doubleToByte: getDouble failed?");
		return NULL;
	}

	bap = barray;
	u.ival = idata;
	for (ij = 0; ij < sizeof(jdouble); ij++) {
		*bap = u.bytes[ij];
		bap++;
	}

	(*env)->ReleaseByteArrayElements(env,rarray,(jbyte *)barray, 0);

	return rarray;
}


/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_longToByte__J 
( JNIEnv *env,
jclass class, 
jlong idata)  /* IN: array of long */
{
	jbyteArray rarray;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ij;
	union things {
		jlong ival;
		char bytes[sizeof(jlong)];
	} u;

	rarray = (*env)->NewByteArray(env,sizeof(jlong));
	if (rarray == NULL) {
		outOfMemory( env, "longToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		JNIFatalError( env, "longToByte: getLong failed?");
		return NULL;
	}

	bap = barray;
	u.ival = idata;
	for (ij = 0; ij < sizeof(jlong); ij++) {
		*bap = u.bytes[ij];
		bap++;
	}

	(*env)->ReleaseByteArrayElements(env,rarray,(jbyte *)barray, 0);

	return rarray;

}

/* returns byte [] */
JNIEXPORT jbyteArray JNICALL Java_ncsa_hdf_hdflib_HDFNativeData_byteToByte__B 
( JNIEnv *env,
jclass class, 
jbyte idata)  /* IN: array of long */
{
	jbyteArray rarray;
	jbyte *barray;
	jbyte *bap;
	jboolean bb;
	int ij;
	union things {
		jbyte ival;
		char bytes[sizeof(jbyte)];
	} u;

	rarray = (*env)->NewByteArray(env,sizeof(jbyte));
	if (rarray == NULL) {
		outOfMemory( env, "byteToByte" );
		return NULL;
	}

	barray = (*env)->GetByteArrayElements(env,rarray,&bb);
	if (barray == NULL) {
		JNIFatalError( env, "byteToByte: getByte failed?");
		return NULL;
	}

	bap = barray;
	u.ival = idata;
	for (ij = 0; ij < sizeof(jbyte); ij++) {
		*bap = u.bytes[ij];
		bap++;
	}

	(*env)->ReleaseByteArrayElements(env,rarray,(jbyte *)barray, 0);

	return rarray;
}
