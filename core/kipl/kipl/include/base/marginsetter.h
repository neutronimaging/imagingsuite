#ifndef MARGINSETTER_H
#define MARGINSETTER_H

#include "timage.h"

namespace kipl {
namespace base {

template <typename T, size_t N=2>
void setMarginValue(kipl::base::TImage<T,N> &img, size_t width, T value);
}
}

#include "core/marginsetter.hpp"

#endif // MARGINSETTER_H
