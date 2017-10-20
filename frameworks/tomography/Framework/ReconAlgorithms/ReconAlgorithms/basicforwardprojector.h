#ifndef BASICFORWARDPROJECTOR_H
#define BASICFORWARDPROJECTOR_H

#include "forwardprojectorbase.h"

class BasicForwardProjector : public ForwardProjectorBase
{
public:
    BasicForwardProjector();

    virtual int project(kipl::base::TImage<float, 2> &slice, std::list<float> &angles, kipl::base::TImage<float, 2> &proj);
};

#endif // BASICFORWARDPROJECTOR_H
