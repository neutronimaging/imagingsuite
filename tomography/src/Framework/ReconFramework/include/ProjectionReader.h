//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//

#ifndef _PROJECTIONREADER_H_
#define _PROJECTIONREADER_H_

#include "ReconFramework_global.h"
//#include "stdafx.h"
#include <string>
#include <map>
#include <base/timage.h>
#include <logging/logger.h>
#include <profile/Timer.h>
#include <base/kiplenums.h>
#include "ReconConfig.h"
#include <interactors/interactionbase.h>

/// This class provides reading capabilities for the image data
class RECONFRAMEWORKSHARED_EXPORT ProjectionReader
{
    kipl::logging::Logger logger; ///< The logger for messages from the reader

public:
    /// Constructor to initialize the reader
    /// \param interactor Reference to the GUI interactor object
    ProjectionReader(kipl::interactors::InteractionBase *interactor=nullptr);

    /// Cleaning up and closing files.
	~ProjectionReader(void);

    /// Reading a single file with explicitly defined file name
    /// \param filename The name of the file to read.
    /// \param flip Should the image be flipped horizontally or vertically.
    /// \param rotate Should the file be rotated, steps of 90deg.
    /// \param binning Binning factor.
    /// \param nCrop ROI for cropping the image. If NULL is provided the whole image will be read.
    /// \returns The 2D image stored in the specified file.
	kipl::base::TImage<float,2> Read(std::string filename,
            kipl::base::eImageFlip flip = kipl::base::ImageFlipNone,
            kipl::base::eImageRotate rotate = kipl::base::ImageRotateNone,
            float binning=1.0f,
            size_t const * const nCrop=nullptr);

    /// Reading a single file with file name given by a file mask and an index number.
    /// \param path Path to the location where the file is saved
    /// \param filemask The mask of the file to read, # are used as place holders for the number.
    /// \param number The file index number.
    /// \param flip Should the image be flipped horizontally or vertically.
    /// \param rotate Should the file be rotated, steps of 90deg.
    /// \param binning Binning factor.
    /// \param nCrop ROI for cropping the image. If NULL is provided the whole image will be read.
    /// \returns The 2D image stored in the specified file.
	kipl::base::TImage<float,2> Read(std::string path,
			std::string filemask,
			size_t number,
			kipl::base::eImageFlip flip,
			kipl::base::eImageRotate rotate,
			float binning,
			size_t const * const nCrop);

    /// Reading a single file from a Nexus file
    /// \param filename The name of the file to read.
    /// \param number The file index number.
    /// \param flip Should the image be flipped horizontally or vertically.
    /// \param rotate Should the file be rotated, steps of 90deg.
    /// \param binning Binning factor.
    /// \param nCrop ROI for cropping the image. If NULL is provided the whole image will be read.
    /// \returns The 2D image stored in the specified file.
    kipl::base::TImage<float, 2> ReadNexus(std::string filename,
                                          size_t number,
                                          kipl::base::eImageFlip flip = kipl::base::ImageFlipNone,
                                          kipl::base::eImageRotate rotate = kipl::base::ImageRotateNone,
                                          float binning=1.0f,
                                          size_t const * const nCrop=nullptr);

    /// Reading a stack from a Nexus file
    /// \param filename The name of the file to read.
    /// \param start The file index number for starting position along z.
    /// \param end The file index number for ending position along z
    /// \param flip Should the image be flipped horizontally or vertically.
    /// \param rotate Should the file be rotated, steps of 90deg.
    /// \param binning Binning factor.
    /// \param nCrop ROI for cropping the image. If NULL is provided the whole image will be read.
    /// \returns The 2D image stored in the specified file.
    kipl::base::TImage<float, 3> ReadNexusStack(std::string filename,
                                          size_t start, size_t end,
                                          kipl::base::eImageFlip flip = kipl::base::ImageFlipNone,
                                          kipl::base::eImageRotate rotate = kipl::base::ImageRotateNone,
                                          float binning=1.0f,
                                          size_t const * const nCrop=nullptr);

    /// Reading tomo from a Nexus file
    /// \param filename The name of the file to read.
    kipl::base::TImage<float, 3> ReadNexusTomo(std::string filename);

