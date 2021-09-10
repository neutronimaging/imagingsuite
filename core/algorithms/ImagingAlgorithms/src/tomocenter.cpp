//<LICENSE>
#include "../include/tomocenter.h"

#include <algorithm>
#include <sstream>
#include <vector>
#include <map>
#include <numeric>


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
    mR2(0),
    bSavePoints(false),
    pointsFileName("TomoCenterPoints.csv")
{

}

void TomoCenter::setFraction(double f)
{
    fraction=f;
}

void TomoCenter::estimate(kipl::base::TImage<float, 2> &img0,
                          kipl::base::TImage<float, 2> &img180,
                          ImagingAlgorithms::TomoCenter::eEstimator est,
                          const std::vector<size_t> &_roi,
                          bool bTilt,
                          double &center,
                          double &tilt,
                          double &pivot)
{
    std::ostringstream msg;
    roi = _roi;
    m_Proj0Deg   = img0;
    m_Proj180Deg = img180;

    m_vCoG.clear();
    switch (est)
    {
        case leastSquare:
            LeastSquareCenter();
            break;
        case correlation:
            CorrelationCenter();
            break;
        default:
            CorrelationCenter();
            break;
    }

    size_t N=m_vCoG.size();
    if (bTilt)
    {
        tiltK=1.0;
        tiltM=0.0;

        double *x=new double[N];
        double *y=new double[N];

        for (size_t i=0; i<N; ++i) {
            x[i]=i+roi[1];
            y[i]=m_vCoG[i];
        }

        kipl::math::LinearLSFit(x,y,static_cast<int>(N),&tiltM,&tiltK,&mR2,fraction);
        delete [] x;
        delete [] y;


        double vertical_center=(roi[3]-roi[1])/2;
        center = tiltK*vertical_center+tiltM;
        pivot  = vertical_center;
        tilt   = -atan(tiltK)*180.0/dPi;

        msg.str("");
        msg<<"Estimated center="<<tiltM<<", tilt="<<tiltK<<", N="<<N<<", fraction="<<fraction<<std::endl;
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
    else
    {
        double tmpCenter=std::accumulate(m_vCoG.begin(),m_vCoG.end(),0.0)/m_vCoG.size();
        std::multimap<double,pair<double,double>> cogMap;
        double xpos=0;
        for (auto &item : m_vCoG)
        {
            cogMap.insert(make_pair(fabs(item-tmpCenter),make_pair(xpos,item)));
            xpos+=1.0;
        }
        std::vector<double> tmpCoG;
        auto it=cogMap.begin();

        for (size_t i=0; i<N*fraction; ++i,++it)
            tmpCoG.push_back(it->second.second);

        center=std::accumulate(tmpCoG.begin(),tmpCoG.end(),0.0)/tmpCoG.size();
        tiltM = center;
        tiltK = 0.0;
        computeR2(m_vCoG,tiltK,tiltM);
    }
}

void TomoCenter::estimate(kipl::base::TImage<float,2> &img0,
              kipl::base::TImage<float,2> &img180,
              ImagingAlgorithms::TomoCenter::eEstimator est,
              const std::vector<size_t> &_roi, bool bTilt,
              float &center,
              float &tilt,
              float &pivot)
{
    double dcenter = 0.0;
    double dtilt   = 0.0;
    double dpivot  = 0.0;

    estimate(img0,img180,est,_roi,bTilt,dcenter,dtilt,dpivot);
    center = static_cast<float>(dcenter);
    tilt   = static_cast<float>(dtilt);
    pivot  = static_cast<float>(dpivot);

}

vector<double> TomoCenter::centers()
{
    return m_vCoG;
}

void TomoCenter::tiltParameters(double &k, double &m)
{
    k=tiltK;
    m=tiltM;
}

double TomoCenter::center(double y)
{
    return tiltK*y+tiltM;
}

double TomoCenter::R2()
{
    return mR2;
}

void TomoCenter::savePoints(bool save)
{
    bSavePoints = save;
}

void TomoCenter::setPointsFileName(const string &fname)
{
    pointsFileName = fname;
}

double TomoCenter::computeR2(std::vector<double> &vec,double k, double m)
{
    double sstot=0.0;
    double ssres=0.0;
    double my=std::accumulate(vec.begin(),vec.end(),0.0)/vec.size();;

    double xx=static_cast<double>(roi[1]);
    for (auto & yy: vec)
    {
        sstot += (yy-my)*(yy-my);

        ssres += (yy - (m + k * xx)) * (yy - (m + k * xx));
        xx+=1.0;
    }

    mR2 = 1-ssres/sstot;

    return mR2;
}

float TomoCenter::CorrelationCenter()
{
    std::ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"Center estimation using correlation");

    msg<<"Corr center: Current ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());

    std::ofstream cogfile;

    if (bSavePoints)
        cogfile.open(pointsFileName.c_str());

    kipl::base::TImage<float,2> limg0,limg180;
    std::vector<size_t> start  = {roi[0],roi[1]};
    std::vector<size_t> length = {roi[2]-roi[0],roi[3]-roi[1]};
    kipl::base::TSubImage<float,2> cropper;
    limg0=cropper.Get(m_Proj0Deg,start,length);
    limg180 = kipl::base::Mirror(cropper.Get(m_Proj180Deg,start,length),kipl::base::ImageAxisX);

    size_t len=limg0.Size(0)/3;

    std::vector<size_t> dims={len*2,limg0.Size(1)};
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
        double value=0.5*((static_cast<double>(pos)-static_cast<double>(len))+(static_cast<double>(limg0.Size(0))));

        if (bSavePoints)
            cogfile<<y<<", "<<len<<", "<<limg0.Size(0)<<", "<<pos<<", "<<value<<std::endl;

        m_vCoG.push_back(value);
    }

    return 0.0f;
}

