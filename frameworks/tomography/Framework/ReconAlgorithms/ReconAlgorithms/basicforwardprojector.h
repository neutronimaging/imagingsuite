#ifndef BASICFORWARDPROJECTOR_H
#define BASICFORWARDPROJECTOR_H

#include "reconalgorithms_global.h"
#include "forwardprojectorbase.h"

class RECONALGORITHMSSHARED_EXPORT BasicForwardProjector : public ForwardProjectorBase
{
public:
    using ForwardProjectorBase::project;
    BasicForwardProjector();

    virtual int project(kipl::base::TImage<float, 2> &slice, std::list<float> &angles, kipl::base::TImage<float, 2> &proj);
};

#endif // BASICFORWARDPROJECTOR_H
