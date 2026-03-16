#include "../include/sortspotclean.h"
#include "../include/ImagingException.h"

#include <algorithms/sortalg.h>
#include <base/tsubimage.h>
#include <base/kiplenums.h>
#include <base/textractor.h>
#include <math/linfit.h>
#include <math/statistics.h>
#include <morphology/repairhole.h>
#include <iostream>


namespace ImagingAlgorithms {

SortSpotClean::SortSpotClean(bool processPatches, size_t patchSize, bool useThreads, kipl::interactors::InteractionBase *interactor) :
    m_logger("SortSpotClean"),
    m_bUseThreading(useThreads),
    m_nNumberOfThreads(0),
    m_threadPool(nullptr),
    m_processPatches(processPatches),
    m_patchSize(patchSize),
    m_bClamp(false),
    m_clampLimits({0.0f, 0.0f}),
    m_bCleanInfNan(false),
    m_connectivity(kipl::base::eConnectivity::conn8),
    m_nCounter(0),
    m_interactor(interactor)    
{
    useThreading(useThreads);
}

SortSpotClean::~SortSpotClean()
{

}

void SortSpotClean::process(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method)
{
    if (m_processPatches)
    {  
        if (m_bUseThreading && m_threadPool!=nullptr) {
            m_logger.message("Processing 2D image in multi-threaded mode with patch-wise processing.");
            process2D_parallel(image, quantile, threshold, method);
        }
        else {
            m_logger.message("Processing 2D image in single-threaded mode with patch-wise processing.");
            process2D_single(image, quantile, threshold, method);
        }       
    }
    else {
        m_logger.message("Processing entire 2D image in a single-threaded manner.");
        findAndCleanSpots(image, threshold, quantile);
    }
}

void SortSpotClean::process(kipl::base::TImage<float,3>& image, float quantile, float threshold, eSortSpotQuantile method)
{
    if (m_bUseThreading && m_threadPool!=nullptr) 
    {
        auto pImage  = &image;
        m_logger.message("Processing 3D image in multi-threaded mode with patch-wise processing.");
        for (size_t z=0; z<image.Size(2); ++z) 
        {
            m_threadPool->enqueue([this, pImage, threshold, quantile, method, z]() 
            {
                kipl::base::TImage<float,2> slice = kipl::base::ExtractSlice(*pImage, z, kipl::base::eImagePlanes::ImagePlaneXY);

                this->process2D_single(slice, quantile, threshold, method);
                
                kipl::base::InsertSlice(slice, *pImage, z, kipl::base::eImagePlanes::ImagePlaneXY);
                
            });
        }

        m_threadPool->barrier();

    }
    else {
        m_logger.message("Processing 3D image in single-threaded mode with patch-wise processing.");
        for (size_t z=0; z<image.Size(2); ++z) 
        {
            kipl::base::TImage<float,2> slice = kipl::base::ExtractSlice(image, z, kipl::base::eImagePlanes::ImagePlaneXY);
            process2D_single(slice, quantile, threshold, method);
            kipl::base::InsertSlice(slice, image, z, kipl::base::eImagePlanes::ImagePlaneXY);
        }
        
    }
}

void SortSpotClean::process2D_single(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method)
{
    kipl::base::TImage<float,2> result(image.dims());

    kipl::base::ImagePatchExtractor<float,2> extractor(image.dims(), {m_patchSize,m_patchSize}, 2 );

    auto patches = extractor.getAllSubImages();

    for (auto & p : patches)
    {
        auto patch = p.extract(image);
        findAndCleanSpots(patch, threshold, quantile, method);
        p.insert(patch,result);
    }

    image = result;
}

void SortSpotClean::process2D_parallel(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method)
{
    kipl::base::TImage<float,2> result(image.dims());

    kipl::base::ImagePatchExtractor<float,2> extractor(image.dims(), {m_patchSize,m_patchSize}, 2 );

    auto patches = extractor.getAllSubImages();

    auto pImage  = &image;
    auto pResult = &result;
    auto pp      = &patches;

    size_t blockSize = extractor.size() / static_cast<size_t>(m_nNumberOfThreads);
    size_t blockRest = extractor.size() % static_cast<size_t>(m_nNumberOfThreads);
    size_t first = 0UL;
    size_t last  = 0UL;
    
    for (size_t i=0; i<static_cast<size_t>(m_nNumberOfThreads); ++i) 
    {
        last = first + blockSize + (i<blockRest ? 1UL : 0UL);
        // m_logger.message(std::format("Enqueuing thread {} for patches {} to {} (len={})", i, first, last-1, last-first));

        m_threadPool->enqueue([this, pImage, pResult, pp, &threshold, &quantile, &method, first, last]() 
        {
            for (size_t j=first; j<last; ++j)
            {
                auto patch = (*pp)[j].extract(*pImage);
                this->findAndCleanSpots(patch, threshold, quantile, method);
                (*pp)[j].insert(patch,*pResult);
            }            
        });
        first = last;
    }
    m_threadPool->barrier();
    image = result;
}

void SortSpotClean::findAndCleanSpots(kipl::base::TImage<float,2>& image, float threshold, float quantile, eSortSpotQuantile method)
{
    auto spotPositions = findSpots(image, quantile, threshold, method);
    kipl::morphology::RepairHoles(image, spotPositions, kipl::base::eConnectivity::conn8);
    // m_mask = createSpotMask(image, spotPositions);
}

std::vector<size_t> SortSpotClean::findSpots(kipl::base::TImage<float,2>& image, float quantile, float threshold, eSortSpotQuantile method)
{
    std::vector<size_t> spotPositions;

    auto idxSort = kipl::algorithms::arg_sort(image.GetDataPtr(), image.GetDataPtr() + image.Size());

    size_t startIdx = 0UL;
    size_t endIdx   = idxSort.size();

    switch (method)
    {
    case eSortSpotQuantile::All:
        m_logger.warning("Spot detection quantile 'All' specified, all pixels will be used.");
        return spotPositions;
        break;
    case eSortSpotQuantile::BrightSpots:
        startIdx = 0.0f;
        endIdx   = static_cast<size_t>(static_cast<float>(image.Size()) * quantile);

        break;
    case eSortSpotQuantile::DarkSpots:
        // already sorted in ascending order
        break;
    case eSortSpotQuantile::Both:
        startIdx = static_cast<size_t>(static_cast<float>(image.Size()) * 0.5f * (1.0f - quantile));
        endIdx   = static_cast<size_t>(static_cast<float>(image.Size()) * (0.5f + 0.5f * quantile));
    
        break;
    default:
        m_logger.error("Unknown spot detection quantile specified.");
        throw ImagingException("Unknown spot detection quantile specified in SortSpotClean.");
        break;
    }

    size_t N = endIdx - startIdx;

    std::vector<float> x(N);
    std::vector<float> y(N);

    float * pData = image.GetDataPtr();
    for (size_t i = 0UL; i < N; ++i)
    {
        size_t idx = idxSort[i+startIdx];
        float val  = pData[idx];

        x[i] = static_cast<float>(i+startIdx);
        y[i] = val;
    }

    auto fitCoeffs = kipl::math::polyFit(x, y, 1);

    kipl::math::Statistics stats;

    for (size_t i = startIdx; i < endIdx; ++i)
    {
        float fittedVal = fitCoeffs[0] + fitCoeffs[1] * static_cast<float>(i);
        stats.put(std::fabs(pData[idxSort[i]] - fittedVal));
    }    

    float th = stats.s() * threshold;

    for (size_t i = 0UL; i < startIdx; ++i)
    {
        float fittedVal = fitCoeffs[0] + fitCoeffs[1] * static_cast<float>(i);
        if (std::fabs(pData[idxSort[i]] - fittedVal) > th)
        {
            spotPositions.push_back(idxSort[i]);
        }
    }

    for (size_t i = endIdx; i < image.Size(); ++i)
    {
        float fittedVal = fitCoeffs[0] + fitCoeffs[1] * static_cast<float>(i);
        if (std::fabs(pData[idxSort[i]] - fittedVal) > th)
        {
            spotPositions.push_back(idxSort[i]);
        }
    }

    return spotPositions;
}   

kipl::base::TImage<float> SortSpotClean::createSpotMask( kipl::base::TImage<float,2>& image, 
                                                         const std::vector<size_t>& spotPositions)
{
    m_mask.resize(image.dims());
    auto& mask = m_mask;
    mask = 0;
    auto pMaskData = mask.GetDataPtr();
    for (const auto& pos : spotPositions)
    {
        pMaskData[pos] = 1.0f; // Mark spots in the mask
    }

    return mask;
}

void SortSpotClean::setConnectivity(kipl::base::eConnectivity conn)
{
    if (conn != kipl::base::eConnectivity::conn4 &&
        conn != kipl::base::eConnectivity::conn8)
    {
        m_logger.error("Invalid connectivity specified.");
        throw ImagingException("Invalid connectivity specified in SortSpotClean.");
    }

    m_connectivity = conn;
}

kipl::base::eConnectivity SortSpotClean::connectivity()
{
    // Implementation here
    return m_connectivity; // Example return value
}

void SortSpotClean::setLimits(bool bClamp, float fMin, float fMax)
{
    m_bClamp = bClamp;
    m_clampLimits[0] = fMin;
    m_clampLimits[1] = fMax;
}

std::vector<float> SortSpotClean::clampLimits()
{
    // Implementation here
    return m_clampLimits; // Example return value
}

bool SortSpotClean::clampActive()
{
    // Implementation here
    return m_bClamp; // Example return value
}

void SortSpotClean::setCleanInfNan(bool activate)
{
    // Implementation here
    m_bCleanInfNan = activate;
}

bool SortSpotClean::cleanInfNan()
{
    // Implementation here
    return false; // Example return value
}

void SortSpotClean::useThreading(bool x)
{
    m_bUseThreading = x;
    if (m_bUseThreading)
    {
        m_nNumberOfThreads = std::thread::hardware_concurrency();

        if (m_threadPool == nullptr)
        {
            m_threadPool = new kipl::utilities::ThreadPool(m_nNumberOfThreads);
        }

        m_logger.message("Multi-thread processing using thread pool (" + std::to_string(m_threadPool->pool_size()) + " threads) is enabled.");
    }
    else
    {
        m_logger.message("Single-thread processing");
        m_nNumberOfThreads = 1;
        if (m_threadPool != nullptr)
        {
            delete m_threadPool;
            m_threadPool = nullptr;
        }
    }
}

bool SortSpotClean::isThreaded()
{
    return m_bUseThreading; // Example return value
}
    
int SortSpotClean::numberOfThreads()
{
    return m_nNumberOfThreads; // Example return value
}

}

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::eSortSpotQuantile esq)
{
    switch (esq)
    {
    case ImagingAlgorithms::eSortSpotQuantile::All:
        return "All";
    case ImagingAlgorithms::eSortSpotQuantile::BrightSpots:
        return "BrightSpots";
    case ImagingAlgorithms::eSortSpotQuantile::DarkSpots:
        return "DarkSpots";
    case ImagingAlgorithms::eSortSpotQuantile::Both:
        return "Both";
    default:
        return "Unknown";
    }
}
void IMAGINGALGORITHMSSHARED_EXPORT string2enum(const std::string &str,ImagingAlgorithms::eSortSpotQuantile &esq)
{
    if (str == "All")
    {
        esq = ImagingAlgorithms::eSortSpotQuantile::All;
    }
    else if (str == "BrightSpots")
    {
        esq = ImagingAlgorithms::eSortSpotQuantile::BrightSpots;
    }
    else if (str == "DarkSpots")
    {
        esq = ImagingAlgorithms::eSortSpotQuantile::DarkSpots;
    }
    else if (str == "Both")
    {
        esq = ImagingAlgorithms::eSortSpotQuantile::Both;
    }
    else
    {
        throw ImagingException("Unknown eSortSpotQuantile string: " + str,__FILE__,__LINE__);
    }
}   

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream &os, const ImagingAlgorithms::eSortSpotQuantile &esq)
{
    os << enum2string(esq);
    return os;
}
