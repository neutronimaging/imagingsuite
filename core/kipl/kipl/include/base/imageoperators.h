//<LICENCE>

#ifndef IMAGEOPERATORS_H_
#define IMAGEOPERATORS_H_

#include "timage.h"
// todo Check redundancy of these functions in math

namespace kipl { namespace base {

/// \brief Finds the min value in the image
/// \param img the image to search in
/// \return the smallest found value in the image
template <typename T, size_t NDims>
T min(const kipl::base::TImage<T,NDims> &img)
{
    auto minpos = std::min_element(img.GetDataPtr(),img.GetDataPtr()+img.Size());

    return *minpos;
}

/// \brief Finds the max value in the image
/// \param img the image to search in
/// \return the greatest found value in the image
template <typename T, size_t NDims>
T max(const kipl::base::TImage<T,NDims> &img)
{
    auto maxpos = std::max_element(img.GetDataPtr(),img.GetDataPtr()+img.Size());

    return *maxpos;
}

/// \brief Computes the sum of the pixel values in the image
/// \param img the image to sum up
/// \return the sum of the pixels
template <typename T, size_t NDims>
double sum(const kipl::base::TImage<T,NDims> &img)
{
	const size_t N=img.Size();
	T const * const pImg=img.GetDataPtr();
	double sum=static_cast<double>(pImg[0]);
	for (size_t i=1; i<N; i++) {
		sum+=static_cast<double>(pImg[i]);
	}
	
	return static_cast<T>(sum);
}


/// \brief Computes the average of the pixel values in the image
/// \param img the image to sum up
/// \return the average intensity of the image
template <typename T, size_t NDims>
double mean(const kipl::base::TImage<T,NDims> &img)
{	
	return sum(img)/img.Size();
}

/// \brief Counts the number of NaN pixels in the image
/// \param img the image to check
/// \return the number of NaN pixels
template <typename T, size_t NDims>
size_t count_NaN(const kipl::base::TImage<T,NDims> &img)
{
	T const * const pData=img.GetDataPtr();
	size_t cnt=0L;

	for (size_t i=0L; i<img.Size(); i++) {
        cnt+=std::isnan(pData[i]);
	}
	
	return cnt;
}

/// \brief Counts the number of inf pixels in the image
/// \param img the image to check
/// \return the number of inf pixels
template <typename T, size_t NDims>
size_t count_Inf(const kipl::base::TImage<T,NDims> img)
{
	T const * const pData=img.GetDataPtr();
	size_t cnt=0L;
	
	for (size_t i=0L; i<img.Size(); i++) {
        cnt+=std::isinf(pData[i]);
	}
	
	return cnt;
}



}}
#endif /*IMAGEOPERATORS_H_*/
