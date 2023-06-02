//<LICENSE>

#include "../include/detectorlag.h"

#include <filters/medianfilter.h>

namespace ImagingAlgorithms
{
    DetectorLag::DetectorLag() : logger("DetectorLag")
    {
    }

    kipl::base::TImage<float, 2> DetectorLag::process(const kipl::base::TImage<float, 2> &img)
    {
        // No detectorlag correction for the first image
        if(!m_prevImageSet)
        {
            m_prevImageSet = true;
            m_prevImage = img;
            m_prevImage.Clone();
            return img;
        }

        //Initialise median filter for image
        std::vector<size_t> meddims = {m_nMedianSize,m_nMedianSize};
        kipl::filters::TMedianFilter<float, 2> medianFilter(meddims);

        // Use overloaded operator
        // Formula for correction: ACTUAL_IMAGE = PROCESS_IMAGE - PREVIOUS_IMAGE * CORRECTION_FACTOR
        auto actImage = img - medianFilter(m_prevImage) * m_prevImageWeigth;

        m_prevImage = img;
        m_prevImage.Clone();

        return actImage;
    }

    DetectorLag::~DetectorLag()
    {
    }
}