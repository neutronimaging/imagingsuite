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

#ifndef TEXTRACTOR_H_
#define TEXTRACTOR_H_

#include "timage.h"
#include "kiplenums.h"

namespace kipl { namespace base {

template<typename T>
kipl::base::TImage<T,2> ExtractSlice(kipl::base::TImage<T,3> &img, const size_t index, const kipl::base::eImagePlanes plane, size_t *roi=NULL);

template<typename T>
void InsertSlice(kipl::base::TImage<T,2> &slice, kipl::base::TImage<T,3> &volume, const size_t index, const kipl::base::eImagePlanes plane);

}}

#include "core/textractor.hpp"
#endif /*TEXTRACTOR_H_*/
