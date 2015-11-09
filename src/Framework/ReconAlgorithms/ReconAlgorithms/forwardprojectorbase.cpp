#include "forwardprojectorbase.h"
#include "reconalgorithmexception.h"

ForwardProjectorBase::ForwardProjectorBase(std::string name) :
    logger(name),
    m_sName(name)
{
}

int ForwardProjectorBase::buildMask(const size_t *dims)
{
    if (dims[0]!=dims[1])
        throw ReconAlgorithmException("Input slice does not have same number of rows as columns");

    m_Mask.clear();

    m_cx=dims[0]/2.0f;
    m_cy=dims[1]/2.0f;

    for (int i=0; i<static_cast<int>(dims[1]); i++) {
        float y=i-m_cy;
        float x=sqrt(m_cx*m_cx-y*y);
        m_Mask.push_back(std::make_pair(ceil(m_cx-x),floor(m_cx+x)));
    }

    return 0;
}
