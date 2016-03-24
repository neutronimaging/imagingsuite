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

#ifndef TSUBIMAGE_H_
#define TSUBIMAGE_H_

#include "timage.h"

namespace kipl { namespace base {

template <typename T, size_t NDims>
class TSubImage {
public:
	static TImage<T,NDims> Get(TImage<T,NDims> const src, size_t const *const nStart, size_t const *const nLength);
    static TImage<T,NDims> Get(TImage<T,NDims> const src, size_t const * const roi);

	static void Put(const TImage<T,NDims> src, TImage<T, NDims> dest, size_t const *const nStart);
};

template <typename T>
TImage<T,2> ExtractSlice(TImage<T,3> volume, size_t idx);

template <typename T>
void InsertSlice(TImage<T,3> volume, TImage<T,2> slice, size_t idx);
}}

#include "core/tsubimage.hpp"

#endif /*TEXTRACTOR_H_*/
