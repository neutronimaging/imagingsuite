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
                  const std::vector<size_t> & _roi,
                  bool bTilt,
                  double &center,
                  double &tilt,
                  double &pivot);

    void estimate(kipl::base::TImage<float,2> &img0,
                  kipl::base::TImage<float,2> &img180,
                  ImagingAlgorithms::TomoCenter::eEstimator est,
                  const std::vector<size_t> &_roi,
                  bool bTilt,
                  float &center,
                  float &tilt,
                  float &pivot);

    vector<double> centers();
    void tiltParameters(double &k, double &m);
    double center(double y);
    double R2();
    void savePoints(bool save);
    void setPointsFileName(const std::string & fname);
private:
    double computeR2(std::vector<double> &vec,double k, double m);
    float CorrelationCenter();

    float LeastSquareCenter();

    float CenterOfGravity(const kipl::base::TImage<float,2> img, size_t start, size_t end);

    std::vector<size_t> roi;
    double fraction;
    double tiltM;
    double tiltK;
    double mR2;
    kipl::base::TImage<float,2> m_Proj0Deg;
    kipl::base::TImage<float,2> m_Proj180Deg;
    std::vector<double> m_vCoG;
    bool bSavePoints;
    std::string pointsFileName;
};
}
#endif // TOMOCENTER_H
