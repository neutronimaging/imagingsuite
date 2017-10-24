
#include "../../include/math/tcenterofgravity.h"
#include "../../include/base/thistogram.h"
#include "../../include/segmentation/thresholds.h"

namespace kipl {
namespace math {

CenterOfGravity::CenterOfGravity() :
    profileX(nullptr),
    profileY(nullptr),
    profileZ(nullptr)
{
    std::fill(dims, dims+3,0UL);
}

CenterOfGravity::~CenterOfGravity()
{
    clearAllocation();
}

void CenterOfGravity::clearAllocation()
{
    if (profileX!=nullptr) {
        delete [] profileX;
        profileX=nullptr;
    }

    if (profileY!=nullptr) {
        delete [] profileY;
        profileY=nullptr;
    }

    if (profileZ!=nullptr) {
        delete [] profileZ;
        profileZ=nullptr;
    }
}

kipl::base::coords3Df CenterOfGravity::findCenter(kipl::base::TImage<float,3> img, bool applythreshold)
{
    float threshold=0.0f;

    if (applythreshold)
    {
        const int nBins=1024;
        size_t hist[nBins+1];
        float axis[nBins+1];
        kipl::base::Histogram(img.GetDataPtr(), img.Size(), hist, nBins, 0.0f, 0.0f, axis);
        int idx=kipl::segmentation::Threshold_Otsu(hist,nBins);

        threshold=axis[idx];
    }

    computeProfiles(img,applythreshold,threshold);

    kipl::base::coords3Df cog;

    cog.x=computeCOG(profileX,dims[0]);

    cog.y=computeCOG(profileY,dims[1]);

    cog.z=computeCOG(profileZ,dims[2]);

    return cog;
}

float CenterOfGravity::computeCOG(float *profile,int N)
{
    float cog=0;
    float wsum=0.0f;
    float sum=0.0f;
    for (int i=0; i<dims[0]; ++i) {
        sum+=profileX[i];
        wsum+=i*profileX[i];
    }

    cog=wsum/sum;

    return cog;
}

void CenterOfGravity::computeProfiles(kipl::base::TImage<float,3> img, bool applythreshold, float threshold)
{
    const size_t *dims = img.Dims();

    clearAllocation();

    profileX=new float[dims[0]];
    profileY=new float[dims[1]];
    profileZ=new float[dims[2]];

    std::fill(profileX,profileX+dims[0],0.0f);
    std::fill(profileY,profileY+dims[1],0.0f);
    std::fill(profileZ,profileZ+dims[2],0.0f);

    if (applythreshold) {
        for (size_t z=0; z<dims[2]; ++z) {
            for (size_t y=0; y<dims[1]; ++y) {
                float *pImg=img.GetLinePtr(y,z);
                for (size_t x=0; x<dims[0]; ++x) {
                    float val= (threshold<=pImg[x]);
                    profileX[x]+=val;
                    profileY[y]+=val;
                    profileZ[z]+=val;
                }
            }
        }
    }
    else {
        for (size_t z=0; z<dims[2]; ++z) {
            for (size_t y=0; y<dims[1]; ++y) {
                float *pImg=img.GetLinePtr(y,z);
                for (size_t x=0; x<dims[0]; ++x) {
                    float val=pImg[x];
                    profileX[x]+=val;
                    profileY[y]+=val;
                    profileZ[z]+=val;
                }
            }
        }
    }
}

}}
