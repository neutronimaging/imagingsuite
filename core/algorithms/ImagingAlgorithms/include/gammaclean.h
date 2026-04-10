//<LICENSE>

#ifndef GAMMACLEAN_H
#define GAMMACLEAN_H
#include "ImagingAlgorithms_global.h"
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <list>


#include <base/timage.h>
#include <strings/miscstring.h>
#include <filters/filter.h>
#include <containers/ArrayBuffer.h>
#include <morphology/morphology.h>
#include <base/kiplenums.h>
#include <math/LUTCollection.h>
#include <logging/logger.h>
#include <interactors/interactionbase.h>
#include <utilities/threadpool.h>

#include <filters/medianfilter.h>

namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT GammaClean
{
    kipl::logging::Logger m_logger;
    kipl::interactors::InteractionBase *m_pInteractor;
    kipl::utilities::ThreadPool *m_threadPool;

public:
    GammaClean(float sigma=0.8f, 
               float th3=25.0f, 
               float th5=100.0f, 
               float th7=400.0f, 
               size_t medsize=3, 
               bool useThreads=false, 
               kipl::interactors::InteractionBase *interactor=nullptr);
    
    ~GammaClean();

    void process(kipl::base::TImage<float,2> & img);
    void process(kipl::base::TImage<float,3> & img);
    
    void configure(float sigma, float th3, float th5, float th7, size_t medsize);

    kipl::base::TImage<float,2> detectionImage() ;
    kipl::base::TImage<unsigned short,2> spotMask() ;

    void setConnectivity(kipl::base::eConnectivity conn = kipl::base::conn8);
    kipl::base::eConnectivity connectivity();
        
    void setLimits(bool bClamp, float fMin, float fMax);
    std::vector<float> clampLimits();
    bool clampActive();
    
    void setCleanInfNan(bool activate);
    bool cleanInfNan();
    void useThreading(bool x);
    bool isThreaded();
    int  numberOfThreads();

private:
    void prepareNeighborhoods(const std::vector<size_t> &dims);
    void medianNeighborhood(float *pImg, float *pRes, ptrdiff_t pos, const std::vector<ptrdiff_t> &ng);
    void buildLoGKernel(float sigma, size_t N);
    
    float m_fSigma;
    float m_fThreshold3;
    float m_fThreshold5;
    float m_fThreshold7;
    size_t m_nMedianSize;
    kipl::base::TImage<unsigned short,2> m_mask;
    kipl::base::TImage<float,2> m_diff;
    std::vector<float> m_logkernel;
    std::vector<size_t> m_logkerneldims;

    std::vector<ptrdiff_t> m_nNG3;
    std::vector<ptrdiff_t> m_nNG5;
    std::vector<ptrdiff_t> m_nNG7;
    ptrdiff_t m_nData;
};

}
#endif // GAMMACLEAN_H
