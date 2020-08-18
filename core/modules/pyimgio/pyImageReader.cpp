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

    // kipl::base::TImage<float,3> Read(FileSet &loader,
    //                                   kipl::base::eImageFlip flip,
    //                                   kipl::base::eImageRotate rotate,
    //                                   float binning=1.0f,
    //                                  const std::vector<size_t> & nCrop = {});

    // kipl::base::TImage<float,3> Read(FileSet &loader);

    // /// Get the image dimensions for an image file using a file mask
    // /// \param path The path where image is stored
    // /// \param filemask Mask of the images, # are used as placeholders for the index numbers.
    // /// \param number The index number of the image to read.
    // /// \param binning Binning factor
    // /// \param dims A preallocated array to store the image dimensions.
    // /// \returns number of dimensions
    // std::vector<size_t> GetImageSize(std::string path, std::string filemask, size_t number, float binning);

    // /// Get the image dimensions for an image file using an explicit file name
    // /// \param filename File name of the of the image to read.
    // /// \param binning Binning factor
    // /// \param dims A preallocated array to store the image dimensions.
    // /// \returns number of dimensions
    // std::vector<size_t> GetImageSize(std::string filename, float binning);

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

    // /// Initializes the reading timer
    // void Initialize() {timer.reset();}

    // /// \returns The elapsed reading time.
    // double ExecTime() {return timer.elapsedTime(kipl::profile::Timer::seconds);}

}