    kipl::base::TImage<float,2> GetNexusSlice(kipl::base::TImage<float,3> &NexusTomo,
                                              size_t number,
                                              kipl::base::eImageFlip flip = kipl::base::ImageFlipNone,
                                              kipl::base::eImageRotate rotate = kipl::base::ImageRotateNone,
                                              float binning=1.0f,
                                              size_t const * const nCrop=nullptr);

    int GetNexusInfo(std::string filename, size_t *NofImg, double *ScanAngles);


    /// Reading a block of image files using information provided by a ReconConfig struct.
    /// \param config A reconstruction configuration struct
    /// \param nCrop ROI for cropping the image. If NULL is provided the whole image will be read.
    /// \param parameters A list of output parameters like acquisition angle, projection weight, and projection dose.
    /// \returns A 3D image containing the 2D images in the xy-plane.
	kipl::base::TImage<float,3> Read(ReconConfig config,
			size_t const * const nCrop,
			std::map<std::string,std::string> &parameters);

    /// Get the image dimensions for an image file using a file mask
    /// \param path The path where image is stored
    /// \param filemask Mask of the images, # are used as placeholders for the index numbers.
    /// \param number The index number of the image to read.
    /// \param binning Binning factor
    /// \param dims A preallocated array to store the image dimensions.
	void GetImageSize(std::string path, std::string filemask, size_t number, float binning, size_t *dims);

    /// Get the image dimensions for an image file using an explicit file name
    /// \param filename File name of the of the image to read.
    /// \param binning Binning factor
    /// \param dims A preallocated array to store the image dimensions.
	void GetImageSize(std::string filename, float binning, size_t * dims);

    /// Get the image dimensions for a Nexus image file using an explicit file name
    /// \param filename File name of the of the image to read.
    /// \param binning Binning factor
    /// \param dims A preallocated array to store the image dimensions.
    void GetImageSizeNexus(std::string filename, float binning, size_t * dims);

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


    float GetProjectionDoseNexus(std::string filename, size_t number, kipl::base::eImageFlip flip, kipl::base::eImageRotate rotate, float binning ,size_t const * const nDoseROI);

    float * GetProjectionDoseListNexus(string filename, size_t start, size_t end,
                                                         kipl::base::eImageFlip flip,
                                                         kipl::base::eImageRotate rotate,
                                                         float binning,
                                                         size_t const * const nDoseROI);
    /// Initializes the reading timer
	void Initialize() {timer.Reset();}

    /// \returns The elapsed reading time.
	double ExecTime() {return timer.ElapsedSeconds();}
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
    kipl::base::TImage<float,2> ReadFITS(std::string filename, size_t const * const nCrop=nullptr);

    /// Read TIFF images
    /// \param filename The name of the file to read.
    /// \param nCrop ROI to read.
    /// \returns A floating point image
    kipl::base::TImage<float,2> ReadTIFF(std::string filename, size_t const * const nCrop=nullptr);

    /// Read PNG images
    /// \param filename The name of the file to read.
    /// \param nCrop ROI to read.
    /// \returns A floating point image
    /// \todo Implement PNG support
    kipl::base::TImage<float,2> ReadPNG(std::string filename,  size_t const * const nCrop=nullptr);

    /// Read HDF5 images
    /// \param filename The name of the file to read.
    /// \param nCrop ROI to read.
    /// \returns A floating point image
    /// \todo Implement PNG support
    kipl::base::TImage<float,2> ReadHDF(std::string filename,  size_t const * const nCrop=nullptr);



    /// Interface to the interactor that updates the message and progress.
    /// \param val Progress, a value between 0.0 and 1.0.
    /// \param msg String contating the update message.
    /// \returns True if the there was a user abort request.
	bool UpdateStatus(float val, std::string msg);

    /// \returns True if there was a user abort request.
    bool Aborted();

    void PrintCrop(std::string name, size_t *crop);
    void PrintCrop(std::string name, int *crop);

    kipl::profile::Timer timer; ///< Timer to measure the execution time for the reading.
    kipl::interactors::InteractionBase *m_Interactor;  ///< Reference to an interactor object.
};

#endif
