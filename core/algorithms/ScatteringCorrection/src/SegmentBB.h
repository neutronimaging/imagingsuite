//<LICENSE>
#ifndef SEGMENTBB_H_
#define SEGMENTBB_H_

#include "ScatteringCorrection_global.h"

#include <map>
#include <vector>
#include <string>
#include <tuple>

#include <base/timage.h>
#include <logging/logger.h>
#include <morphology/regionproperties.h>
#include <morphology/label.h>
#include <segmentation/thresholds.h>

class SCATTERINGCORRECTIONSHARED_EXPORT SegmentBB
{
private:
    kipl::logging::Logger logger;
public:
    enum class eSegmentationMethod {
        manual,
        otsu,
        rosin,
        usermask
    };

    /// @brief  Base C'tor for the segmentation class
    SegmentBB();

    /// @brief C'tor for initialization using a list of paramters
    /// @param params Parameter list, all parameters are converted to strings
    SegmentBB(const std::map<std::string, std::string> &params );

    /// @brief Sets the radius of the BB dots in the mask image
    /// @param radius The dot radius
    void setDotRadius(float radius);

    /// @brief Limits the area of the segmented dots to reject small and large dots
    /// @param limits Vector with upper and lower limit on the dot area
    void setDotAreaLimits(const std::vector<size_t> & limits);

    /// @brief Sets which segmentation method to use to select the dots
    /// @param method Method is selected using an enum 
    /// @param threshold Optional threshold, only needed for method_manual
    void setSegmentationMethod(eSegmentationMethod method, float threshold = 0.0);

    /// @brief The current segmentation configuration as parameter list
    /// @return The parameter list
    std::map<std::string, std::string> parameters();

    /// @brief Configure the segmenation using a list of parameters
    /// @param params The parameter list
    void setParameters(const std::map<std::string, std::string> &params);

    /// @brief Perform the segmentation
    /// @param bb Image with BBs
    /// @param mask The segmented mask image. Segmentation is performed if the mask has a different size than the BB image.
    /// @param ROI Optional ROI to limit down the search area
    void exec(const kipl::base::TImage<float,2> & bb, 
                    kipl::base::TImage<float,2> & mask,
              const std::vector<size_t> & ROI={});

    /// @brief The current mask image
    /// @return The mask image
    const kipl::base::TImage<float,2> & mask();

    /// @brief The coordinates of the segment dot pixels
    /// @return A tuple with the x and y coordinates
    std::tuple<std::vector<float>,  std::vector<float> >  dotCoordinates();

    /// @brief Detection image histogram
    /// @return A tuple with the bins and start bin position
    std::tuple<std::vector<size_t>, std::vector<float> >  histogram();

    eSegmentationMethod segmentationMethod() const { return m_segmentationMethod; }
    float threshold() const { return m_fThreshold; }

private:
    /// @brief Performs the segmentation task
    /// @param bb A graylevel image with BB dots
    /// @param mask The segmented mask image
    void segment(const kipl::base::TImage<float,2> &bb,
                       kipl::base::TImage<float,2> &mask);

    /// @brief Indexes the bb dots in the segmented image
    /// @param bb 
    /// @param mask 
    /// @param ROI 
    void identifyBBs(const kipl::base::TImage<float,2> &bb,
                     const kipl::base::TImage<float,2> &mask,
                     const std::vector<size_t> & ROI);

    /// @brief Creates an image with refined dots having radius provided by setRadius()
    /// @param dims Vector with the image dimensions
    void prepareMask(const std::vector<size_t> & dims);

    float m_fRadius;
    std::vector<size_t> m_areaLimits;
    eSegmentationMethod m_segmentationMethod;
    float m_fThreshold;
    std::vector<size_t> m_histBins;
    std::vector<float>  m_histAxis;

    kipl::base::TImage<float,2> m_mask;
    kipl::morphology::RegionProperties<int,float> m_regProps;
};

std::string SCATTERINGCORRECTIONSHARED_EXPORT enum2string(SegmentBB::eSegmentationMethod sm);
void SCATTERINGCORRECTIONSHARED_EXPORT string2enum(const std::string & str, SegmentBB::eSegmentationMethod &sm);
const std::ostream &  SCATTERINGCORRECTIONSHARED_EXPORT operator<<(std::ostream &s, SegmentBB::eSegmentationMethod sm);


#endif // SEGMENTBB_H_