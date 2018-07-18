//<LICENSE>

#include "../../include/math/findpeaks.h"
#include <cmath>

#include <QDebug>

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

}

}
