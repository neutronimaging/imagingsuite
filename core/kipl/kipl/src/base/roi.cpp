#include "../../include/base/roi.h"
#include <sstream>

namespace kipl {
namespace base {

int RectROI::cnt=0;

RectROI::RectROI(size_t *roi) :
    logger("RectROI"),
    id(cnt++),
    roiName("RectROI"),
    dimensions(2)
{
    std::copy(roi,roi+4,coords);
}

RectROI::RectROI(size_t x0, size_t y0, size_t x1, size_t y1) :
    logger("RectROI"),
    id(cnt++),
    roiName("RectROI"),
    dimensions(2)
{
    coords[0]=x0;
    coords[1]=y0;
    coords[2]=x1;
    coords[3]=y1;
}

RectROI::RectROI(const RectROI &roi) :
    logger(roi.roiName),
    id(cnt++),
    roiName(roi.roiName),
    dimensions(roi.dimensions)
{
    std::copy(roi.coords,roi.coords+4,coords);
}

RectROI::~RectROI()
{
    //empty
}

RectROI & RectROI::operator=(const RectROI & roi)
{
    roiName = roi.roiName;
    dimensions =roi.dimensions;
    std::copy(roi.coords,roi.coords+4,coords);

    return *this;
}

int RectROI:: getBox(size_t *roi)
{
    std::copy(coords,coords+4,roi);
    return dimensions;
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
    size_t pos0=str.find_first_of(" ,;\t");

}

std::string RectROI::getName()
{
    return roiName;
}

}

}
