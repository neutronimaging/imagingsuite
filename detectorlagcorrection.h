//<LICENSE>

#ifndef DETECTORLAGCORRECTION_H
#define DETECTORLAGCORRECTION_H

#include "unpublishedimagingalgorithms_global.h"

class UNPUBLISHEDIMAGINGALGORITHMSSHARED_EXPORT DetectorLagCorrection
{
public:
    enum LagFunction {
        ExponentialLag,
        DoubleExponentialLag
    };

    DetectorLagCorrection();


};

#endif // DETECTORLAGCORRECTION_H
