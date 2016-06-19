#ifndef IO_GENERIC_H
#define IO_GENERIC_H
#include "../kipl_global.h"
#include "../base/timage.h"
#include "../base/kiplenums.h"
#include <list>
#include <fstream>

namespace kipl { namespace io {

/// \brief Prototype for reading the ANDOR NEO dmp data format
/// \param fname The file name of the image
/// \param x Size of the image in x
/// \param y Size of the image in y
/// \param idx Image index if multiple image are stored in the same file
kipl::base::TImage<unsigned short,2> KIPLSHARED_EXPORT ReadDat(std::string fname, size_t x, size_t y, size_t idx);

/// \brief Read a binary file from a stream as 2D image (unsigned short)
/// \param file An opened binary file stream containing the image
/// \param img Destination image
/// \param size_x Size of the image in x
/// \param size_y Size of the image in y
/// \param offset Number of bytes until the image data starts. Usually to skip a header.
/// \param stride Number of bytes per line. This can be different than the 2x size_x due to line padding.
/// \param imagesperfile Number of images stored in the file.
/// \param dt The data type stored in the file
/// \param endian How are the byte arranged in the file
/// \param imageindex In the case when imagesperfile is more than one, this parameter indexes a specific image
/// \param nCrop Crop region. The entire image will be read if this is set to NULL
/// \returns 0
int  KIPLSHARED_EXPORT ReadGeneric(ifstream &file,kipl::base::TImage<unsigned short,2> &img,
                size_t size_x,
                size_t size_y,
                size_t offset, //< Given in bytes
                size_t stride, //< Given in bytes
                size_t imagesperfile,
                kipl::base::eDataType dt,
                kipl::base::eEndians endian,
                size_t imageindex,
                size_t const * const nCrop=NULL);

/// \brief Read a binary file as 2D image (unsigned short)
/// \param img Destination image
/// \param fname File name of the image file
/// \param size_x Size of the image in x
/// \param size_y Size of the image in y
/// \param offset Number of bytes until the image data starts. Usually to skip a header.
/// \param stride Number of bytes per line. This can be different than the 2x size_x due to line padding.
/// \param imagesperfile Number of images stored in the file.
/// \param dt The data type stored in the file
/// \param endian How are the byte arranged in the file
/// \param imageindex In the case when imagesperfile is more than one, this parameter indexes a specific image
/// \param nCrop Crop region. The entire image will be read if this is set to NULL
/// \returns 0
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

/// \brief Read all images from binary file as a list of 2D images
/// \param img Destination image
/// \param fname File name of the image file
/// \param size_x Size of the image in x
/// \param size_y Size of the image in y
/// \param offset Number of bytes until the image data starts. Usually to skip a header.
/// \param stride Number of bytes per line. This can be different than the 2x size_x due to line padding.
/// \param imagesperfile Number of images stored in the file.
/// \param dt The data type stored in the file
/// \param endian How are the byte arranged in the file
/// \param nCrop Crop region. The entire image will be read if this is set to NULL
/// \returns 0
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

