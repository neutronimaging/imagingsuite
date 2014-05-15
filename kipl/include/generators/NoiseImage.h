#ifndef __NOISEIMAGE_H
#define __NOISEIMAGE_H

#include "../base/timage.h"
#include "../math/GaussianNoise.h"

namespace kipl { namespace generators {

void AddTexturedNoise(kipl::base::TImage<float,2> &img, float variance, size_t width);
void AddTexturedNoise(kipl::base::TImage<float,3> *img, float variance, size_t width);

}}


#endif
