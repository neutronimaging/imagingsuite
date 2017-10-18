#ifndef IO_PNG_H
#define IO_PNG_H
#include "../kipl_global.h"
#include "../base/timage.h"

namespace kipl { namespace io {

int KIPLSHARED_EXPORT GetPNGDims(std::string fname, size_t *dims);

int KIPLSHARED_EXPORT ReadPNG(std::string fname, kipl::base::TImage<float,2> &img, int idx, size_t *roi=nullptr);

int KIPLSHARED_EXPORT WritePNG(std::string fname, kipl::base::TImage<float,2> & img, float low=0.0f, float high=0.0f);

}}
#endif // IO_PNG_H
