#ifndef SINOGRAM_H
#define SINOGRAM_H
#include "ImagingAlgorithms_global.h"
#include <base/timage.h>

namespace ImagingAlgorithms 
{
int IMAGINGALGORITHMSSHARED_EXPORT ExtractSinogram( kipl::base::TImage<float,3> &projections, 
                                                    kipl::base::TImage<float,2> &sinogram, 
                                                    size_t idx);

int IMAGINGALGORITHMSSHARED_EXPORT InsertSinogram(  kipl::base::TImage<float,2> &sinogram, 
                                                    kipl::base::TImage<float,3> &projections,
                                                    size_t idx);

}
#endif