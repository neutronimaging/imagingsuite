//<LICENSE>

#include "../include/pixelinfo.h"

namespace ImagingAlgorithms {

PixelInfo::PixelInfo() :
    pos(0),
    value(0.0f),
    weight(1.0f)
{}

PixelInfo::PixelInfo(int nPos,float fValue, float fWeight) :
    pos(nPos),
    value(fValue),
    weight(fWeight)
{}

PixelInfo::PixelInfo(const PixelInfo &info) :
    pos(info.pos),
    value(info.value),
    weight(info.weight)
{}

PixelInfo & PixelInfo::operator=(const PixelInfo & info)
{
    pos   = info.pos;
    value = info.value;
    weight = info.weight;

    return *this;
}

}
