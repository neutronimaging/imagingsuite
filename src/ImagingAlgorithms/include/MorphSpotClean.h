//<LICENSE>

#ifndef MORPHSPOTCLEAN_H
#define MORPHSPOTCLEAN_H

#include <string>
#include <iostream>

#include "ImagingAlgorithms_global.h"
#include <base/timage.h>
#include <morphology/morphology.h>
#include <logging/logger.h>
#include <containers/ArrayBuffer.h>
#include <math/LUTCollection.h>

#include "pixelinfo.h"


namespace ImagingAlgorithms {

enum eMorphDetectionMethod {
    MorphDetectHoles = 0,
    MorphDetectPeaks,
    MorphDetectBoth
};

enum eMorphCleanMethod {
    MorphCleanReplace =0,
    MorphCleanFill
};

class IMAGINGALGORITHMSSHARED_EXPORT MorphSpotClean
{
    kipl::logging::Logger logger;
    const float mark;
public:
    MorphSpotClean();
    void Process(kipl::base::TImage<float,2> &img, float th, float sigma);

    void setConnectivity(kipl::morphology::MorphConnect conn = kipl::morphology::conn8);
    void setCleanMethod(eMorphDetectionMethod mdm, eMorphCleanMethod mcm);
    void setLimits(float fMin, float fMax, int nMaxArea);
    void setEdgeConditioning(int nSmoothLenght);
    kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> img);

protected:
    void FillOutliers(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &noholes, kipl::base::TImage<float,2> &nopeaks);
    void ProcessReplace(kipl::base::TImage<float,2> &img);
    void ProcessFill(kipl::base::TImage<float,2> &img);

    void PadEdges(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded);
    void UnpadEdges(kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &img);
    /// \brief Prepares neighborhood indexing LUT
    /// \param dimx Length of the x-axis
    /// \param N number of pixels in the image
    int PrepareNeighborhood(int dimx, int N);

    /// \brief Extracts the neighborhood of a pixel. Skips pixels with the value max(float)
    /// \param pImg reference to the image matrix
    /// \param idx Index of the pixel
    /// \param neigborhood preallocated array to carry the extracted neighbors
    /// \returns number of extracted pixels in the array
    int Neighborhood(float * pImg, int idx, float * neigborhood);

    kipl::base::TImage<float,2> DetectSpots(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels);
    kipl::base::TImage<float,2> DetectHoles(kipl::base::TImage<float,2> img);
    kipl::base::TImage<float,2> DetectPeaks(kipl::base::TImage<float,2> img);
    kipl::base::TImage<float,2> DetectBoth(kipl::base::TImage<float,2> img);

    void ExcludeLargeRegions(kipl::base::TImage<float,2> &img);

    kipl::base::TImage<float,2> CleanByArray(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels);

    kipl::morphology::MorphConnect m_eConnectivity;
    eMorphCleanMethod              m_eMorphClean;
    eMorphDetectionMethod          m_eMorphDetect;
    int m_nEdgeSmoothLength;
    int m_nPadMargin;
    int m_nMaxArea;
    float m_fMinLevel;
    float m_fMaxLevel;
    float m_fThreshold;
    float m_fSigma;
    kipl::math::SigmoidLUT m_LUT;

    kipl::base::TImage<float,2> mask;
    int sx;
    int ng[8];
    int first_line;
    int last_line;

};

}

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eMorphCleanMethod mc);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream &s, ImagingAlgorithms::eMorphCleanMethod mc);
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::eMorphCleanMethod &mc);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eMorphDetectionMethod md);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream &s, ImagingAlgorithms::eMorphDetectionMethod md);
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::eMorphDetectionMethod &md);

#endif // MORPHSPOTCLEAN_H
