#ifndef FORWARDPROJECTORBASE_H
#define FORWARDPROJECTORBASE_H

#include <string>
#include <list>

#include <logging/logger.h>
#include <base/timage.h>

class ForwardProjectorBase
{
protected:
    kipl::logging::Logger logger;
    std::string m_sName;

public:
    ForwardProjectorBase(std::string name = "ForwardProjectorBase");

    virtual int project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj) = 0;
    virtual int project(kipl::base::TImage<float,3> &slice, std::list<float> & angles, kipl::base::TImage<float,3> &proj) = 0;

    void setPixelSize(float size) { m_PixelSize=size; }
protected:
    int buildMask(const size_t *dims);

    list<pair<int, int> > m_Mask;
    float m_cx;
    float m_cy;
    float m_PixelSize; // Size of the pixels in the projections

};



#endif // FORWARDPROJECTORBASE_H
