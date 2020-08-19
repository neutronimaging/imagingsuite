//<LICENSE>

#include <sstream>
#include <iostream>

#include <imagereader.h>
#include <fileset.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindImageReader(py::module &m)
{
    py::class_<ImageReader> irClass(m, "ImageReader");

    irClass.def(py::init<>());
    /// Constructor to initialize the reader
//    ImageReader(kipl::interactors::InteractionBase *interactor=nullptr);


    // /// Reading a single file with explicitly defined file name
    // /// \param filename The name of the file to read.
    // /// \param flip Should the image be flipped horizontally or vertically.
    // /// \param rotate Should the file be rotated, steps of 90deg.
    // /// \param binning Binning factor.
    // /// \param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.
    // /// \returns The 2D image stored in the specified file.
    // kipl::base::TImage<float,2> Read(std::string filename,
    //         kipl::base::eImageFlip flip=kipl::base::ImageFlipNone,
    //         kipl::base::eImageRotate rotate=kipl::base::ImageRotateNone,
    //         float binning=1.0f,
    //         const std::vector<size_t> &nCrop={}, size_t idx=0L);
    // irClass.def("read", py::overload_cast<std::string, kipl::base::eImageFlip, kipl::base::eImageRotate, float, const std::vector<size_t> &, size_t>(&ImageReader::Read),
    //             "Reading a single file with explicitly defined file name\n param filename The name of the file to read.\n param flip Should the image be flipped horizontally or vertically.\n param rotate Should the file be rotated, steps of 90deg. \n param binning Binning factor. \n param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.\n returns The 2D image stored in the specified file.",
    //             py::arg("filename"),
    //             py::arg("flip")    = kipl::base::ImageFlipNone ,
    //             py::arg("rotate")  = kipl::base::ImageRotateNone,
    //             py::arg("binning") = 1.0f,
    //             py::arg("nCrop")   = std::vector<size_t>(0),
    //             py::arg("idx")     = 0L);

    irClass.def("read", [](ImageReader &reader, std::string filename, 
                                                kipl::base::eImageFlip flip, 
                                                kipl::base::eImageRotate rotate, 
                                                float binning, 
                                                const std::vector<size_t> &roi, 
                                                size_t idx)
    {
        auto img=reader.Read(filename,flip,rotate,binning,roi,idx);

        py::array_t<float> pyimg = py::array_t<float>(img.Size());
        pyimg.resize({img.Size(1),img.Size(0)});

        std::copy_n(img.GetDataPtr(),img.Size(), static_cast<float *>(pyimg.request().ptr));
        return pyimg;
    },
                "Reading a single file with explicitly defined file name\n param filename The name of the file to read.\n param flip Should the image be flipped horizontally or vertically.\n param rotate Should the file be rotated, steps of 90deg. \n param binning Binning factor. \n param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.\n returns The 2D image stored in the specified file.",
                py::arg("filename"),
                py::arg("flip")    = kipl::base::ImageFlipNone ,
                py::arg("rotate")  = kipl::base::ImageRotateNone,
                py::arg("binning") = 1.0f,
                py::arg("nCrop")   = std::vector<size_t>(0),
                py::arg("idx")     = 0L);     
    // /// Reading a single file with file name given by a file mask and an index number.
    // /// \param path Path to the location where the file is saved
    // /// \param filemask The mask of the file to read, # are used as place holders for the number.
    // /// \param number The file index number.
    // /// \param flip Should the image be flipped horizontally or vertically.
    // /// \param rotate Should the file be rotated, steps of 90deg.
    // /// \param binning Binning factor.
    // /// \param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.
    // /// \returns The 2D image stored in the specified file.
    // kipl::base::TImage<float,2> Read(std::string path,
    //         std::string filemask,
    //         size_t number,
    //         kipl::base::eImageFlip flip,
    //         kipl::base::eImageRotate rotate,
    //         float binning,
    //         const std::vector<size_t> &nCrop);

    irClass.def("read", py::overload_cast<std::string,std::string, size_t, kipl::base::eImageFlip, kipl::base::eImageRotate, float, const std::vector<size_t> & >(&ImageReader::Read),
                "Reading a single file with explicitly defined file name\n param filename The name of the file to read.\n param flip Should the image be flipped horizontally or vertically.\n param rotate Should the file be rotated, steps of 90deg. \n param binning Binning factor. \n param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.\n returns The 2D image stored in the specified file.",
                py::arg("path"),
                py::arg("filemask"),
                py::arg("number"), 
                py::arg("flip")    = kipl::base::ImageFlipNone ,
                py::arg("rotate")  = kipl::base::ImageRotateNone,
                py::arg("binning") = 1.0f,
                py::arg("nCrop")   = std::vector<size_t>(0));

    // /// Reading a block of images into a volume with file name given by a file mask and an index number.
    // /// \param path Path to the location where the file is saved
    // /// \param filemask The mask of the file to read, # are used as place holders for the number.
    // /// \param number The file index number.
    // /// \param flip Should the image be flipped horizontally or vertically.
    // /// \param rotate Should the file be rotated, steps of 90deg.
    // /// \param binning Binning factor.
    // /// \param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.
    // /// \returns The 2D image stored in the specified file.
    // kipl::base::TImage<float,3> Read(std::string fname,
    //                                   size_t first,
    //                                   size_t last,
    //                                   size_t step,
    //                                   kipl::base::eImageFlip flip,
    //                                   kipl::base::eImageRotate rotate,
    //                                   float binning,
    //                                   const std::vector<size_t> & nCrop);

    irClass.def("read", py::overload_cast<std::string, size_t,size_t, size_t, kipl::base::eImageFlip, kipl::base::eImageRotate, float, const std::vector<size_t> & >(&ImageReader::Read),
            "Reading a single file with explicitly defined file name\n param filename The name of the file to read.\n param flip Should the image be flipped horizontally or vertically.\n param rotate Should the file be rotated, steps of 90deg. \n param binning Binning factor. \n param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.\n returns The 2D image stored in the specified file.",
            py::arg("filemask"),
            py::arg("first"),
            py::arg("last"),
            py::arg("step") = 1UL, 
            py::arg("flip")    = kipl::base::ImageFlipNone ,
            py::arg("rotate")  = kipl::base::ImageRotateNone,
            py::arg("binning") = 1.0f,
            py::arg("nCrop")   = std::vector<size_t>(0));

    // kipl::base::TImage<float,3> Read(FileSet &loader,
    //                                   kipl::base::eImageFlip flip,
    //                                   kipl::base::eImageRotate rotate,
    //                                   float binning=1.0f,
    //                                  const std::vector<size_t> & nCrop = {});
    irClass.def("read", py::overload_cast<FileSet &, kipl::base::eImageFlip, kipl::base::eImageRotate, float, const std::vector<size_t> & >(&ImageReader::Read),
        "Reading a single file with explicitly defined file name\n param filename The name of the file to read.\n param flip Should the image be flipped horizontally or vertically.\n param rotate Should the file be rotated, steps of 90deg. \n param binning Binning factor. \n param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.\n returns The 2D image stored in the specified file.",
        py::arg("loader"),
        py::arg("flip")    = kipl::base::ImageFlipNone ,
        py::arg("rotate")  = kipl::base::ImageRotateNone,
        py::arg("binning") = 1.0f,
        py::arg("nCrop")   = std::vector<size_t>(0));

    // /// Get the image dimensions for an image file using a file mask
    // /// \param path The path where image is stored
    // /// \param filemask Mask of the images, # are used as placeholders for the index numbers.
    // /// \param number The index number of the image to read.
    // /// \param binning Binning factor
    // /// \param dims A preallocated array to store the image dimensions.
    // /// \returns number of dimensions
    // std::vector<size_t> GetImageSize(std::string path, std::string filemask, size_t number, float binning);
    irClass.def("imageSize", py::overload_cast<std::string , std::string, size_t, float >(&ImageReader::GetImageSize),
        "Reading a single file with explicitly defined file name\n param filename The name of the file to read.\n param flip Should the image be flipped horizontally or vertically.\n param rotate Should the file be rotated, steps of 90deg. \n param binning Binning factor. \n param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.\n returns The 2D image stored in the specified file.",
        py::arg("path") = "",
        py::arg("filemask"),
        py::arg("number"),
        py::arg("binning") = 1.0f);

    // /// Get the image dimensions for an image file using an explicit file name
    // /// \param filename File name of the of the image to read.
    // /// \param binning Binning factor
    // /// \param dims A preallocated array to store the image dimensions.
    // /// \returns number of dimensions
    // std::vector<size_t> GetImageSize(std::string filename, float binning);
    irClass.def("imageSize", py::overload_cast<std::string, float >(&ImageReader::GetImageSize),
    "Get the image dimensions for an image file using an explicit file name \nparam filename File name of the of the image to read.\nparam binning Binning factor\nparam dims A preallocated array to store the image dimensions.\nreturns number of dimensions",
    py::arg("filemask"),
    py::arg("binning") = 1.0f);

    // /// Get the projection dose for an image file using an explicit file name
    // /// \param filename File name of the of the image to read.
    // /// \param flip Flips the image about the horizontal or vertical axis.
    // /// \param rotate Rotates the image in steps of 90 deg
    // /// \param binning Binning factor
    // /// \param doseroi The area were the dose is to be measured (x0,y0,x1,y1).
    // /// \returns The dose value as the median of the row average intensity.
    // float GetProjectionDose(std::string filename,
    //                         kipl::base::eImageFlip flip,
    //                         kipl::base::eImageRotate rotate,
    //                         float binning,
    //                         const std::vector<size_t> &nDoseROI);
    irClass.def("projectionDose", py::overload_cast<std::string, kipl::base::eImageFlip, kipl::base::eImageRotate, float, const std::vector<size_t> &  >(&ImageReader::GetProjectionDose),
        "Get the image dimensions for an image file using an explicit file name \nparam filename File name of the of the image to read.\nparam binning Binning factor\nparam dims A preallocated array to store the image dimensions.\nreturns number of dimensions",
        py::arg("filemask"),
        py::arg("flip")    = kipl::base::ImageFlipNone ,
        py::arg("rotate")  = kipl::base::ImageRotateNone,
        py::arg("binning") = 1.0f,
        py::arg("nCrop"));

    // /// Get the projection dose for an image file using an explicit file name
    // /// \param path The path where image is stored
    // /// \param filemask Mask of the images, # are used as placeholders for the index numbers.
    // /// \param number The index number of the image to read.
    // /// \param flip Flips the image about the horizontal or vertical axis.
    // /// \param rotate Rotates the image in steps of 90 deg
    // /// \param binning Binning factor
    // /// \param doseroi The area were the dose is to be measured (x0,y0,x1,y1).
    // /// \returns The dose value as the median of the row average intensity.
    // float GetProjectionDose(std::string path,
    //                         std::string filemask,
    //                         size_t number,
    //                         kipl::base::eImageFlip flip,
    //                         kipl::base::eImageRotate rotate,
    //                         float binning,
    //                         const std::vector<size_t> &nCrop);
    irClass.def("projectionDose", py::overload_cast<std::string, std::string, size_t, kipl::base::eImageFlip, kipl::base::eImageRotate, float, const std::vector<size_t> &  >(&ImageReader::GetProjectionDose),
        "Get the projection dose for an image file using an explicit file name\n" 
        "param path The path where image is stored\n" 
        "param filemask Mask of the images, # are used as placeholders for the index numbers.\n" 
        "param number The index number of the image to read.\n" 
        "param flip Flips the image about the horizontal or vertical axis.\n" 
        "param rotate Rotates the image in steps of 90 deg\n"
        "param binning Binning factor\n" 
        "param doseroi The area were the dose is to be measured (x0,y0,x1,y1).\n" 
        "returns The dose value as the median of the row average intensity.",
        py::arg("path")    = "",
        py::arg("filemask"),
        py::arg("number"),
        py::arg("flip")    = kipl::base::ImageFlipNone ,
        py::arg("rotate")  = kipl::base::ImageRotateNone,
        py::arg("binning") = 1.0f,
        py::arg("nCrop"));
}