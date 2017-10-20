#ifndef BASICBACKPROJECTOR_H
#define BASICBACKPROJECTOR_H

#include "backprojectorbase.h"

class BasicBackProjector: public BackProjectorBase
{
public:
    BasicBackProjector();
    ~BasicBackProjector();

    virtual int backproject(kipl::base::TImage<float,2> &proj, float center, std::list<float> & angles, kipl::base::TImage<float,2> &slice);
};

#endif // BASICBACKPROJECTOR_H
