#ifndef DATAVALIDATOR_H
#define DATAVALIDATOR_H
#include <cstddef>
#include <cmath>

namespace kipl { namespace algorithms {
template <typename T>
size_t dataValidator(T *data,size_t N, size_t &cntNaN, size_t &cntInf)
{
    cntInf = 0UL;
    cntNaN = 0UL;
    for (size_t i=0; i<N; ++i)
    {
           cntInf+=std::isinf(data[i]);
           cntNaN+=std::isnan(data[i]);
    }

    return cntInf+cntNaN;
}

}

}
#endif // DATAVALIDATOR_H
