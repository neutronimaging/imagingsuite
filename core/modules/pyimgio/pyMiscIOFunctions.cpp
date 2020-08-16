//<LICENSE>

#include <sstream>
#include <iostream>

#include <analyzefileext.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindMiscIOFunctions(py::module &m)
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
}
