#ifndef BASICBACKPROJECTOR_H
#define BASICBACKPROJECTOR_H

#include "reconalgorithms_global.h"
#include "backprojectorbase.h"

class RECONALGORITHMSSHARED_EXPORT BasicBackProjector: public BackProjectorBase
{
public:
    using BackProjectorBase::backproject;
    
    BasicBackProjector();
    ~BasicBackProjector();

    virtual int backproject(kipl::base::TImage<float,2> &proj, float center, std::list<float> & angles, kipl::base::TImage<float,2> &slice);
};

#endif // BASICBACKPROJECTOR_H
