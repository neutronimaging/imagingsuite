#ifndef SINOGRAMS_H
#define SINOGRAMS_H
#include "ImagingAlgorithms_global.h"
#include <base/timage.h>

namespace ImagingAlgorithms {

int IMAGINGALGORITHMSSHARED_EXPORT ExtractSinogram( const kipl::base::TImage<float,3> &projections, 
                                                          kipl::base::TImage<float,2> &sinogram, 
                                                    const size_t idx);

int IMAGINGALGORITHMSSHARED_EXPORT InsertSinogram(  const kipl::base::TImage<float,2> &sinogram, 
                                                          kipl::base::TImage<float,3> &projections,
                                                    const size_t idx);

}

#endif