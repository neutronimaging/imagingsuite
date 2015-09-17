#ifndef BACKPROJECTORBASE_H
#define BACKPROJECTORBASE_H

#include <string>
#include <list>

#include <logging/logger.h>
#include <base/timage.h>

class BackProjectorBase
{
protected:
    kipl::logging::Logger logger;
    std::string m_sName;

public:
    BackProjectorBase(std::string name = "BackProjectorBase");
    ~BackProjectorBase();

    virtual int backproject(kipl::base::TImage<float,2> &proj, float center, std::list<float> & angles, kipl::base::TImage<float,2> &slice) = 0;

protected:
    int buildMask(float center, size_t *dims);

    list<pair<int, int> > m_Mask;
};

#endif // BACKPROJECTORBASE_H
