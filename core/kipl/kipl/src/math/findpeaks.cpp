//<LICENSE>

#include "../../include/math/findpeaks.h"
#include <cmath>
#include "../../include/math/statistics.h"

namespace kipl {
namespace math {

size_t findPeaks(float *data, size_t N, float m, float level, std::list<size_t> &peaks)
{
    peaks.clear();

    size_t start=0L;
    size_t stop=0L;
    bool current=false;
    bool previous=false;
    previous = level < fabs(data[0]-m) ;

    for (size_t i=1; i<N; ++i) {
        current = level< (data[i]-m) ;

        if (current != previous) {
            if (current==true) {
                start=i;
                stop=i;
            }
            else {
                size_t pos = start+(stop-start)/2;
                peaks.push_back(pos);
            }
        }
        else {
            stop++;
        }
        previous=current;
    }

    if ((current==previous) && (current==true)) {
        size_t pos = (stop-start)/2;
        peaks.push_back(pos);
    }

    return peaks.size();
}

float findPeakCOG(float *data, size_t N, bool useUnBias, bool useNegative)
{

    float bias = 0.0f;
    if (useUnBias) {
        kipl::math::Statistics stat;
        stat.put(data,N);
        bias=static_cast<float>(stat.E()+stat.s());
    }

    float cog=0.0f;
    float valsum=0.0f;
    for (size_t i=0; i<N; ++i) {
        float val=data[i]-bias;
        if (!useNegative)
            val=val < 0.0f ? 0.0f : val;
        valsum+=val;
        cog += static_cast<float>(i+1)*val;
    }

    cog/=valsum;

    return cog-1;
}

}

}
