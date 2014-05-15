/*
 * trotate.h
 *
 *  Created on: Nov 30, 2011
 *      Author: kaestner
 */

#ifndef TROTATE_H_
#define TROTATE_H_

#include "timage.h"
#include "../math/mathconstants.h"

namespace kipl{ namespace base {
template <typename T>
class TRotate {
public :
	TRotate(T blackvalue=0) : m_BlackValue(blackvalue) {}

    kipl::base::TImage<T,2> & Rotate( kipl::base::TImage<T,2>  &img, double angle);
    kipl::base::TImage<T,2> & Rotate90  (kipl::base::TImage<T,2> &img);
    kipl::base::TImage<T,2> & Rotate180 (kipl::base::TImage<T,2> &img);
    kipl::base::TImage<T,2> & Rotate270 (kipl::base::TImage<T,2> &img);

    kipl::base::TImage<T,2> & MirrorHorizontal(kipl::base::TImage<T,2> &img);
    kipl::base::TImage<T,2> & MirrorVertical  (kipl::base::TImage<T,2> &img);

protected:
	T m_BlackValue;
    kipl::base::TImage<T,2> dst;

	void HorizontalSkew (T *pSrc,
	                size_t nSrc,
	                T *pDst,
	                size_t sDst,
	                size_t uRow,
	                int iOffset,
	                double dWeight,
	                T blackvalue);

	void VerticalSkew (T *pSrc,
		                size_t nSrc,
		                T *pDst,
		                size_t sDst,
		                size_t uRow,
		                int iOffset,
		                double dWeight,
		                T blackvalue);

    kipl::base::TImage<T,2> & Rotate45 (kipl::base::TImage<T,2> &img, double angle);
};

}}

#include "core/trotate.hpp"
#endif /* TROTATE_H_ */
