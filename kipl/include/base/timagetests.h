/*
 * timagetests.h
 *
 *  Created on: Mar 13, 2011
 *      Author: anders
 */

#ifndef TIMAGETESTS_H_
#define TIMAGETESTS_H_

#include "timage.h"

namespace kipl { namespace base {

/// \brief Checks if an image has NaN valued pixels
/// \param img
/// \returns true on the occurance of a NaN otherwise false
template <typename T, size_t N>
bool hasNan(TImage<T,N> &img)
{
	T * pImg=img.GetDataPtr();
	size_t i;
	for (i=0; i<img.Size(); i++) {
		if (pImg[i]!=pImg[i])
			break;
	}

	return i!=img.Size();
}

}}


#endif /* TIMAGETESTS_H_ */
