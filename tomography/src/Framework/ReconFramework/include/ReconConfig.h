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

#ifndef __RECONCONFIG_H
#define __RECONCONFIG_H

#include "ReconFramework_global.h"

#include <map>
#include <list>
#include <set>
#include <string>

#include <libxml/xmlreader.h>

#include <ModuleConfig.h>
#include <ConfigBase.h>

#include <logging/logger.h>
#include <io/io_stack.h>
#include <base/kiplenums.h>

/// The reconstruction configuration structure. Used to set up the reconstruction process.
class RECONFRAMEWORKSHARED_EXPORT ReconConfig : public ConfigBase
{
public:
    /// System configuration settings.
    struct RECONFRAMEWORKSHARED_EXPORT cSystem {
        /// The constructor initializes the parameters.
		cSystem();

        /// Copy constructor (deep copy)
		cSystem(const cSystem &a);

        /// Assignment operator (deep copy)
		cSystem & operator=(const cSystem &a);
        size_t nMemory; ///< Available memory in kB.
        kipl::logging::Logger::LogLevel eLogLevel;      ///< Default log level.
        std::string WriteXML(size_t indent=0);          ///< Serializes the settings.
	};

    /// Projection configuration settings
    struct RECONFRAMEWORKSHARED_EXPORT cProjections {
        /// Enumeration of acquistion strategy
		enum eScanType {
            SequentialScan,                             ///< Tradiational linear increments uniformly distributed.
            GoldenSectionScan                           ///< Increments determined by the golden ratio.
		};

        /// Enumeration of input image types
		enum eImageType {
            ImageType_Projections=0,                    ///< Read projections as they were acquired
            ImageType_Sinograms,                        ///< Read sinograms
            ImageType_Proj_RepeatProjection,            ///< Repeat a single projection, test mode that reconstructs a cylinder from a single projection
            ImageType_Proj_RepeatSinogram               ///< Repeat a single slice, the input is projections. Indended use; find center of rotation.
		};

        enum eBeamGeometry {
            BeamGeometry_Parallel=0,
            BeamGeometry_Cone,
            BeamGeometry_Helix
        };

        /// Base constructor, initialize default values.
		cProjections();

        /// Copy constructor, performs a deep copy.
        /// \param a the struct to copy.
		cProjections(const cProjections & a);

        /// Assignment operation, performs a deep copy
        /// \param a the struct to copy.
		cProjections & operator=(const cProjections &a);

        size_t nDims[2];            ///< Dimensions of the projections.
        eBeamGeometry beamgeometry; ///< Selects beam geometry for the data
        float fResolution[2];       ///< Resolution of the projections in mm/pixel.
        float fBinning;             ///< Binning factor, currently only integers are valid.
        size_t nMargin;             ///< Width of the image margin to relax the boundary processing criteria
        size_t nFirstIndex;         ///< The index number of the first projection in the data set
        size_t nLastIndex;          ///< The index number of the last projection in the data set
        size_t nProjectionStep;     ///< Increment of the projection index during read
        std::set<size_t> nlSkipList;///< List of projection indices that are retakes and will be skipped. This is not a missing angle.
        bool bRepeatLine;           ///< Repeat line this is a part of the repeat sinogram reconstruction
        eScanType scantype;         ///< Indicates how the data was acquired
        eImageType imagetype;       ///< Indicates how the data is arranged in the images.
        float fCenter;              ///< Center of rotation
        float fSOD;                 ///< Source object distance, relevant for divergent beam only
        float fSDD;                 ///< Source detector distance, relevant for divergent beam only
        float fpPoint[2];           ///< Piercing point, relevant for divergent beam only
                                    ///< 2D coordinates, in pixel coordinates of the point on the image which is closest to the source: a pair of floating point number, in units of pixel
                                    ///< The first number is the column (x-coordinate) and the second number is the row  (y-coordinate)
                                    ///< The first pixel of the image is to be considered to be coordinate (0,0)
        bool bTranslate;            ///< Indicates if the center of rotation is shifted to the margin.
        float fTiltAngle;           ///< Axis tilt angle
        float fTiltPivotPosition;   ///< Pivot point of the axis tilt. This is the slice were the center of rotation is correct.
        bool bCorrectTilt;          ///< Use the axis tilt correction

