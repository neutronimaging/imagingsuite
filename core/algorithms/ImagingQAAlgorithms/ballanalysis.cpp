#include "ballanalysis.h"

namespace ImagingQAAlgorithms {
BallAnalysis::BallAnalysis() :
    logger("BallAnalysis")
{

}

void BallAnalysis::setImage(kipl::base::TImage<float,3> _img)
{
    img=_img;
    img.Clone();
    computeSphereGeometry();
}

/// \brief Computes the global radial edge profile of a selected region
/// The the profile uses the center estimated when set
/// \param azimuth_angle
/// \param r0 The start radius
/// \param r1 The end radius
/// \param profile a map containg the average intensity for each encountered radius.
void BallAnalysis::getEdgeProfile(float azimuth_angle,
                    float elevation_angle,
                    float cone_angle,
                    float r0, float r1, std::map<float,float> &profile)
{
    kipl::base::coords3D b0(0,0,0);
    kipl::base::coords3D b1(img.Size(0)-1,img.Size(1)-1,img.Size(2)-1);

    // todo find bounding box

    std::map<float,pair<float,int> > profile0;
    profileInBoundingBox(center, b0, b1, r0,r1,profile0);

}

/// \brief Computes the global radial edge profile of image object
/// The the profile uses the center estimated when set
/// \param r0 The start radius
/// \param r1 The end radius
/// \param profile a map containg the average intensity for each encountered radius.
void BallAnalysis::getEdgeProfile(float r0, float r1,std::map<float,float> &profile)
{
    kipl::base::coords3D b0(0,0,0);
    kipl::base::coords3D b1(img.Size(0)-1,img.Size(1)-1,img.Size(2)-1);

    std::map<float,pair<float,int> > profile0;
    profileInBoundingBox(center, b0, b1, r0,r1,profile0);


}

void BallAnalysis::computeSphereGeometry()
{

}

void BallAnalysis::projectionCenter()
{

}

void BallAnalysis::profileInBoundingBox(kipl::base::coords3D center,
                          kipl::base::coords3D b0,
                          kipl::base::coords3D b1,
                          float r0,
                          float r1,
                          std::map<float,pair<float,int> > &profile)
{
    profile.clear();

    for (int z=b0.z; z<=b1.z; ++z) {
        for (int y=b0.y; y<=b1.y; ++y) {
            for (int x=b0.x; x<=b1.x; ++x) {

            }
        }
    }
}

}
