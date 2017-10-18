//<LICENCE>

#ifndef _IMAGECAST_H
#define _IMAGECAST_H

#include "timage.h"

namespace kipl { namespace base {
/// \brief Casts the image data type of one TImage to another data type without changing dimensions
/// \param T2 data type of input image
/// \param T1 data type of casted image
/// \param N number of dimensions
template <typename T1, typename T2 , size_t N>
class ImageCaster {
public:
    /// \brief Cast image from T1 to T2 without value check
    /// \param img The image to cast
	static TImage<T1,N> cast(const TImage<T2,N> &img);

    /// \brief Cast image from T1 to T2 with rescaling into a given interval
    /// \param img The image to cast
    /// \param lo lower bound for rescaling
    /// \param hi upper bound for rescaling
	static TImage<T1,N> cast(const TImage<T2,N> &img, T2 lo, T2 hi);
};


}}
#include "core/imagecast.hpp"

#endif
