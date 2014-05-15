//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef _IMAGECAST_H
#define _IMAGECAST_H

#include "timage.h"

namespace kipl { namespace base {

template <typename T1, typename T2 , size_t N>
class ImageCaster {
public:
	static TImage<T1,N> cast(const TImage<T2,N> &img);
	static TImage<T1,N> cast(const TImage<T2,N> &img, T2 lo, T2 hi);
};


}}
#include "core/imagecast.hpp"

#endif
