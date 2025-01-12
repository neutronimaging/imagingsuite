//<LICENCE>

#ifndef QUAD_H_
#define QUAD_H_

#ifdef __aarch64__
    #include <sse2neon/sse2neon.h>
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
