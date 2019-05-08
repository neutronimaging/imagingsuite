//<LICENSE>
#include "../include/tomocenter.h"

#include <algorithm>
#include <sstream>

#include <QDebug>

#include <math/linfit.h>
#include <base/tsubimage.h>
#include <base/KiplException.h>
#include <math/mathconstants.h>
#include <base/tpermuteimage.h>

namespace ImagingAlgorithms {
TomoCenter::TomoCenter() :
    logger("TomoCenter"),
    roi{0,0,0,0},
    fraction(0.9),
    tiltM(0.0),
    tiltK(1.0),
    R2(0)
{

}

void TomoCenter::setFraction(double f)
{
    fraction=f;
}

void TomoCenter::estimate(kipl::base::TImage<float, 2> &img0,
                          kipl::base::TImage<float, 2> &img180,
                          ImagingAlgorithms::TomoCenter::eEstimator est,
                          size_t *_roi,
                          double *center,
                          double *tilt,
                          double *pivot)
{
    std::ostringstream msg;
    std::copy_n(_roi,4,roi);

    m_vCoG.clear();
    switch (est)
    {
        case leastSquare:
            LeastSquareCenter(m_Proj0Deg,m_Proj180Deg,roi);
            break;
        case correlation:
            CorrelationCenter(m_Proj0Deg,m_Proj180Deg,roi);
            break;
        default:
            CorrelationCenter(m_Proj0Deg,m_Proj180Deg,roi);
            break;
    }

    size_t N=m_vCoG.size();
    if ((tilt!=nullptr) && (pivot!=nullptr))
    {
        tiltK=1.0;
        tiltM=0.0;

        float *x=new float[N];
        float *y=new float[N];

        for (size_t i=0; i<N; ++i) {
            x[i]=i+roi[1];
            y[i]=m_vCoG[i];
        }

        kipl::math::LinearLSFit(x,y,static_cast<int>(N),&tiltM,&tiltK,&R2,fraction);
        delete [] x;
        delete [] y;

        *tilt=-atan(tiltK)*180.0/dPi;
        double vertical_center=(roi[3]-roi[1])/2;
        *center=tiltK*vertical_center+tiltM;
        *pivot=vertical_center;

        msg.str("");
        msg<<"Estimated center="<<tiltM<<", tilt="<<tiltK<<", N="<<N<<", fraction="<<fraction<<std::endl;
        qDebug() << QString::fromStdString(msg.str());
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
    else
    {
        std::vector<float> tmpCoG=m_vCoG;

        std::sort(tmpCoG.begin(),tmpCoG.end());
        auto start = tmpCoG.begin();
        int N2=static_cast<int>(0.5*(1-fraction)*N);
        std::advance(start,N2);
        auto stop = start;
        std::advance(stop,static_cast<ptrdiff_t>(N*fraction+1));
        *center=std::accumulate(start,stop,0.0)/(stop-start);

    }
}

vector<float> TomoCenter::centers()
{
    return m_vCoG;
}

void TomoCenter::tiltParameters(double &k, double &m)
{
    k=tiltK;
    m=tiltM;
}

float TomoCenter::CorrelationCenter(kipl::base::TImage<float, 2> proj_0,
                                    kipl::base::TImage<float, 2> proj_180,
                                    size_t *roi)
{
    std::ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"Center estimation using correlation");

 //   size_t *roi = m_Config.ProjectionInfo.roi;
    msg<<"Corr center: Current ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());

    std::ofstream cogfile("cog_corr.txt");

    kipl::base::TImage<float,2> limg0,limg180;
    size_t start[2]={roi[0],roi[1]};
    size_t length[2]={roi[2]-roi[0],roi[3]-roi[1]};
    kipl::base::TSubImage<float,2> cropper;
    limg0=cropper.Get(proj_0,start,length);
    limg180 = kipl::base::Mirror(cropper.Get(m_Proj180Deg,start,length),kipl::base::ImageAxisX);

    size_t len=limg0.Size(0)/3;

    size_t dims[2]={len*2,limg0.Size(1)};
    kipl::base::TImage<float,2> corrimg(dims);

    for (size_t y=0; y<limg0.Size(1); y++) {
        float *corr=corrimg.GetLinePtr(y);
        const float * const p0   = limg0.GetLinePtr(y)+len;
        const float * const p180 = limg180.GetLinePtr(y);

        for (size_t idx=0; idx<2*len; idx++ ) {
            corr[idx]=0.0f;
            for (size_t x=0; x<len; x++) {
                corr[idx]+=p0[x]*p180[idx+x];
            }
        }
        size_t pos=0;
        for (size_t i=1; i<2*len; i++)
            if (corr[pos]<corr[i]) pos=i;
        float value=(static_cast<float>(pos)-static_cast<float>(len))*0.5f+(static_cast<float>(limg0.Size(0))*0.5f);

        cogfile<<y<<" "<<len<<" "<<limg0.Size(0)<<" "<<pos<<" "<<value<<std::endl;
        m_vCoG.push_back(value);
    }

    return 0.0f;
}

float TomoCenter::CenterOfGravity(const kipl::base::TImage<float, 2> img,
                                  size_t start,
                                  size_t end)
{

}

float TomoCenter::LeastSquareCenter(kipl::base::TImage<float, 2> proj_0,
                                    kipl::base::TImage<float, 2> proj_180,
                                    size_t *roi)
{
    std::ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"Center estimation using least squared error");

    msg<<"LS center: Current ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    kipl::base::TImage<float,2> limg0,limg180;
    size_t start[2]={roi[0],roi[1]};
    size_t length[2]={roi[2]-roi[0],roi[3]-roi[1]};
    kipl::base::TSubImage<float,2> cropper;

    msg.str("");
    msg<<"x ["<<start[0]<<", "<<length[0]<<"], y ["<<start[1]<<", "<<length[1]<<"]";

    logger(kipl::logging::Logger::LogMessage,msg.str());
    limg0=cropper.Get(proj_0,start,length);
    limg180 = kipl::base::Mirror(cropper.Get(proj_180,start,length),kipl::base::ImageAxisX);

    ofstream cogfile("cog_lsc.txt");

    size_t len=limg0.Size(0)/3;

    size_t dims[2]={len*2,limg0.Size(1)};
    kipl::base::TImage<float,2> corrimg(dims);
    float diff=0.0f;
    for (size_t y=0; y<limg0.Size(1); y++) {
        float *corr=corrimg.GetLinePtr(y);
        const float * const p0   = limg0.GetLinePtr(y)+len;
        const float * const p180 = limg180.GetLinePtr(y);

        for (size_t idx=0; idx<2*len; idx++ ) {
            corr[idx]=0.0f;
            for (size_t x=0; x<len; x++) {
                diff=p0[x]-p180[idx+x];
                corr[idx]+=diff*diff;
            }
        }
        size_t pos=0;
        for (size_t i=1; i<2*len; i++)
            if (corr[i]<corr[pos]) pos=i;
        float value=(static_cast<float>(len)-static_cast<float>(pos))*0.5f+(static_cast<float>(limg0.Size(0))*0.5f);

        cogfile<<y<<" "<<len<<" "<<limg0.Size(0)<<" "<<pos<<" "<<value<<std::endl;

        m_vCoG.push_back(value);
    }

    return 0.0f;
}


}
