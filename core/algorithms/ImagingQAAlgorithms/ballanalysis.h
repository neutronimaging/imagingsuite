#ifndef BALLANALYSIS_H
#define BALLANALYSIS_H

#include "imagingqaalgorithms_global.h"

#include <map>

#include <base/timage.h>
#include <base/index2coord.h>
#include <logging/logger.h>

namespace ImagingQAAlgorithms {

class IMAGINGQAALGORITHMSSHARED_EXPORT BallAnalysis
{
    kipl::logging::Logger logger;
public:
    BallAnalysis();

    void setImage(kipl::base::TImage<float,3> _img);

    /// \brief Computes the global radial edge profile of a selected region
    /// The the profile uses the center estimated when set
    /// \param azimuth_angle
    /// \param r0 The start radius
    /// \param r1 The end radius
    /// \param profile a map containg the average intensity for each encountered radius.
    void getEdgeProfile(float azimuth_angle,
                        float elevation_angle,
                        float cone_angle,
                        float r0, float r1, std::map<float,float> &profile);

    /// \brief Computes the global radial edge profile of image object
    /// The the profile uses the center estimated when set
    /// \param r0 The start radius
    /// \param r1 The end radius
    /// \param profile a map containg the average intensity for each encountered radius.
    void getEdgeProfile(float r0, float r1,std::map<float,float> &profile);
    float getRadius() { return radius; }
    const kipl::base::coords3D getCenter() { return center; }

protected:
    void computeSphereGeometry();
    void projectionCenter();
    void profileInBoundingBox(kipl::base::coords3D center,
                              kipl::base::coords3D b0,
                              kipl::base::coords3D b1,
                              float r0,
                              float r1,
                              std::map<float,pair<float,int> > &profile);

    kipl::base::TImage<float,3> img;
    kipl::base::coords3D center;
    float radius;
};

}

#endif // BALLANALYSIS_H
