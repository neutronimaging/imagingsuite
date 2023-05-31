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
    enum eSegmentationMethod {
        method_manual,
        method_otsu,
        method_rosin,
        method_usermask
    };
    SegmentBB();
    SegmentBB(const std::map<std::string, std::string> &params );

    void setDotRadius(float radius);
    void setDotAreaLimits(const std::vector<size_t> & limits);
    void setSegmentationMethod(eSegmentationMethod method, float threshold = 0.0);

    std::map<std::string, std::string> parameters();
    void setParameters(const std::map<std::string, std::string> &params);

    void exec(const kipl::base::TImage<float,2> & bb, 
                    kipl::base::TImage<float,2> & mask,
              const std::vector<size_t> & ROI={});

    const kipl::base::TImage<float,2> & mask();
    std::tuple<std::vector<float>,std::vector<float> >    dotCoordinates();
    std::tuple<std::vector<size_t>, std::vector<float> >  histogram();

private:
    void segment(const kipl::base::TImage<float,2> &bb,
                       kipl::base::TImage<float,2> &mask);

    void identifyBBs(const kipl::base::TImage<float,2> &bb,
                     const kipl::base::TImage<float,2> &mask,
                     const std::vector<size_t> & ROI);
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