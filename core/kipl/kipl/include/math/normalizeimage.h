//<LICENSE>
#ifndef NORMALIZEIMAGE_H
#define NORMALIZEIMAGE_H

#include "../kipl_global.h"
#include <cstdlib>

namespace kipl {
namespace math {
void KIPLSHARED_EXPORT normalizeData(float *data, size_t N, float &slope, float &intercept, bool forward=true, bool autoscale=true);
}}
#endif // NORMALIZEIMAGE_H
