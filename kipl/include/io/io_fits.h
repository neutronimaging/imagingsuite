#ifndef IO_FITS_H_
#define IO_FITS_H_

#include "../kipl_global.h"
#include "../base/timage.h"

namespace kipl { namespace io {

template <typename ImgType>
int ReadFITS(kipl::base::TImage<ImgType,2> &src,char const * const fname, size_t const * const nCrop=NULL);

template <typename ImgType>
int WriteFITS(kipl::base::TImage<ImgType,2> &src,char const * const fname);

void KIPLSHARED_EXPORT GetFITSDims(char const * const filename,size_t * dims);


}}

#include "core/io_fits.hpp"

#endif /*IO_FITS_H_*/
