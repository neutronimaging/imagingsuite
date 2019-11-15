//<LICENCE>

#ifndef IMAGEARITHMETICS_H_
#define IMAGEARITHMETICS_H_

#include <iostream>

#include "../../kipl_global.h"


namespace kipl { namespace base { namespace core {
template <typename T>
void BasicAdd(T *a, const T *b, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]+=b[i];
	}
}

template <typename T>
void BasicAdd(T *a, T c, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]+=c;
	}
}

template <typename T>
void BasicMinus(T *a, const T *b, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]-=b[i];
	}
}

template <typename T>
void BasicMinus(T *a, T c, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]-=c;
	}
}

template <typename T>
void BasicMult(T *a, const T *b, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]*=b[i];
	}
}

template <typename T>
void BasicMult(T *a, T c, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]*=c;
	}
}

template <typename T>
void BasicDiv(T *a, const T *b, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]/=b[i];
	}
}

template <typename T>
void BasicDiv(T *a, T c, const size_t N)
{
	for (size_t i=0; i<N ; i++) {
		a[i]/=c;
	}
}

#ifdef __CYGWIN__
void SSE2Add(float *a, const float *b, const size_t N) {BasicAdd(a,b,N);}
void SSE2Add(float *a, const float b, const size_t N) {BasicAdd(a,b,N);}
void SSE2Minus(float *a, const float *b, const size_t N) {BasicMinus(a,b,N);}
void SSE2Minus(float *a, const float b, const size_t N) {BasicMinus(a,b,N);}
void SSE2Mult(float *a, const float *b, const size_t N) {BasicMult(a,b,N);}
void SSE2Mult(float *a, const float b, const size_t N) {BasicMult(a,b,N);}
void SSE2Div(float *a, const float *b, const size_t N) {BasicDiv(a,b,N);}
void SSE2Div(float *a, const float b, const size_t N) {BasicDiv(a,b,N);}
#else
void KIPLSHARED_EXPORT SSE2Add(float *a, const float *b, const size_t N);
void KIPLSHARED_EXPORT SSE2Add(float *a, const float b, const size_t N);
void KIPLSHARED_EXPORT SSE2Minus(float *a, const float *b, const size_t N);
void KIPLSHARED_EXPORT SSE2Minus(float *a, const float b, const size_t N);
void KIPLSHARED_EXPORT SSE2Mult(float *a, const float *b, const size_t N);
void KIPLSHARED_EXPORT SSE2Mult(float *a, const float b, const size_t N);
void KIPLSHARED_EXPORT SSE2Div(float *a, const float *b, const size_t N);
void KIPLSHARED_EXPORT SSE2Div(float *a, const float b, const size_t N);
#endif

}}}
#endif /*IMAGEARITHMETICS_H_*/
