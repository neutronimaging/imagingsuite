#ifndef NNFORWARDPROJECTOR_H
#define NNFORWARDPROJECTOR_H

#include "forwardprojectorbase.h"
class NNForwardProjector : public ForwardProjectorBase
{
public:
    NNForwardProjector();
    ~NNForwardProjector();

    virtual int project(kipl::base::TImage<float,2> &slice, std::list<float> & angles, kipl::base::TImage<float,2> &proj);
};

#endif // NNFORWARDPROJECTOR_H
