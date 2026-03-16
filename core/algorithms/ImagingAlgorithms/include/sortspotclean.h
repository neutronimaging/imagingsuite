#ifndef IMGALG_SORTSPOTCLEAN_H_
#define IMGALG_SORTSPOTCLEAN_H_
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

#include "pixelinfo.h"

namespace ImagingAlgorithms {


enum class eSortSpotQuantile {
    All         = 0,
    BrightSpots = 1,
    DarkSpots   = 2,
    Both        = 3
};

class IMAGINGALGORITHMSSHARED_EXPORT SortSpotClean {
    kipl::logging::Logger m_logger;
public:
    SortSpotClean(bool processPatches=false, size_t patchSize=32, bool useThreads=false, kipl::interactors::InteractionBase *interactor=nullptr);
    ~SortSpotClean();

    void process(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method=eSortSpotQuantile::Both);
    void process(kipl::base::TImage<float,3>& image, float quantile, float threshold, eSortSpotQuantile method=eSortSpotQuantile::Both);
    
    kipl::base::TImage<float,2> getSpotMask() const { return m_mask; }
    kipl::base::TImage<float,2> getDifferenceImage() const { return m_diff; }  

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
    void process2D_single(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method=eSortSpotQuantile::Both);
    void process2D_parallel(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method=eSortSpotQuantile::Both);
    void findAndCleanSpots(kipl::base::TImage<float,2>& image, float threshold, float quantile, eSortSpotQuantile method=eSortSpotQuantile::Both);
    std::vector<size_t> findSpots(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method=eSortSpotQuantile::Both);
    kipl::base::TImage<float,2> createSpotMask(kipl::base::TImage<float,2>& image, const std::vector<size_t>& spotPositions);

    bool   m_bUseThreading;
    int    m_nNumberOfThreads;
    kipl::utilities::ThreadPool *m_threadPool;
    bool   m_processPatches;
    size_t m_patchSize;
    bool  m_bClamp;
    std::vector<float> m_clampLimits;
    bool  m_bCleanInfNan;
    kipl::base::eConnectivity m_connectivity;

    kipl::base::TImage<float,2> m_mask; //!< Mask image indicating the spot positions
    kipl::base::TImage<float,2> m_diff;

    std::atomic_int m_nCounter;
    kipl::interactors::InteractionBase *m_interactor;
};

}  // namespace ImagingAlgorithms

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eSortSpotQuantile esq);
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(const std::string &str,ImagingAlgorithms::eSortSpotQuantile &esq);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream &os, const ImagingAlgorithms::eSortSpotQuantile &esq);

#endif  // IMGALG_SORTSPOTCLEAN_H_