float TomoCenter::CenterOfGravity(const kipl::base::TImage<float, 2> img,
                                  size_t start,
                                  size_t end)
{
    return 0.0f;
}

float TomoCenter::LeastSquareCenter()
{
    std::ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"Center estimation using least squared error");

    msg<<"LS center: Current ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    kipl::base::TImage<float,2> limg0,limg180;
    std::vector<size_t> start  = {roi[0],roi[1]};
    std::vector<size_t> length = {roi[2]-roi[0],roi[3]-roi[1]};
    kipl::base::TSubImage<float,2> cropper;

    msg.str("");
    msg<<"x ["<<start[0]<<", "<<length[0]<<"], y ["<<start[1]<<", "<<length[1]<<"]";

    logger(kipl::logging::Logger::LogMessage,msg.str());
    limg0=cropper.Get(m_Proj0Deg,start,length);
    limg180 = kipl::base::Mirror(cropper.Get(m_Proj180Deg,start,length),kipl::base::ImageAxisX);

    ofstream cogfile;
    if (bSavePoints)
        cogfile.open(pointsFileName.c_str());

    size_t len=limg0.Size(0)/3;

    std::vector<size_t> dims={len*2,limg0.Size(1)};
    kipl::base::TImage<float,2> corrimg(dims);
    float diff=0.0f;
    for (size_t y=0; y<limg0.Size(1); y++)
    {
        float *corr=corrimg.GetLinePtr(y);
        const float * const p0   = limg0.GetLinePtr(y)+len;
        const float * const p180 = limg180.GetLinePtr(y);

        for (size_t idx=0; idx<2*len; idx++ )
        {
            corr[idx]=0.0f;
            for (size_t x=0; x<len; x++)
            {
                diff=p0[x]-p180[idx+x];
                corr[idx]+=diff*diff;
            }
        }

        size_t pos=0;
        for (size_t i=1; i<2*len; i++)
            if (corr[i]<corr[pos]) pos=i;
        double value=0.5*((static_cast<double>(len)-static_cast<double>(pos))+(static_cast<double>(limg0.Size(0))));

        if (bSavePoints)
            cogfile<<y<<", "<<len<<", "<<limg0.Size(0)<<", "<<pos<<", "<<value<<std::endl;

        m_vCoG.push_back(value);
    }

    return 0.0f;
}


}
