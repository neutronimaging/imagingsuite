#ifndef IO_FITS_H_
#define IO_FITS_H_

#include "../kipl_global.h"
#include "../base/timage.h"

namespace kipl { namespace io {

/// \brief Read a 2D image from a .fits file.
/// \param src the image to store the file contents
/// \param fname File name of the file to read
/// \param nCrop optional crop region. The entire image will be read if the value is set to NULL.
/// \returns Always 0.
template <typename ImgType>
int ReadFITS(kipl::base::TImage<ImgType,2> &src,char const * const fname, size_t const * const nCrop=NULL);


/// \brief Write a 2D image to a .fits file.
/// \param src The image to write
/// \param fname file name of the destination file.
/// \returns 0
/// \todo WriteFITS is not implemented
template <typename ImgType>
int WriteFITS(kipl::base::TImage<ImgType,2> &src,char const * const fname);

/// \brief Reads the image dimensions of a fits file
/// \param filename Name of the fits file containing the image
/// \param dims Array to recieve the image dimensions.
void KIPLSHARED_EXPORT GetFITSDims(char const * const filename,size_t * dims);


}}

#include "core/io_fits.hpp"

#endif /*IO_FITS_H_*/
