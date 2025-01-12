//<LICENCE>

#ifndef IO_FITS_H_
#define IO_FITS_H_

#include "../kipl_global.h"

#include "../base/timage.h"
#include <stdint.h>

namespace kipl { namespace io {

/// \brief Read a 2D image from a .fits file.
/// \param src the image to store the file contents
/// \param fname File name of the file to read
/// \param nCrop optional crop region. The entire image will be read if the value is set to nullptr.
/// \returns Always 0.
template <typename ImgType>
int ReadFITS(kipl::base::TImage<ImgType,2> &src,const std::string & fname, const std::vector<size_t> & nCrop={}, size_t idx=0L);


/// \brief Write a 2D image to a .fits file.
/// \param src The image to write
/// \param fname file name of the destination file.
/// \returns 0
template <typename ImgType>
int WriteFITS(kipl::base::TImage<ImgType,2> &src,char const * const fname);

/// \brief Reads the image dimensions of a fits file
/// \param filename Name of the fits file containing the image
/// \param dims Array to recieve the image dimensions.
/// \returns number of dimensions
std::vector<size_t> KIPLSHARED_EXPORT GetFITSDims(const std::string & filename);


}}

#include "core/io_fits.hpp"

#endif /*IO_FITS_H_*/
