//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2011-09-27 19:35:43 +0200 (Di, 27 Sep 2011) $
// $Rev: 1079 $
//
#include <xmmintrin.h>
#include <emmintrin.h>
#include <cmath>
#include <cstdlib>

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

