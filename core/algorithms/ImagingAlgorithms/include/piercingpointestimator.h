//<LICENSE>

#ifndef PIERCINGPOINTESTIMATOR_H
#define PIERCINGPOINTESTIMATOR_H

#include "ImagingAlgorithms_global.h"

#include <tnt_array1d.h>
#include <vector>

#include <base/timage.h>
#include <logging/logger.h>

namespace ImagingAlgorithms {
class IMAGINGALGORITHMSSHARED_EXPORT PiercingPointEstimator
{
    kipl::logging::Logger logger;
public:
    PiercingPointEstimator();

    pair<float,float> operator()(kipl::base::TImage<float,2> &img,const std::vector<size_t> & roi = {});

    pair<float,float> operator()(kipl::base::TImage<float,2> &img,
                                 kipl::base::TImage<float,2> &dc,
                                 bool gaincorrection=true,
                                 const std::vector<size_t> &roi={});
private:

    void ComputeEstimate(kipl::base::TImage<float, 2> &img);
    pair<float,float> LocateMax();


    kipl::base::TImage<float,2> correctedImage;
    TNT::Array1D< double > parameters;

    float m_gainThreshold;
};

}
#endif // PIERCINGPOINTESTIMATOR_H
