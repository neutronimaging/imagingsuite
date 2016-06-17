#ifndef SEQUENCEIMAGE_H_
#define SEQUENCEIMAGE_H_

#include "../base/timage.h"

namespace kipl { namespace generators { 
/// \brief This class is generates different test images with contents as sequences of axis index of pixel index
class SequenceImage
{
private:
	SequenceImage() {}
public:
	// 2D test images

	/// \brief Creates an image with pixels assigned as a sequence with increment 1.0
	/// \param dims Array containing the image dimensions
	/// \param start Bias of the sequence
	/// \returns The generated image
	static kipl::base::TImage<float,2> MakeSequenceImage2D(size_t const * const dims, float start=0.0f);

    /// \brief Creates symmetric a modula image. The remainder of the division of the
	/// index sequence and a number M is computed
	/// \param N Side length of the image
	/// \param M Denominator for the sequence division
	static kipl::base::TImage<float,2> MakeModImage2D(size_t N, size_t M);

    /// \brief Creates an image containint the sum of the axis indices
	/// \param N Side length of the image
	static kipl::base::TImage<float,2> MakeAxisSumImage2D(size_t N);

	// 3D test images
	/// \brief Creates an image with pixels assigned as a sequence with increment 1.0
	/// \param dims Array containing the image dimensions
	/// \param start Bias of the sequence
	/// \returns The generated image
	static kipl::base::TImage<float,3> MakeSequenceImage3D(size_t const * const dims, float start=0.0f);
};

}}

#endif
