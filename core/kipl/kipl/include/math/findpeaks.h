//<LICENSE>

#ifndef FINDPEAKS_H
#define FINDPEAKS_H

#include <list>

namespace kipl {
namespace math {

size_t findPeaks(float *data, size_t N, float m, float level, std::list<size_t> &peaks);
}

}
#endif // FINDPEAKS_H
