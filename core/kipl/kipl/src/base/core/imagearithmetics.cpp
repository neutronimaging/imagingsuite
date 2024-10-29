//<LICENCE>

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

#include <cstddef>
#include "../../../include/base/core/imagearithmetics.h"

#ifndef __CYGWIN__
namespace kipl { namespace base { namespace core {

void SSE2Add(float *a, const float *b, const size_t N)
{
	const size_t N4=N>>2;

    __m128 *vecA = reinterpret_cast<__m128 *>(a);
    const __m128 *vecB = reinterpret_cast<const __m128 *>(b);

	size_t i;
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_add_ps(vecA[i],vecB[i]);
	}
	for (i=N4<<2; i<N; i++) {
		a[i]+=b[i];
	}
}

void SSE2Add(float *a, const float b, const size_t N)
{
	const size_t N4=N>>2;

    __m128 *vecA = reinterpret_cast<__m128 *>(a);
	__m128 B = _mm_load_ps1(&b);

	size_t i;
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_add_ps(vecA[i],B);
	}
	for (i=N4<<2; i<N; i++) {
		a[i]+=b;
	}
}

void SSE2Minus(float *a, const float *b, const size_t N)
{
	const size_t N4=N>>2;

    __m128 *vecA = reinterpret_cast<__m128 *>(a);
    const __m128 *vecB = reinterpret_cast<const __m128 *>(b);

	size_t i;
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_sub_ps(vecA[i],vecB[i]);
	}
	for (i=N4<<2; i<N; i++) {
		a[i]-=b[i];
	}
}

void SSE2Minus(float *a, const float b, const size_t N)
{
	const size_t N4=N>>2;

    __m128 *vecA = reinterpret_cast<__m128 *>(a);
	__m128 B = _mm_load_ps1(&b);

	size_t i;
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_sub_ps(vecA[i],B);

	}
	for (i=N4<<2; i<N; i++) {
		a[i]-=b;
	}
}

void SSE2Mult(float *a, const float *b, const size_t N)
{
	const size_t N4=N>>2;

    __m128 *vecA = reinterpret_cast<__m128 *>(a);
    const __m128 *vecB = reinterpret_cast<const __m128 *>(b);

	size_t i;
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_mul_ps(vecA[i],vecB[i]);
	}

	for (i=N4<<2; i<N; i++) {
		a[i]*=b[i];
	}
}

void SSE2Mult(float *a, const float b, const size_t N)
{
	const size_t N4=N>>2;

    __m128 *vecA = reinterpret_cast<__m128 *>(a);
	__m128 B = _mm_load_ps1(&b);

	size_t i;
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_mul_ps(vecA[i],B);
	}

	for (i=N4<<2; i<N; i++) {
		a[i]*=b;
	}
}

void SSE2Div(float *a, const float *b, const size_t N)
{
	const ptrdiff_t N4=N>>2;

	__m128 *vecA = reinterpret_cast<__m128 *>(a);
    const __m128 *vecB = reinterpret_cast<const __m128 *>(b);

	ptrdiff_t i;
	#pragma omp parallel for
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_div_ps(vecA[i],vecB[i]);
	}

    for (ptrdiff_t j=N4<<2; j<static_cast<ptrdiff_t>(N); j++) {
		a[j]/=b[j];
	}
}

void SSE2Div(float *a, const float b, const size_t N)
{
	const size_t N4=N>>2;

    __m128 *vecA = reinterpret_cast<__m128 *>(a);
	__m128 B = _mm_load_ps1(&b);

	size_t i;
	for (i=0; i<N4; i++) {
		vecA[i]=_mm_div_ps(vecA[i],B);

	}

	for (i=N4<<2; i<N; i++) {
		a[i]/=b;
	}
}

}}}
#endif
