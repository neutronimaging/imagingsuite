/*
 * trotate.h
 *
 *  Created on: Nov 30, 2011
 *      Author: kaestner
 */

#ifndef TROTATE_H_
#define TROTATE_H_

#include "timage.h"
#include "kiplenums.h"
#include "../math/mathconstants.h"

namespace kipl{ namespace base {

/// \brief Rotation and flipping class for 2D images
/// \note This is almost the same as some functions in tpermute image - merge? This class is under development
/// \test The class has a Unit test in tst_kiplbase.cpp, currently on orthogonal rotations tested.
template <typename T>
class TRotate {
public :
    /// \brief basic C'tor
    /// \param blackvalue the intensity for unsuported pixels when the image is rotated arbitrarily
	TRotate(T blackvalue=0) : m_BlackValue(blackvalue) {}

    /// \brief Rotates the image by an arbitrary angle
    /// \param img the image to rotate
    /// \param angle rotation angle in degrees. Clockwise rotation. This function is still under construction.
    /// \returns The rotated image. The size is maintained.
    kipl::base::TImage<T,2> & Rotate( kipl::base::TImage<T,2>  &img, double angle);

    /// \brief Rotates an image by 90 degrees clockwise
    /// \param img the image to rotate
    /// \returns the rotated image
    kipl::base::TImage<T,2> & Rotate90  (kipl::base::TImage<T,2> &img);

    /// \brief Rotates an image by 180 degrees clockwise
    /// \param img the image to rotate
    /// \returns the rotated image
    kipl::base::TImage<T,2> & Rotate180 (kipl::base::TImage<T,2> &img);

    /// \brief Rotates an image by 270 degrees clockwise
    /// \param img the image to rotate
    /// \returns the rotated image
    kipl::base::TImage<T,2> & Rotate270 (kipl::base::TImage<T,2> &img);

    /// \brief Mirrors an image about the vertical axis. The horizon will be mirrored.
    /// \param img the image to mirrored
    /// \returns the mirrored image
    kipl::base::TImage<T,2> & MirrorHorizontal(kipl::base::TImage<T,2> &img);

    /// \brief Mirrors an image about the horizontal axis. Sky turns to ground.
    /// \param img the image to mirrored
    /// \returns the mirrored image
    kipl::base::TImage<T,2> & MirrorVertical  (kipl::base::TImage<T,2> &img);

    kipl::base::TImage<float,2> Rotate(kipl::base::TImage<float,2> & img,
            kipl::base::eImageFlip flip,
            kipl::base::eImageRotate rotate);
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