        std::string sFileMask;      ///< File mask for the projection. It should be formatted using #'s as place holders for the index number.
        std::string sPath;          ///< Path to the projection data, is not used in GUI operation but can be useful for CLI operation.

        std::string sReferencePath; ///< Path to the reference data , is not used in GUI operation but can be useful for CLI operation.
        std::string sOBFileMask;    ///< File mask for the open beam reference image. It should be formatted using #'s as place holders for the index number.
        size_t nOBFirstIndex;       ///< Index number of the first open beam image.
        size_t nOBCount;            ///< Number of open beam images
        std::string sDCFileMask;    ///< File mask for the dark reference image. It should be formatted using #'s as place holders for the index number.
        size_t nDCFirstIndex;       ///< Index number of the first dark image.
        size_t nDCCount;            ///< Number of dark image.

        size_t roi[4];              ///< Region of interest to reconstruct (x0,y0,x1,y1).
        size_t projection_roi[4];   ///< Region of interest for the entire sample (x0,y0,x1,y1).
        size_t dose_roi[4];         ///< Region of interest to calculate the projection dose (x0,y0,x1,y1).
        float fScanArc[2];          ///< Provides the first and last scan angles
        kipl::base::eImageFlip eFlip;   ///< Projection flip operation (horizontal, vertical, both).
        kipl::base::eImageRotate eRotate; ///< Projection rotation operation (90 cw,90 ccw, 180).

        kipl::base::eRotationDirection eDirection; ///< Direction of rotation (clockwise, counterclockwise)

        /// Writes the configuration to a string with XML formatting.
        /// \param indent Indent the XML block by N characters.
		std::string WriteXML(size_t indent=0);
	};

    /// Configuration information for the reconstructed matrix.
    struct RECONFRAMEWORKSHARED_EXPORT cMatrix {
        /// Base constructor, sets default configuration
        cMatrix();

        /// Copy constructor, performs a deep copy.
        /// \param a The configuration to copy
		cMatrix(const cMatrix &a);

        /// Assignment operator, performs a deep copy.
        /// \param a The configuration to copy
		cMatrix & operator=(const cMatrix &a);

        size_t nDims[3];                ///< Matrix dimensions (x,y,z);
        float fRotation;                ///< Rotation offset of the data.
        std::string sDestinationPath;   ///< Destination path of the reconstructed slices
        bool bAutomaticSerialize;       ///< Indicates if the reconstructed data should be saved to disk or only used in the GUI.
        std::string sFileMask;          ///< File mask for the reconstructed slices. It should be formatted using #'s as place holders for the index number.
        size_t nFirstIndex;             ///< First index of the reconstructed slices, this is mostly set to the line index in the reconstructed projection.
        float fGrayInterval[2];         ///< Interval in which the graylevels are represented.
        bool bUseROI;                   ///< Reconstruct the data in the defined region of interest.
        size_t roi[4];                  ///< Region of interest to reconstruct (slice coordinates x0,y0,x1,y1). Relevant for parallel beam
        size_t voi[6];                  ///< Subvolume to reconstruct (volume coordinates x0, x1, y0, y1, z0, z1). Relevant for divergent beam
//        bool bUseVOI;                   ///< Reconstruct the data in the defined volume of interest. Relevant for divergent beam
        kipl::io::eFileType FileType;   ///< File type of the reconstructed slices.
        float fVoxelSize[3];            ///< Voxel size of the reconstructed volume, relevant for divergent beam only


        /// Writes the configuration to a string with XML formatting.
        /// \param indent Indent the XML block by N characters.
        std::string WriteXML(size_t indent=0);
	};

