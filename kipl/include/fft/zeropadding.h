/*
 * zeropadding.h
 *
 *  Created on: Aug 16, 2011
 *      Author: anders
 */

#ifndef ZEROPADDING_H_
#define ZEROPADDING_H_
#include "../kipl_global.h"
#include "../base/timage.h"

namespace kipl { namespace math { namespace fft {

	enum ePaddingPosition {
		PadCorner,
		PadCenter
	};

size_t KIPLSHARED_EXPORT NextPower2(size_t N);

template <typename T>
void ZeroPad(kipl::base::TImage<T,2> &img, kipl::base::TImage<T,2> &padded, size_t const * const dims, size_t * const insert, ePaddingPosition position);

template <typename T>
void ZeroPad(kipl::base::TImage<T,2> &img, kipl::base::TImage<T,2> &padded, size_t *insert, size_t addexp, ePaddingPosition position);


template <typename T>
void RemoveZeroPad(kipl::base::TImage<T,2> &padded, kipl::base::TImage<T,2> &img, size_t *insert, size_t const * dims);

}}}

#include "core/zeropadding.hpp"

#endif /* ZEROPADDING_H_ */
