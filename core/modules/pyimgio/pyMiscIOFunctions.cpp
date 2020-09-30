//<LICENSE>

#include <sstream>
#include <iostream>

#include <base/kiplenums.h>
#include <analyzefileext.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindMiscFunctions(py::module &m)
{
    m.def("GetFileExtensionType", &readers::GetFileExtensionType);

    py::enum_<readers::eExtensionTypes>(m,"eExtensionTypes")
        .value("ExtensionTXT",  readers::ExtensionTXT)   ///< The image is provided as lines of space separated numbers
        .value("ExtensionDMP",  readers::ExtensionDMP) ///< A raw binary image format. Usually a binary dump file from a camera
        .value("ExtensionDAT",  readers::ExtensionDAT) ///< A raw binary image format.
        .value("ExtensionXML",  readers::ExtensionXML)    ///< Image data stored in xml format (rather unusual)
        .value("ExtensionRAW",  readers::ExtensionRAW)    ///< A raw binary image format
        .value("ExtensionFITS", readers::ExtensionFITS)   ///< Images are stored in the fits format
        .value("ExtensionTIFF", readers::ExtensionTIFF)   ///< Images are stored in the tiff format
        .value("ExtensionPNG",  readers::ExtensionPNG)    ///< Images are stored in the png format
        .value("ExtensionJPG",  readers::ExtensionJPG)    ///< Images are stored in the jpg format
        .value("ExtensionHDF",  readers::ExtensionHDF)    ///< Images are stored using a HDF format
        .value("ExtensionHDF4", readers::ExtensionHDF4)   ///< Images are stored using a HDF4 format
        .value("ExtensionHDF5", readers::ExtensionHDF5)   ///< Images are stored using a HDF5 format
        .value("ExtensionSEQ",  readers::ExtensionSEQ);    ///< Images are stored in the Varian ViVa SEQ format

    py::enum_<kipl::base::eImageFlip>(m,"eImageFlip")
        .value("ImageFlipNone",               kipl::base::ImageFlipNone)               ///< No flipping
        .value("ImageFlipHorizontal",         kipl::base::ImageFlipHorizontal)         ///< Flip about the vertical axis
        .value("ImageFlipVertical",           kipl::base::ImageFlipVertical)           ///< Flip about the horizontal axis
        .value("ImageFlipHorizontalVertical", kipl::base::ImageFlipHorizontalVertical) ///< Flip both horizontal and vertial
        .value("ImageFlipDefault",            kipl::base::ImageFlipDefault);           ///< Use default Flip operation


/// \brief Image rotation selector
    py::enum_<kipl::base::eImageRotate>(m,"eImageRotate")
        .value("ImageRotateNone",       kipl::base::ImageRotateNone)///< No rotation
        .value("ImageRotate90",         kipl::base::ImageRotate90)///< Rotate 90 degrees clockwise
        .value("ImageRotate180",        kipl::base::ImageRotate180)///< Rotate 180 degrees clockwise
        .value("ImageRotate270",        kipl::base::ImageRotate270)///< Rotate 270 degrees clockwise
        .value("ImageRotateDefault",    kipl::base::ImageRotateDefault);  ///< Use default rotation operation


}
