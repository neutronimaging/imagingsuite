#include "../../include/base/roi.h"
#include <sstream>

#include "../../include/strings/string2array.h"

namespace kipl {
namespace base {

int RectROI::cnt=0;

RectROI::RectROI(size_t *roi, const std::string &lbl) :
    logger("RectROI"),
    id(cnt++),
    roiLabel(lbl),
    dimensions(2)
{
    std::copy(roi,roi+4,coords);
}

RectROI::RectROI(size_t x0, size_t y0, size_t x1, size_t y1, const std::string &lbl) :
    logger("RectROI"),
    id(cnt++),
    roiLabel(lbl),
    dimensions(2)
{
    coords[0]=x0;
    coords[1]=y0;
    coords[2]=x1;
    coords[3]=y1;
}

RectROI::RectROI(const RectROI &roi) :
    logger(roi.roiLabel),
    id(cnt++),
    roiLabel(roi.roiLabel),
    dimensions(roi.dimensions)
{
    std::copy_n(roi.coords,4,coords);
}

RectROI::~RectROI()
{
    //empty
}

const RectROI & RectROI::operator=(const RectROI & roi)
{
    roiLabel = roi.roiLabel;
    dimensions =roi.dimensions;
    std::copy_n(roi.coords,4,coords);

    return *this;
}

int RectROI:: getBox(size_t *roi) const
{
    std::copy(coords,coords+4,roi);
    return dimensions;
}

size_t const * RectROI::box() const
{
    return coords;
}

int RectROI::getDimensions()
{
    return dimensions;
}
int RectROI::getID()
{
    return id;
}

std::string RectROI::toString()
{
    std::ostringstream str;

    str<<coords[0]<<" "<<coords[1]<<" "<<coords[2]<<" "<<coords[3];

    return str.str();
}

void RectROI::fromString(std::string str)
{
    kipl::strings::String2Array(str,coords,4);
}

std::string RectROI::label()
{
    return roiLabel;
}

void RectROI::setLabel(const std::string &lbl)
{
    roiLabel = lbl;
}

}

}
