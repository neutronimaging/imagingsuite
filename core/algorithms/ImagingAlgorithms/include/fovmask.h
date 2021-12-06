#ifndef FOVMASK_H
#define FOVMASK_H

#include "ImagingAlgorithms_global.h"
#include <string>
#include <vector>
#include <deque>

#include <base/timage.h>

namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT FOVMask
{

public:
    FOVMask();
    void setMask(kipl::base::TImage<short,2> &img);

    kipl::base::TImage<float,2> process(kipl::base::TImage<float,2> & img);

protected:
    kipl::base::TImage<short,2> maskImg;
    std::deque<size_t> intialEdge;
};
}

#endif // FOVMASK_H
