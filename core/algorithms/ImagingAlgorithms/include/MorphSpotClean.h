//<LICENSE>

#ifndef MORPHSPOTCLEAN_H
#define MORPHSPOTCLEAN_H

#include <string>
#include <iostream>
#include <vector>
#include <array>

#include "ImagingAlgorithms_global.h"
#include <base/timage.h>
#include <base/kiplenums.h>
#include <morphology/morphology.h>
#include <logging/logger.h>
#include <containers/ArrayBuffer.h>
#include <math/LUTCollection.h>
#include <interactors/interactionbase.h>

#include "pixelinfo.h"


namespace ImagingAlgorithms {

enum eMorphDetectionMethod {
    MorphDetectDarkSpots = 0,
    MorphDetectBrightSpots,
    MorphDetectAllSpots,
    MorphDetectHoles,
    MorphDetectPeaks,
    MorphDetectBoth
};

enum eMorphCleanMethod {
    MorphCleanReplace = 0,
    MorphCleanFill
};

class IMAGINGALGORITHMSSHARED_EXPORT MorphSpotClean
{
    kipl::logging::Logger logger;
    const float mark;
public:
    MorphSpotClean(kipl::interactors::InteractionBase *interactor=nullptr);
    void process(kipl::base::TImage<float,2> &img, float th, float sigma);
    void process(kipl::base::TImage<float,2> &img, std::vector<float> &th, std::vector<float> &sigma);

    void process(kipl::base::TImage<float,3> &img, float th, float sigma);
    void process(kipl::base::TImage<float,3> &img, std::vector<float> &th, std::vector<float> &sigma);

    void setConnectivity(kipl::base::eConnectivity conn = kipl::base::conn8);
    kipl::base::eConnectivity connectivity();
    void setCleanMethod(eMorphDetectionMethod mdm, eMorphCleanMethod mcm);
    eMorphDetectionMethod detectionMethod();
    eMorphCleanMethod cleanMethod();
    void setLimits(bool bClamp, float fMin, float fMax, int nMaxArea);
    void setThresholdByFraction(bool method);
    void setDetectionStrelSize(size_t size);
    size_t detectionStrelSize();
    std::vector<float> clampLimits();
    bool clampActive();
    int maxArea();
    void setCleanInfNan(bool activate);
    bool cleanInfNan();
    void setEdgeConditioning(int nSmoothLenght);
    int edgeConditionLength();
    pair<kipl::base::TImage<float,2>,kipl::base::TImage<float,2>> detectionImage(kipl::base::TImage<float,2> img, bool removeBias);
    void detectionImage(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &noholes, kipl::base::TImage<float,2> &nopeaks, bool removeBias);
    void useThreading(bool x);
    bool isThreaded();
    std::string dumpParameters();

    /// \brief Set number of allowed threads.
    /// \param N is a value between 0 and number of available cores. 0 = use max available cores.
    void setNumberOfThreads(int N);
    int numberOfThreads();
//    const kipl::base::TImage<float> & getNoHoles();
//    const kipl::base::TImage<float> & getNoPeaks();

protected:
    void ProcessReplace(kipl::base::TImage<float,2> &img);
    void ProcessFill(kipl::base::TImage<float,2> &img);
    void process(kipl::base::TImage<float, 3> *pImg,
                 size_t first,
                 size_t last,
                 std::vector<float> th,
                 std::vector<float> sigma,
                 size_t tid=0UL);

    void PadEdges(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &padded);
    void unpadEdges(kipl::base::TImage<float,2> &padded, kipl::base::TImage<float,2> &img);
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

    bool UpdateStatus(float val, std::string msg);

    kipl::base::TImage<float,2> DetectSpots(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels);
    kipl::base::TImage<float,2> DetectHoles(kipl::base::TImage<float,2> img);
    kipl::base::TImage<float,2> DetectPeaks(kipl::base::TImage<float,2> img);
    kipl::base::TImage<float,2> DetectBoth(kipl::base::TImage<float,2> img);

    kipl::base::TImage<float,2> DetectBrightSpots(kipl::base::TImage<float,2> &img);
    kipl::base::TImage<float,2> DetectDarkSpots(kipl::base::TImage<float,2> &img);
    kipl::base::TImage<float,2> DetectAllSpots(kipl::base::TImage<float,2> &img);

    void ExcludeLargeRegions(kipl::base::TImage<float,2> &img);

    void replaceInfNaN(kipl::base::TImage<float,2> &img);

    kipl::base::TImage<float,2> CleanByArray(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels);

    bool m_bUseThreading;
    int m_nNumberOfThreads;
    kipl::base::eConnectivity m_eConnectivity;
    eMorphCleanMethod              m_eMorphClean;
    eMorphDetectionMethod          m_eMorphDetect;
    size_t m_seSize;
    size_t m_nEdgeSmoothLength;
    size_t m_nPadMargin;
    int m_nMaxArea;
    bool m_bRemoveInfNan;
    bool m_bClampData;
    bool m_bThresholdByPercentage;
    float m_fMinLevel;
    float m_fMaxLevel;
    std::vector<float> m_fThreshold;
    std::vector<float> m_fSigma;
    kipl::math::SigmoidLUT m_LUT;

    kipl::base::TImage<float,2> mask;
    int sx;
    int ng[8];
    int first_line;
    int last_line;

    std::atomic_int m_nCounter;

    kipl::interactors::InteractionBase *m_interactor;
};

}

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eMorphCleanMethod mc);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream &s, ImagingAlgorithms::eMorphCleanMethod mc);
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::eMorphCleanMethod &mc);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eMorphDetectionMethod md);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream &s, ImagingAlgorithms::eMorphDetectionMethod md);
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::eMorphDetectionMethod &md);

#endif // MORPHSPOTCLEAN_H

