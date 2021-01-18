#include "../../include/base/roi.h"
#include <sstream>
#include <vector>

#include "../../include/strings/string2array.h"
#include "../../include/base/KiplException.h"

namespace kipl {
namespace base {

int RectROI::cnt=0;

RectROI::RectROI(const std::vector<size_t> &roi, const std::string &lbl) :
    logger("RectROI"),
    id(cnt++),
    roiLabel(lbl),
    dimensions(2)
{
    if (roi.size()!=4)
        throw kipl::base::KiplException("ROI vector does not have 4 elements",__FILE__,__LINE__);

    coords = roi;
}

RectROI::RectROI(size_t x0, size_t y0, size_t x1, size_t y1, const std::string &lbl) :
    logger("RectROI"),
    id(cnt++),
    roiLabel(lbl),
    dimensions(2),
    coords({x0,y0,x1,y1})
{
}

RectROI::RectROI(const RectROI &roi) :
    logger(roi.roiLabel),
    id(cnt++),
    roiLabel(roi.roiLabel),
    dimensions(roi.dimensions)
{
    coords = roi.coords;
}

RectROI::~RectROI()
{
    //empty
}

const RectROI & RectROI::operator=(const RectROI & roi)
{
    roiLabel = roi.roiLabel;
    dimensions =roi.dimensions;
    coords = roi.coords;

    return *this;
}

int RectROI::getBox(std::vector<size_t> &roi) const
{
    roi = coords;
    return dimensions;
}

const std::vector<size_t> & RectROI::box() const
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

    str<<roiLabel<<" "<<coords[0]<<" "<<coords[1]<<" "<<coords[2]<<" "<<coords[3];

    return str.str();
}

void RectROI::fromString(std::string str)
{
    ptrdiff_t pos=0;
    if (str[0]!=' ')
        pos=str.find_first_not_of(' ');
    ptrdiff_t pos2 = str.find_first_of(' ',pos);
    roiLabel = str.substr(pos,pos2);
    pos=str.find_first_not_of(' ',pos2);
    kipl::strings::String2Array(str.substr(pos),coords,4);
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
