//<LICENSE>
#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include "readerconfig_global.h"

#include <logging/logger.h>
#include <base/timage.h>
#include <base/kiplenums.h>
#include <profile/Timer.h>
#include <interactors/interactionbase.h>

#include "datasetbase.h"

class READERCONFIGSHARED_EXPORT ImageReader
{
    kipl::logging::Logger logger; ///< The logger for messages from the reader

public:
    /// Constructor to initialize the reader
    ImageReader(kipl::interactors::InteractionBase *interactor=nullptr);

    /// Cleaning up and closing files.
    ~ImageReader(void);

    /// Reading a single file with explicitly defined file name
    /// \param filename The name of the file to read.
    /// \param flip Should the image be flipped horizontally or vertically.
    /// \param rotate Should the file be rotated, steps of 90deg.
    /// \param binning Binning factor.
    /// \param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.
    /// \returns The 2D image stored in the specified file.
    kipl::base::TImage<float,2> Read(std::string filename,
            kipl::base::eImageFlip flip=kipl::base::ImageFlipNone,
            kipl::base::eImageRotate rotate=kipl::base::ImageRotateNone,
            float binning=1.0f,
            size_t const * const nCrop=nullptr, size_t idx=0L);

    /// Reading a single file with file name given by a file mask and an index number.
    /// \param path Path to the location where the file is saved
    /// \param filemask The mask of the file to read, # are used as place holders for the number.
    /// \param number The file index number.
    /// \param flip Should the image be flipped horizontally or vertically.
    /// \param rotate Should the file be rotated, steps of 90deg.
    /// \param binning Binning factor.
    /// \param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.
    /// \returns The 2D image stored in the specified file.
    kipl::base::TImage<float,2> Read(std::string path,
            std::string filemask,
            size_t number,
            kipl::base::eImageFlip flip,
            kipl::base::eImageRotate rotate,
            float binning,
            size_t const * const nCrop);

    /// Reading a block of images into a volume with file name given by a file mask and an index number.
    /// \param path Path to the location where the file is saved
    /// \param filemask The mask of the file to read, # are used as place holders for the number.
    /// \param number The file index number.
    /// \param flip Should the image be flipped horizontally or vertically.
    /// \param rotate Should the file be rotated, steps of 90deg.
    /// \param binning Binning factor.
    /// \param nCrop ROI for cropping the image. If nullptr is provided the whole image will be read.
    /// \returns The 2D image stored in the specified file.
    kipl::base::TImage<float,3> Read(std::string fname,
                                      size_t first,
                                      size_t last,
                                      size_t step,
                                      kipl::base::eImageFlip flip,
                                      kipl::base::eImageRotate rotate,
                                      float binning,
                                      size_t const * const nCrop);

    kipl::base::TImage<float,3> Read(FileSet &loader,
                                      kipl::base::eImageFlip flip,
                                      kipl::base::eImageRotate rotate,
                                      float binning=1.0f,
                                      size_t const * const nCrop=nullptr);

    kipl::base::TImage<float,3> Read(FileSet &loader);

    /// Get the image dimensions for an image file using a file mask
    /// \param path The path where image is stored
    /// \param filemask Mask of the images, # are used as placeholders for the index numbers.
    /// \param number The index number of the image to read.
    /// \param binning Binning factor
    /// \param dims A preallocated array to store the image dimensions.
    /// \returns number of dimensions
    int GetImageSize(std::string path, std::string filemask, size_t number, float binning, size_t *dims);

    /// Get the image dimensions for an image file using an explicit file name
    /// \param filename File name of the of the image to read.
    /// \param binning Binning factor
    /// \param dims A preallocated array to store the image dimensions.
    /// \returns number of dimensions
    int GetImageSize(std::string filename, float binning, size_t * dims);

