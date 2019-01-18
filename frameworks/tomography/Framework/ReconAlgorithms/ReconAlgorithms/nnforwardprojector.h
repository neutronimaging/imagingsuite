#ifndef NNFORWARDPROJECTOR_H
#define NNFORWARDPROJECTOR_H

#include "reconalgorithms_global.h"
#include "forwardprojectorbase.h"

class RECONALGORITHMSSHARED_EXPORT NNForwardProjector : public ForwardProjectorBase
{
public:
    NNForwardProjector();
    ~NNForwardProjector();

    virtual int project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj);
    virtual int project(kipl::base::TImage<float,3> &slice, std::list<float> & angles, kipl::base::TImage<float,3> &proj);
};

#endif // NNFORWARDPROJECTOR_H