    /// Base constructor of the reonstruction configation.
	ReconConfig(void);

    /// Copy constructor of the reconstructor configuration.
    /// \param config The configuration to copy.
    ReconConfig(const ReconConfig &config);

    /// Assignment operator forthe reconstruction configuration
    /// \param config The configuration to copy.
    const ReconConfig & operator=(const ReconConfig &config);

    bool SanityCheck();

    /// Destructor to clean up
	~ReconConfig(void);

    cSystem System;                 ///< System configuration information
    cProjections ProjectionInfo;    ///< Projection configuration information
    cMatrix MatrixInfo;             ///< Matrix configuration information
    ModuleConfig backprojector;     ///< Configuration of the back projection module

    /// Writes the reconstruction configuration to a string with XML formatting.
    /// \param indent Indent the XML block by N characters.
    std::string WriteXML();

protected:
    /// Parse the contents of an opened configuration file.
    /// \param reader A handle to the an opened xml reader.
    /// \param cName Name of the main block.
	virtual void ParseConfig(xmlTextReaderPtr reader, std::string cName);

    /// Parse a list of arguments provided by the CLI
    /// \param args A list of arguments as they come from the CLI.
    virtual void ParseArgv(std::vector<std::string> &args);

    /// Parse system parameters
    /// \param reader A handle to the an opened xml reader.
	void ParseSystem(xmlTextReaderPtr reader);

    /// Parse projection parameters
    /// \param reader A handle to the an opened xml reader.
    void ParseProjections(xmlTextReaderPtr reader);

    /// Parse matrix parameters
    /// \param reader A handle to the an opened xml reader.
	void ParseMatrix(xmlTextReaderPtr reader);

    /// Unused method?
	void ParseParameters(xmlTextReaderPtr reader);

    /// Parse process chain parameters
    /// \param reader A handle to the an opened xml reader.
    virtual void ParseProcessChain(xmlTextReaderPtr reader);
};

/// Converts a string to a scan type enum
/// \param str The string to convert
/// \param st a scan type variable
RECONFRAMEWORKSHARED_EXPORT void string2enum(const std::string str, ReconConfig::cProjections::eScanType &st);

/// Converts a string to a image type enum
/// \param str The string to convert
/// \param st an image type variable
RECONFRAMEWORKSHARED_EXPORT void string2enum(const std::string str, ReconConfig::cProjections::eImageType &it);

/// Converts an Image type enum to a string
/// \param st a image type variable
/// \returns The converted string
RECONFRAMEWORKSHARED_EXPORT std::string enum2string(ReconConfig::cProjections::eImageType &it);

/// Converts a string to a beam geometry enum
/// \param str The string to convert
/// \param st an image type variable
RECONFRAMEWORKSHARED_EXPORT void string2enum(const std::string str, ReconConfig::cProjections::eBeamGeometry &bg);

/// Converts an beam geometry enum to a string
/// \param st a image type variable
/// \returns The converted string
RECONFRAMEWORKSHARED_EXPORT std::string enum2string(ReconConfig::cProjections::eBeamGeometry &bg);

/// Writes the enum to a stream
/// \param s the target stream
/// \param st a scan type variable
/// \returns The updated stream
RECONFRAMEWORKSHARED_EXPORT std::ostream & operator<<(std::ostream &s, ReconConfig::cProjections::eBeamGeometry bg);

/// Writes the enum to a stream
/// \param s the target stream
/// \param st a scan type variable
/// \returns The updated stream
RECONFRAMEWORKSHARED_EXPORT std::ostream & operator<<(std::ostream &s, ReconConfig::cProjections::eScanType st);

/// Writes the enum to a stream
/// \param s the target stream
/// \param st a scan type variable
/// \returns The updated stream
/// RECONFRAMEWORKSHARED_EXPORT std::ostream & operator<<(std::ostream &s, ReconConfig::cProjections::eImageType it);



#endif
