//<LICENCE>

#include <cmath>
#include <cstdlib>
#ifdef __x86_64__
    #include <xmmintrin.h>
    #include <emmintrin.h>
#else
    #include <sse2neon.h>
#endif

namespace kipl { namespace math {

//inline __m128 pow_ps(__m128 a, __m128 b)
//{
//	__m128 res;
//#ifdef _MSC_VER
//	__asm {
//
//	}
//#endif
//
//	return res;
//}

//
//int pow(float *data, size_t N, float exponent)
//{
//#ifdef _MSVC_VER_NEVER
//	size_t d=reinterpret_cast<size_t>(data);
//	if (d % 4 == 0) {
//		ptrdiff_t N4=N/4;
//		__m128 e={exponent,exponent,exponent,exponent};
//		__m128 *data128=reinterpret_cast<__m128 *>(data);
//		ptrdiff_t i=0;
//
//		for (i=0; i<N4; i++) {
//			data128[i]=pow_ps(data128[i],e);
//		}
//	}
//	else {
//
//	}
//#else
//	size_t i=0;
//	#pragma omp parallel for firstprivate(exponent)
//	for (i=0; i<N; i++) {
//		data[i]=pow(data[i],exponent);
//	}
//#endif
//}

}}

