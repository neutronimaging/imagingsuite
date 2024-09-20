//<LICENCE>

#ifndef QUAD_H_
#define QUAD_H_

#ifdef __aarch64__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#pragma clang diagnostic ignored "-Wcast-align"
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma clang diagnostic ignored "-W#warnings"
	#include <sse2neon/sse2neon.h>	
	#pragma clang diagnostic pop
#else
    #include <xmmintrin.h>
    #include <emmintrin.h>
#endif

namespace kipl { namespace base { 

template <typename T>
struct TQuad {
	T a;
	T b;
	T c;
	T d;
};

union uFQuad {
	TQuad<float> q;
	__m128 xmm;
};

union uIQuad {
	TQuad<int> q;
	__m128i xmm;
};

}}
#endif /*QUAD_H_*/