    /// Get the projection dose for an image file using an explicit file name
    /// \param filename File name of the of the image to read.
    /// \param flip Flips the image about the horizontal or vertical axis.
    /// \param rotate Rotates the image in steps of 90 deg
    /// \param binning Binning factor
    /// \param doseroi The area were the dose is to be measured (x0,y0,x1,y1).
    /// \returns The dose value as the median of the row average intensity.
    float GetProjectionDose(std::string filename,kipl::base::eImageFlip flip, kipl::base::eImageRotate rotate, float binning, size_t const * const nDoseROI);

    /// Get the projection dose for an image file using an explicit file name
    /// \param path The path where image is stored
    /// \param filemask Mask of the images, # are used as placeholders for the index numbers.
    /// \param number The index number of the image to read.
    /// \param flip Flips the image about the horizontal or vertical axis.
    /// \param rotate Rotates the image in steps of 90 deg
    /// \param binning Binning factor
    /// \param doseroi The area were the dose is to be measured (x0,y0,x1,y1).
    /// \returns The dose value as the median of the row average intensity.
    float GetProjectionDose(std::string path, std::string filemask, size_t number, kipl::base::eImageFlip flip, kipl::base::eImageRotate rotate, float binning ,size_t const * const nCrop);

    /// Initializes the reading timer
    void Initialize() {timer.reset();}

    /// \returns The elapsed reading time.
    double ExecTime() {return timer.elapsedTime(kipl::profile::Timer::seconds);}
protected:
    /// Recomputes the crop ROI based on flipping and rotation into real image coordinates.
    /// \param flip How should the image be flipped.
    /// \param rotate How should the image be rotated.
    /// \param dims Array with the image dimensions.
    /// \param nCrop ROI for the cropping (x0,y0,x1,y1).
    void UpdateCrop(kipl::base::eImageFlip flip,
            kipl::base::eImageRotate rotate, size_t *dims, size_t *nCrop);


    /// Performs the rotation and flipping of the image
    /// \param img The image to transform
    /// \param flip How should the image be flipped.
    /// \param rotate How should the image be rotated.
    /// \returns The transformed image
    kipl::base::TImage<float,2> RotateProjection(kipl::base::TImage<float,2> img,
            kipl::base::eImageFlip flip,
            kipl::base::eImageRotate rotate);

    /// Read old matlab files. The newer hdf based are not supported.
    /// \param filename The name of the file to read.
    /// \param nCrop ROI to read.
    /// \returns A floating point image
    kipl::base::TImage<float,2> ReadMAT(std::string filename,  size_t const * const nCrop=nullptr);

    /// Read FITS images.
    /// \param filename The name of the file to read.
    /// \param nCrop ROI to read.
    /// \returns A floating point image
    kipl::base::TImage<float,2> ReadFITS(std::string filename, size_t const * const nCrop=nullptr, size_t idx=0L);

    /// Read TIFF images
    /// \param filename The name of the file to read.
    /// \param nCrop ROI to read.
    /// \returns A floating point image
    kipl::base::TImage<float,2> ReadTIFF(std::string filename, size_t const * const nCrop=nullptr, size_t idx=0L);

    /// Read PNG images
    /// \param filename The name of the file to read.
    /// \param nCrop ROI to read.
    /// \returns A floating point image
    /// \todo Implement PNG support
    kipl::base::TImage<float,2> ReadPNG(std::string filename,  size_t const * const nCrop=nullptr, size_t idx=0L);

    kipl::base::TImage<float,2> ReadHDF(std::string filename, size_t const * const nCrop=nullptr, size_t idx=0L);

    kipl::base::TImage<float,2> ReadSEQ(std::string filename, size_t const * const nCrop=nullptr, size_t idx=0L);

    /// Interface to the interactor that updates the message and progress.
    /// \param val Progress, a value between 0.0 and 1.0.
    /// \param msg String contating the update message.
    /// \returns True if the there was a user abort request.
    bool UpdateStatus(float val, std::string msg);

    /// \returns True if there was a user abort request.
    bool Aborted();

    kipl::profile::Timer timer; ///< Timer to measure the execution time for the reading.
    kipl::interactors::InteractionBase *m_Interactor;
};

#endif
 // IMAGEREADER_H
