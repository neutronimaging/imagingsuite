//<LICENSE>

#ifndef PIXELINFO_H
#define PIXELINFO_H
#include "ImagingAlgorithms_global.h"
//#include "stdafx.h"

namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT PixelInfo {
public:
    PixelInfo();
    PixelInfo(int nPos,float fValue, float fWeight);
    PixelInfo(const PixelInfo &info);
    PixelInfo & operator=(const PixelInfo & info);
    int pos;
    float value;
    float weight;
};
}

#endif // PIXELINFO_H
