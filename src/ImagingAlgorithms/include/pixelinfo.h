#ifndef PIXELINFO_H
#define PIXELINFO_H
#include "stdafx.h"

namespace ImagingAlgorithms {

class DLL_EXPORT PixelInfo {
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
