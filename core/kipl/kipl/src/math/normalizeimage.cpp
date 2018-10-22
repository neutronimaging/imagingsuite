//<LICENSE>

#include <sstream>

#include "../../include/math/normalizeimage.h"
#include "../../include/logging/logger.h"
#include "../../include/math/image_statistics.h"

namespace kipl {
namespace math {

void normalizeData(float *data, size_t N, float &slope, float &intercept, bool forward, bool autoscale)
{
    if (forward) {
        if (autoscale) {
            std::pair<double,double> stats=kipl::math::statistics(data,N);

            intercept=stats.first;
            slope=1.0f/stats.second;
        }

        for (size_t i=0; i<N; ++i) {
            data[i]=slope*(data[i]-intercept);
        }
    }
    else {
        float invSlope=1.0f/slope;

        for (size_t i=0; i<N; ++i) {
            data[i]=invSlope*data[i]+intercept;
        }
    }

}

}}
