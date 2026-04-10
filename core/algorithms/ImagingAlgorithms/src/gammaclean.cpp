//<LICENSE>

#include "../include/gammaclean.h"
#include <sstream>
#include <list>
#include <algorithm>
#include <array>
#include <vector>

#include <filters/laplacianofgaussian.h>
#include <filters/medianfilter.h>
#include <math/median.h>

#include "../include/ImagingException.h"

namespace ImagingAlgorithms {

        
//     void process(kipl::base::TImage<float,2> & img);
//     void process(kipl::base::TImage<float,3> & img);
    
//     void configure(float sigma, float th3, float th5, float th7, size_t medsize);

//     kipl::base::TImage<float,2> DetectionImage(kipl::base::TImage<float,2> &img);

//     void setConnectivity(kipl::base::eConnectivity conn = kipl::base::conn8);
//     kipl::base::eConnectivity connectivity();
        
//     void setLimits(bool bClamp, float fMin, float fMax);
//     std::vector<float> clampLimits();
//     bool clampActive();
    
//     void setCleanInfNan(bool activate);
//     bool cleanInfNan();
//     void useThreading(bool x);
//     bool isThreaded();
//     int  numberOfThreads();

// private:
//     void prepareNeighborhoods(const std::vector<size_t> &dims);
//     void medianNeighborhood(float *pImg, float *pRes, ptrdiff_t pos, ptrdiff_t *ng, size_t N);
//     void laplacianOfGaussian(float *pImg, float *pRes, ptrdiff_t pos, ptrdiff_t *ng, size_t N);

GammaClean::GammaClean( float sigma, 
                        float th3, 
                        float th5, 
                        float th7, 
                        size_t medsize, 
                        bool useThreads, 
                        kipl::interactors::InteractionBase *interactor) :
    m_logger("GammaClean"),
    m_pInteractor(interactor),
    m_threadPool(nullptr),
    m_fSigma(sigma),
    m_fThreshold3(th3),
    m_fThreshold5(th5),
    m_fThreshold7(th7),
    m_nMedianSize(medsize)
{
    if (useThreads) {
        m_threadPool = new kipl::utilities::ThreadPool(std::thread::hardware_concurrency());
    }   

    // size_t N=static_cast<size_t>(std::ceil(6*sigma));
    size_t N=9UL;
    buildLoGKernel(sigma, static_cast<size_t>(N)); // The kernel size is set to 6*sigma to capture the significant part of the Gaussian. This is a common choice for LoG filters.
}

GammaClean::~GammaClean()
{
    if (m_threadPool) {
        delete m_threadPool;
    }
}

void GammaClean::configure(float sigma, float th3, float th5, float th7, size_t medsize)
{
    m_fSigma       = sigma;
    m_fThreshold3  = th3;
    m_fThreshold5  = th5;
    m_fThreshold7  = th7;
    m_nMedianSize  = medsize;

    // size_t N=static_cast<size_t>(std::ceil(6*sigma));
    size_t N=9UL;

    buildLoGKernel(sigma, static_cast<size_t>(N)); // The kernel size is set to 6*sigma to capture the significant part of the Gaussian. This is a common choice for LoG filters.
}

void GammaClean::process(kipl::base::TImage<float,3> & /*img*/)
{
    throw ImagingException("3D processing not implemented yet",__FILE__,__LINE__);
}

void GammaClean::process(kipl::base::TImage<float,2> & img)
{
    std::stringstream msg;

    m_nData=static_cast<ptrdiff_t>(img.Size());
    msg.str(""); msg<<"Starting GammaClean with parameters: sigma="<<m_fSigma<<", th3="<<m_fThreshold3<<", th5="<<m_fThreshold5<<", th7="<<m_fThreshold7<<", median size="<<m_nMedianSize;
    m_logger.message(msg.str());
    // kipl::base::TImage<float,2> LoG=kipl::filters::LaplacianOfGaussian(img,m_fSigma);
    msg.str(""); msg<<"kernel size: "<<m_logkerneldims[0]<<"x"<<m_logkerneldims[1] <<" (sigma="<<m_fSigma<<")"<<", kernel vector size: "<<m_logkernel.size();
    m_logger.message(msg.str());
    kipl::filters::TFilter<float,2UL> logFilter(m_logkernel,m_logkerneldims);
    kipl::base::TImage<float,2UL> LoG=logFilter(img,kipl::filters::FilterBase::EdgeMirror);

    std::vector<size_t>  meddims={m_nMedianSize,m_nMedianSize};
    kipl::filters::TMedianFilter<float,2> med3(meddims);

    kipl::base::TImage<float,2> LoGm3=med3(LoG);

    m_diff.resize(img.dims()); 
    m_mask.resize(img.dims()); 

    std::vector<ptrdiff_t> m3;
    std::vector<ptrdiff_t> m5;
    std::vector<ptrdiff_t> m7;

    ptrdiff_t N=static_cast<ptrdiff_t>(img.Size());
    for (ptrdiff_t i=0; i<N; i++) 
    {
        // m_diff[i]=LoG[i]-LoGm3[i];
        m_diff[i]=LoGm3[i]-LoG[i];
        m_mask[i]=(m_fThreshold3<m_diff[i]) + ((m_fThreshold5<m_diff[i])*2) + ((m_fThreshold7<m_diff[i])*4);
        
        switch (static_cast<int>(m_mask[i])) 
        {
            case 0: break;
            case 1: m3.push_back(i); break;
            case 3: m5.push_back(i); break;
            case 7: m7.push_back(i); break;
            default:
                msg.str(""); msg<<"Strange mask code ("<<m_mask[i]<<") found at position"<<i;
                throw ImagingException(msg.str(),__FILE__,__LINE__);
        }
    }

    msg.str(""); msg<<"Gamma spots detected: "<<m3.size()<<" (3x3), "<<m5.size()<<" (5x5), "<<m7.size()<<" (7x7)";
    m_logger.message(msg.str());

    kipl::base::TImage<float,2> res=img;
    res.Clone();

    std::list<ptrdiff_t>::iterator it;
    float *pRes=res.GetDataPtr();
    float *pImg=img.GetDataPtr();


    prepareNeighborhoods(img.dims());
    for (const auto &pos : m3) 
    {
        medianNeighborhood(pImg,pRes,pos,m_nNG3);
    }

    for (const auto &pos : m5) 
    {
        medianNeighborhood(pImg,pRes,pos,m_nNG5);
    }

    for (const auto &pos : m7) 
    {
        medianNeighborhood(pImg,pRes,pos,m_nNG7);
    }

    img=res;
}

void GammaClean::prepareNeighborhoods(const std::vector<size_t> &dims)
{
    m_nNG3.resize(9);
    m_nNG5.resize(25);
    m_nNG7.resize(49);

    for (ptrdiff_t i=-1,idx=0; i<=1; i++)
        for (ptrdiff_t j=-1; j<=1; j++, idx++)
            m_nNG3[idx]=i*dims[0]+j;

    for (ptrdiff_t i=-2,idx=0; i<=2; i++)
        for (ptrdiff_t j=-2; j<=2; j++, idx++)
            m_nNG5[idx]=i*dims[0]+j;

    for (ptrdiff_t i=-3,idx=0; i<=3; i++)
        for (ptrdiff_t j=-3; j<=3; j++, idx++)
            m_nNG7[idx]=i*dims[0]+j;

}

void GammaClean::medianNeighborhood(float *pImg, float *pRes, ptrdiff_t pos, const std::vector<ptrdiff_t> &ng)
{
    std::vector<float> medvec;
    medvec.reserve(ng.size());

    for (const auto &i : ng) 
    {
        ptrdiff_t p=pos+i;

        if ((0<=p) && (p<m_nData)) 
        {
            medvec.push_back(pImg[p]);
        }
    }

    if (medvec.size()==0UL) 
    {
        std::ostringstream msg;
        msg<<"No valid neighborhood pixels found for median calculation at position "<<pos<<", ng size: "<<ng.size()<<", image data size: "<<m_nData;
        throw ImagingException(msg.str(),__FILE__,__LINE__);
    }
    
    pRes[pos]=kipl::math::median_STL(medvec);
    // kipl::math::median(medvec,pRes+pos);
}

void GammaClean::buildLoGKernel(float sigma, size_t N)
{
    ptrdiff_t N2 = N / 2;

    m_logkernel.resize((2*N2+1)*(2*N2+1)); // resize the kernel to the correct size
    
    m_logkerneldims.resize(2);
    m_logkerneldims[0] = 2*N2+1;
    m_logkerneldims[1] = 2*N2+1;

    std::vector<float> g(m_logkernel.size(),0.0f);
    for (ptrdiff_t i=-N2,idx=0; i<=N2; i++)
        for (ptrdiff_t j=-N2; j<=N2; j++, idx++)
            g[idx]=std::exp(-(i * i + j * j) / (2.0f * sigma * sigma));
    
    float sumg = std::accumulate(g.begin(), g.end(), 0.0f);

    for (ptrdiff_t i=-N2,idx=0; i<=N2; i++)
        for (ptrdiff_t j=-N2; j<=N2; j++, idx++)
            m_logkernel[idx] = ((i * i + j * j - 2 * sigma * sigma) * g[idx]) / (2.0f * static_cast<float>(M_PI) * std::pow(sigma,6) * sumg);
}

kipl::base::TImage<float,2> GammaClean::detectionImage() 
{
    return m_diff;
}

kipl::base::TImage<unsigned short,2> GammaClean::spotMask() 
{
    return m_mask;
}

bool GammaClean::isThreaded()
{
    return (m_threadPool!=nullptr);
}

int GammaClean::numberOfThreads()
{
    if (m_threadPool) {
        return m_threadPool->pool_size();
    }
    return 1;
}

}
