#ifndef NORMALISATION_H
#define NORMALISATION_H

#include "ImagingAlgorithms_global.h"

#include <vector>
#include <base/timage.h>

namespace ImagingAlgorithms
{

kipl::base::TImage<float,2> IMAGINGALGORITHMSSHARED_EXPORT normalize(const kipl::base::TImage<float,2> &img,
                                                                     const kipl::base::TImage<float,2> &ob,
                                                                     const kipl::base::TImage<float,2> &dc,
                                                                     const std::vector<size_t> &doseROI = {});

}


#endif // NORMALISATION_H
