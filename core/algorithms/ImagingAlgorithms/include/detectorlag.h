//<LICENSE>

#ifndef DETECTORLAG_H
#define DETECTORLAG_H

#include <logging/logger.h>
#include <base/timage.h>

#include "ImagingAlgorithms_global.h"

namespace ImagingAlgorithms
{
    class IMAGINGALGORITHMSSHARED_EXPORT DetectorLag
    {
        kipl::logging::Logger logger;

    public:
        DetectorLag();
        virtual ~DetectorLag();

        kipl::base::TImage<float, 2> process(const kipl::base::TImage<float, 2> &img);
        void setup(float correctionFactor, int medianKernelSize);

    protected:
        float m_correctionFactor;
        kipl::base::TImage<float, 2> m_prevImage;
        size_t m_nMedianSize;
        bool m_prevImageSet = false;
    };
}

#endif
