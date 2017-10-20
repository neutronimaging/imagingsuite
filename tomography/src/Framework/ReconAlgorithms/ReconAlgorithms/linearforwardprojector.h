#ifndef LINEARFORWARDPROJECTOR_H
#define LINEARFORWARDPROJECTOR_H

#include "forwardprojectorbase.h"

class LinearForwardProjector : public ForwardProjectorBase
{
public:
    LinearForwardProjector();
    ~LinearForwardProjector();

    virtual int project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj);
    virtual int project(kipl::base::TImage<float,3> &slice, std::list<float> & angles, kipl::base::TImage<float,3> &proj);
};

#endif // LINEARFORWARDPROJECTOR_H
