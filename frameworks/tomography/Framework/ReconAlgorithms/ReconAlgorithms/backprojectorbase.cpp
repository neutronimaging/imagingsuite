#include "backprojectorbase.h"

BackProjectorBase::BackProjectorBase(std::string name) :
    logger(name),
    m_sName(name)
{

}

BackProjectorBase::~BackProjectorBase()
{

}

int BackProjectorBase::buildMask(float center, size_t *dims)
{

    return 0;
}
