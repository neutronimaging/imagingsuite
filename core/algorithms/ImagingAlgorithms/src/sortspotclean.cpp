#include "../include/sortspotclean.h"

#include <algorithms/sortalg.h>
#include <base/tsubimage.h>
#include <base/kiplenums.h>
#include <math/linfit.h>
#include <math/statistics.h>
#include <morphology/repairhole.h>
#include <iostream>


namespace ImagingAlgorithms {

SortSpotClean::SortSpotClean(bool processPatches, size_t patchSize, bool useThreads) :
    m_logger("SortSpotClean"),
    m_processPatches(processPatches),
    m_useThreads(useThreads),
     m_patchSize(patchSize)
{
}

SortSpotClean::~SortSpotClean()
{

}

void SortSpotClean::process(kipl::base::TImage<float,2>& image, float quantile, float threshold)
{
    if (m_processPatches)
    {  
        kipl::base::TImage<float,2> result(image.dims());
        // Add patch-wise processing here
        kipl::base::ImagePatchExtractor<float,2> extractor(image.dims(), {m_patchSize,m_patchSize}, 0);

        auto patches = extractor.getAllSubImages();
        for (auto & p : patches)
        {
            // auto sub = extractor[p];

            auto patch = p.extract(image);
            findAndCleanSpots(patch, threshold, quantile);
            p.insert(patch,result);
        }

        image = result;
    }
    else
        findAndCleanSpots(image, threshold, quantile);
}

void SortSpotClean::findAndCleanSpots(kipl::base::TImage<float,2>& image, float threshold, float quantile)
{
    auto spotPositions = findSpots(image, quantile, threshold);
    kipl::morphology::RepairHoles(image, spotPositions, kipl::base::eConnectivity::conn8);
    m_mask = createSpotMask(image, spotPositions);
}

std::vector<size_t> SortSpotClean::findSpots(kipl::base::TImage<float,2>& image, float quantile, float threshold)
{
    std::vector<size_t> spotPositions;

    auto idxSort = kipl::algorithms::arg_sort(image.GetDataPtr(), image.GetDataPtr() + image.Size());

    size_t startIdx = static_cast<size_t>(static_cast<float>(image.Size()) * 0.5f * (1.0f - quantile));
    size_t endIdx   = static_cast<size_t>(static_cast<float>(image.Size()) * (0.5f + 0.5f * quantile));
    size_t N= endIdx - startIdx;
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
    
    m_diff.resize(image.dims());

    float th = stats.s() * threshold;
    // std::cout<<"Spot detection threshold: " << th <<"\n"<<stats<< std::endl;

    for (size_t i = 0UL; i < startIdx; ++i)
    {
        float fittedVal = fitCoeffs[0] + fitCoeffs[1] * static_cast<float>(i);
        if (std::fabs(pData[idxSort[i]] - fittedVal) > th)
        {
            spotPositions.push_back(idxSort[i]);
        }
        m_diff[idxSort[i]] = pData[idxSort[i]] - fittedVal;
    }

    for (size_t i = endIdx; i < image.Size(); ++i)
    {
        float fittedVal = fitCoeffs[0] + fitCoeffs[1] * static_cast<float>(i);
        if (std::fabs(pData[idxSort[i]] - fittedVal) > th)
        {
            spotPositions.push_back(idxSort[i]);
        }
        m_diff[idxSort[i]] = pData[idxSort[i]] - fittedVal;
    }

    // std::cout<<"Detected " << spotPositions.size() << " spots." << std::endl;
    return spotPositions;
}   

kipl::base::TImage<float> SortSpotClean::createSpotMask(        kipl::base::TImage<float,2>& image, 
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

}