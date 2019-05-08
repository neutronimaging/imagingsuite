//<LICENSE>
#ifndef TOMOCENTER_H
#define TOMOCENTER_H
#include "ImagingAlgorithms_global.h"
#include <vector>

#include <base/timage.h>
#include <logging/logger.h>

namespace ImagingAlgorithms {
class IMAGINGALGORITHMSSHARED_EXPORT TomoCenter
{
    kipl::logging::Logger logger;
public:
    enum eEstimator {
        leastSquare,
        correlation,
        centerOfGravity
    };

    TomoCenter();

    void setFraction(double f);
    void estimate(kipl::base::TImage<float,2> &img0,
                  kipl::base::TImage<float,2> &img180,
                  ImagingAlgorithms::TomoCenter::eEstimator est,
                  size_t *_roi,
                  double *center,
                  double *tilt=nullptr,
                  double *pivot=nullptr);
    vector<float> centers();
    void tiltParameters(double &k, double &m);
private:
    float CorrelationCenter(	kipl::base::TImage<float,2> proj_0,
                                kipl::base::TImage<float,2> proj_180,
                                size_t *roi);

    float LeastSquareCenter(kipl::base::TImage<float,2> proj_0,
                            kipl::base::TImage<float,2> proj_180,
                            size_t *roi);

    float CenterOfGravity(const kipl::base::TImage<float,2> img, size_t start, size_t end);

    size_t roi[4];
    double fraction;
    double tiltM;
    double tiltK;
    double R2;
    kipl::base::TImage<float,2> m_Proj0Deg;
    kipl::base::TImage<float,2> m_Proj180Deg;
    std::vector<float> m_vCoG;
};
}
#endif // TOMOCENTER_H
