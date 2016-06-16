#ifndef __GABORFILTER_H
#define __GABORFILTER_H

#include "../logging/logger.h"

namespace kipl {
namespace filters {

/// \brief Not implemented
template <typename T>
void BuildGaborKernel(T ** kernel, size_t * dims, float sigma, float aspectratio, float frequency, float orientation);

/// \brief Not implemented
template<typename T>
class GaborAnalysis {
    kipl::logging::Logger logger;
public:
    GaborAnalysis(int orientations, int basecount ,float *sigma, float *aspect, float *frequencies);

    TImage<T,2> analyze(TImage<T,2> &img);

protected:
    void BuildFilters();


};

}
}
#endif
