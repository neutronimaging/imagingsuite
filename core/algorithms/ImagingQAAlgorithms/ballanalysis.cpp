//<LICENSE>
#include <math/tcenterofgravity.h>
#include <base/index2coord.h>
#include <io/io_serializecontainers.h>

#include "ballanalysis.h"

namespace ImagingQAAlgorithms {
BallAnalysis::BallAnalysis() :
    logger("BallAnalysis"),
    precision(0.1f)
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

    std::map<float,kipl::math::Statistics > profile0;
    profileInBoundingBox(center, b0, b1, r0,r1,profile0);

}

/// \brief Computes the global radial edge profile of image object
/// The the profile uses the center estimated when set
/// \param r0 The start radius
/// \param r1 The end radius
/// \param profile a map containg the average intensity for each encountered radius.
void BallAnalysis::getEdgeProfile(float r0, float r1, vector<float> &distance, vector<float> &profile, vector<float> &stddev)
{
    kipl::base::coords3D b0(0,0,0);
    kipl::base::coords3D b1(img.Size(0)-1,img.Size(1)-1,img.Size(2)-1);

    std::map<float,kipl::math::Statistics > profile0;
    profileInBoundingBox(center, b0, b1, r0,r1,profile0);

    distance.clear();
    profile.clear();
    stddev.clear();
    std::ostringstream msg;


    for (auto it=profile0.begin(); it!=profile0.end(); ++it ) {
        distance.push_back(it->first);
        profile.push_back(it->second.E());
        stddev.push_back(it->second.s());
    }

    msg<<"p0="<<profile0.size()<<", p="<<profile.size();

    logger(logger.LogMessage, msg.str());
}

void BallAnalysis::computeSphereGeometry()
{
    std::ostringstream msg;
    kipl::math::CenterOfGravity cog;

    center = cog.findCenter(img,true);

    size_t N=img.Size(0);

    float *cl = new float[img.Size(0)];
    float *pImgCL = img.GetLinePtr(static_cast<size_t>(center.y+0.5f),static_cast<size_t>(center.z+0.5));

    std::copy(pImgCL,pImgCL+N,cl);
 //   kipl::io::serializeContainer(pImgCL,pImgCL+N,"/Users/kaestner/profile.txt");

    float maxVal = *std::max_element(cl,cl+N);
    float minVal = *std::min_element(cl,cl+N);

    float th = (maxVal+minVal)*0.5f;

    size_t pos0=0;
    for (pos0=0; pos0<static_cast<size_t>(center.x+0.5f); ++pos0) {
        if (th<cl[pos0])
            break;
    }

    size_t pos1=N-1;
    for (pos1=N-1; static_cast<size_t>(center.x+0.5f)<pos1; --pos1) {
        if (th<cl[pos1])
            break;
    }



    radius = (static_cast<float>(pos1)-static_cast<float>(pos0))*0.5f;
//    msg<<"Threshold:"<<th<<", min:"<<minVal<<", max:"<<maxVal<<", pos0:"<<pos0<<", pos1:"<<pos1<<", radius:"<<radius;
//    logger.message(msg.str());

    delete [] cl;
}

void BallAnalysis::projectionCenter()
{

}

void BallAnalysis::profileInBoundingBox(kipl::base::coords3Df center,
                          kipl::base::coords3D b0,
                          kipl::base::coords3D b1,
                          float r0,
                          float r1,
                          std::map<float,kipl::math::Statistics> &profile)
{
    profile.clear();
    float z2=0.0f;
    float y2=0.0f;
    float x2=0.0f;
    float r=0.0f;

    const float r02 = r0;
    const float r12 = r1;

    size_t posZ=0UL;
    size_t posY=0UL;
    float *pImg=img.GetDataPtr();
    float invprecision=1.0f/precision;

    for (int z=b0.z; z<=b1.z; ++z) {
        z2=static_cast<float>(z)-center.z;
        z2=z2*z2;
        posZ=z*img.Size(0)*img.Size(1);
        for (int y=b0.y; y<=b1.y; ++y) {
            y2=static_cast<float>(y)-center.y;
            y2=y2*y2;
            y2+=z2;
            posY=posZ+y*img.Size(0);
            for (int x=b0.x; x<=b1.x; ++x) {
                x2=static_cast<float>(x)-center.x;
                x2=x2*x2;
                r=floor(sqrt(y2+x2)*invprecision+0.5)*precision;
                if ((r02<=r) && (r<=r12)) {
                    profile[r].put(pImg[posY+x]);
                }
            }
        }
    }
}

}
