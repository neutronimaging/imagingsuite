#ifndef CLASSGROWING_H_
#define CLASSGROWING_H_

#include "../base/timage.h"


namespace akipl { namespace segmentation {
	
template <class T, size_t N>
int ClassGrow(kipl::base::TImage<T,N> &img, const size_t nClasses, const T voidvalue);
	
}}

#include "core/ClassGrowing.hpp"
#endif /*CLASSGROWING_H_*/
