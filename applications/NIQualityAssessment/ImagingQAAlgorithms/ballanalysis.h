#ifndef BALLANALYSIS_H
#define BALLANALYSIS_H

#include "imagingqaalgorithms_global.h"

#include <map>

#include <base/timage.h>
#include <logging/logger.h>


class BallAnalysis
{
    kipl::logging::Logger logger;
public:
    BallAnalysis();

    void process(kipl::base::TImage<float,3> _img);
    void computeSphereGeometry(kipl::base::TImage<float,3> img, float *center, float &radius);
    void getEdgeProfile(float azimuth_angle,
                        float elevation_angle,
                        float cone_angle,
                        float width, std::map<float,float> &profile);
    void getEdgeProfile(std::map<float,float> &profile);
    float getRadius() { return radius; }
    const float * getCenter() { return center; }
protected:
    void projectionCenter();

    kipl::base::TImage<float,3> img;
    float center[3];
    float radius;
};

#endif // BALLANALYSIS_H
