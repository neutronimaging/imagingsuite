#ifndef IO_GENERIC_H
#define IO_GENERIC_H
#include "../kipl_global.h"
#include "../base/timage.h"
#include "../base/kiplenums.h"
#include <list>

namespace kipl { namespace io {

template <typename ImgType>
int ReadGeneric(kipl::base::TImage<ImgType,2> &img,
                char const * const fname,
                size_t size_x,
                size_t size_y,
                size_t offset,
                size_t stride,
                size_t imagesperfile,
                kipl::base::eDataType dt,
                kipl::base::eEndians endian,
                size_t imageindex,
                size_t const * const nCrop=NULL);

template <typename ImgType>
int ReadGeneric(std::list<kipl::base::TImage<ImgType,2> > &imglist,
                char const * const fname,
                size_t size_x,
                size_t size_y,
                size_t offset,
                size_t stride,
                size_t imagesperfile,
                kipl::base::eDataType dt,
                kipl::base::eEndians endian,
                size_t const * const nCrop=NULL);

}}

#include "core/io_generic.hpp"
#endif // IO_GENERIC_H

