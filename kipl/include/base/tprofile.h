#ifndef TPROFILE_H
#define TPROFILE_H

namespace kipl { namespace base {

template <typename T, typename S>
void verticalprofile2D(const T *pData, const size_t *dims, S *pProfile, bool bMeanProfile=false);

template <typename T, typename S>
void horizontalprofile2D(const T *pData, const size_t *dims, S *pProfile, bool bMeanProfile=false);
}}

#include "core/tprofile.hpp"
#endif // TPROFILE_H
