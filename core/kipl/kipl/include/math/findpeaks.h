//<LICENSE>

#ifndef FINDPEAKS_H
#define FINDPEAKS_H

#include "../kipl_global.h"
#include <list>

namespace kipl {
namespace math {

size_t KIPLSHARED_EXPORT findPeaks(float *data, size_t N, float m, float level, std::list<size_t> &peaks);
float KIPLSHARED_EXPORT findPeakCOG(float *data, size_t N, bool useUnBias=false, bool useNegative=true);
}

}
#endif // FINDPEAKS_H
