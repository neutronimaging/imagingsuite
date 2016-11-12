
#include <iostream>
#include "../../include/morphology/pixeliterator.h"
#include "../../include/base/KiplException.h"

namespace kipl {
namespace base {

PixelIterator::PixelIterator(size_t *dims, kipl::base::eConnectivity conn) :
    m_imageSize(dims[0]*dims[1]),
    m_connectivity(conn),
    m_currentPosition(0L),
    m_currentX(0),
    m_currentY(0),
    m_sx(dims[0]),
    m_rowStart(0),
    m_rowEnd(m_sx-1)
{
    m_dims[0]=static_cast<int>(dims[0]); m_dims[1]=static_cast<int>(dims[1]);

    setupNeighborhoods();
    setPosition(0L);
}

PixelIterator::PixelIterator(const PixelIterator &pi) :
    m_imageSize(pi.m_imageSize),
    m_connectivity(pi.m_connectivity),
    m_neighborhoodSize(pi.m_neighborhoodSize),
    m_sx(pi.m_sx)
{
    m_dims[0]=pi.m_dims[0];
    m_dims[1]=pi.m_dims[1];
    memcpy(m_neighborhoodIndex,pi.m_neighborhoodIndex,sizeof(ptrdiff_t)*8);
    memcpy(m_edgeX,pi.m_edgeX,sizeof(ptrdiff_t)*8);
    memcpy(m_edgeY,pi.m_edgeY,sizeof(ptrdiff_t)*8);
    memcpy(m_corner0,pi.m_corner0,sizeof(ptrdiff_t)*8);
    memcpy(m_corner1,pi.m_corner1,sizeof(ptrdiff_t)*8);

    setPosition(pi.m_currentPosition);
}

PixelIterator & PixelIterator::operator=(const PixelIterator &pi)
{
    m_imageSize=pi.m_imageSize;
    m_connectivity=pi.m_connectivity;
    m_neighborhoodSize=pi.m_neighborhoodSize;
    m_dims[0]=pi.m_dims[0];
    m_dims[1]=pi.m_dims[1];
    m_sx=pi.m_sx;

    memcpy(m_neighborhoodIndex,pi.m_neighborhoodIndex,sizeof(ptrdiff_t)*8);
    memcpy(m_edgeX,pi.m_edgeX,sizeof(ptrdiff_t)*8);
    memcpy(m_edgeY,pi.m_edgeY,sizeof(ptrdiff_t)*8);
    memcpy(m_corner0,pi.m_corner0,sizeof(ptrdiff_t)*8);
    memcpy(m_corner1,pi.m_corner1,sizeof(ptrdiff_t)*8);

    return *this;
}

void PixelIterator::setupNeighborhoods()
{
        memset(m_neighborhoodIndex,0,sizeof(ptrdiff_t)*8);
        memset(m_edgeX,0,sizeof(ptrdiff_t)*8);
        memset(m_edgeY,0,sizeof(ptrdiff_t)*8);
        memset(m_corner0,0,sizeof(ptrdiff_t)*8);
        memset(m_corner1,0,sizeof(ptrdiff_t)*8);

        switch (m_connectivity) {
        case kipl::base::conn4 :
            m_neighborhoodSize.full=4;
            m_neighborhoodSize.edgeX0=3;
            m_neighborhoodSize.edgeX1=3;
            m_neighborhoodSize.edgeY0=3;
            m_neighborhoodSize.edgeY1=3;
            m_neighborhoodSize.cornerX0Y0=2;
            m_neighborhoodSize.cornerX1Y0=2;
            m_neighborhoodSize.cornerX0Y1=2;
            m_neighborhoodSize.cornerX1Y1=2;

            m_fwbwSize.full=2;
            m_fwbwSize.edgeX0=2;
            m_fwbwSize.edgeX1=1;
            m_fwbwSize.edgeY0=2;
            m_fwbwSize.edgeY1=1;
            m_fwbwSize.cornerX0Y0=2;
            m_fwbwSize.cornerX1Y0=1;
            m_fwbwSize.cornerX0Y1=1;
            m_fwbwSize.cornerX1Y1=0;

            m_neighborhoodIndex[0] = -m_sx;
            m_neighborhoodIndex[1] = -1;
            m_neighborhoodIndex[2] =  1;
            m_neighborhoodIndex[3] =  m_sx;

            m_edgeX[0] = -m_sx;
            m_edgeX[1] = -1;
            m_edgeX[2] = m_sx;

            m_edgeY[0] = -1;
            m_edgeY[1] =  1;
            m_edgeY[2] =  m_sx;

            m_corner0[0] =  1;
            m_corner0[1] = m_sx;

            m_corner1[0] = -1;
            m_corner1[1] = m_sx;

            break;
        case kipl::base::conn8 :
            m_neighborhoodSize.full=8;
            m_neighborhoodSize.edgeX0=5;
            m_neighborhoodSize.edgeX1=5;
            m_neighborhoodSize.edgeY0=5;
            m_neighborhoodSize.edgeY1=5;
            m_neighborhoodSize.cornerX0Y0=3;
            m_neighborhoodSize.cornerX1Y0=3;
            m_neighborhoodSize.cornerX0Y1=3;
            m_neighborhoodSize.cornerX1Y1=3;

            m_fwbwSize.full=4;
            m_fwbwSize.edgeX0=3;
            m_fwbwSize.edgeX1=2;
            m_fwbwSize.edgeY0=4;
            m_fwbwSize.edgeY1=1;
            m_fwbwSize.cornerX0Y0=3;
            m_fwbwSize.cornerX1Y0=2;
            m_fwbwSize.cornerX0Y1=1;
            m_fwbwSize.cornerX1Y1=0;

            m_neighborhoodIndex[0] = -m_sx-1;
            m_neighborhoodIndex[1] = -m_sx;
            m_neighborhoodIndex[2] = -m_sx+1;
            m_neighborhoodIndex[3] = -1;
            m_neighborhoodIndex[4] =  1;
            m_neighborhoodIndex[5] =  m_sx-1;
            m_neighborhoodIndex[6] =  m_sx;
            m_neighborhoodIndex[7] =  m_sx+1;

            m_edgeX[0] = -m_sx-1;
            m_edgeX[1] = -m_sx;
            m_edgeX[2] = -1;
            m_edgeX[3] = m_sx-1;
            m_edgeX[4] = m_sx;

            m_edgeY[0] = -1;
            m_edgeY[1] =  1;
            m_edgeY[2] =  m_sx-1;
            m_edgeY[3] =  m_sx;
            m_edgeY[4] =  m_sx+1;

            m_corner0[0] =  1;
            m_corner0[1] =  m_sx;
            m_corner0[2] =  m_sx+1;

            m_corner1[0] = -1;
            m_corner1[1] =  m_sx;
            m_corner1[2] =  m_sx-1;

            break;
        default :
            if ((m_connectivity!=kipl::base::conn4) && (m_connectivity!=kipl::base::conn8)) {
                throw kipl::base::KiplException("The pixel-iterator (2D) only supports 4- and 8-connectivity");
            }
        }
}

PixelIterator & PixelIterator::operator++()
{
    setPosition(++m_currentPosition);
    return *this;
}

PixelIterator & PixelIterator::operator--()
{
    setPosition(m_currentPosition--);
    return *this;
}

ptrdiff_t PixelIterator::begin()
{
    return 0;
}

ptrdiff_t PixelIterator::end()
{
    return m_imageSize;
}

ptrdiff_t PixelIterator::operator=(ptrdiff_t pos)
{
    return setPosition(pos);
}

bool PixelIterator::operator==(ptrdiff_t pos)
{
    return m_currentPosition==pos;
}

bool PixelIterator::operator!=(ptrdiff_t pos)
{
    return m_currentPosition!=pos;
}

ptrdiff_t PixelIterator::setPosition(int x, int y)
{
    m_currentX=x;
    m_currentY=y;

    m_rowStart        = y*m_sx;
    m_currentPosition = m_rowStart+x;
    m_rowEnd          = m_rowStart+m_sx-1;

    updateEdgeStatus();

    return m_currentPosition;
}

ptrdiff_t PixelIterator::setPosition(ptrdiff_t pos)
{
    if ((pos<m_rowStart) || (m_rowEnd<pos)) {
        m_currentY=pos/m_sx;
        m_currentX=pos-m_currentY*m_sx;
    }
    else {
        m_currentX+=(pos-m_currentPosition);
    }

    m_currentPosition=pos;
    updateEdgeStatus();

    return m_currentPosition;
}

ptrdiff_t PixelIterator::neighborhood(int idx)
{
    ptrdiff_t pos=0L;
    switch (getEdgeStatus()) {
    case kipl::base::noEdge :     pos = m_neighborhoodIndex[idx]; break;
    case kipl::base::edgeX0 :     pos = m_edgeX[idx]; break;
    case kipl::base::edgeX1 :     pos =-m_edgeX[idx]; break;
    case kipl::base::edgeY0 :     pos = m_edgeY[idx]; break;
    case kipl::base::edgeY1 :     pos =-m_edgeY[idx];   break;
    case kipl::base::cornerX0Y0 : pos = m_corner0[idx]; break;
    case kipl::base::cornerX1Y1 : pos =-m_corner0[idx]; break;
    case kipl::base::cornerX1Y0 : pos = m_corner1[idx]; break;
    case kipl::base::cornerX0Y1 : pos =-m_corner1[idx]; break;
    default : throw kipl::base::KiplException("Unknown edge status in PixelIterator::neighborhood.",__FILE__,__LINE__);
    }

    pos+=m_currentPosition;
    updateEdgeStatus();

    return pos;
}

int PixelIterator::neighborhoodSize()
{
    switch (m_edgeStatus) {
        case kipl::base::noEdge : return m_neighborhoodSize.full; break;
        case kipl::base::edgeX0 : return m_neighborhoodSize.edgeX0; break;
        case kipl::base::edgeX1 : return m_neighborhoodSize.edgeX1; break;
        case kipl::base::edgeY0 : return m_neighborhoodSize.edgeY0; break;
        case kipl::base::edgeY1 : return m_neighborhoodSize.edgeY1; break;
        case kipl::base::cornerX0Y0 : return m_neighborhoodSize.cornerX0Y0; break;
        case kipl::base::cornerX1Y0 : return m_neighborhoodSize.cornerX1Y0; break;
        case kipl::base::cornerX0Y1 : return m_neighborhoodSize.cornerX0Y1; break;
        case kipl::base::cornerX1Y1 : return m_neighborhoodSize.cornerX1Y1; break;
    }

    return 0;
}

/// \brief Returns the size of the forward and backward neighborhoods with the current connectivity
int PixelIterator::forwardSize()
{
    switch (m_edgeStatus) {
        case kipl::base::noEdge : return m_neighborhoodSize.full; break;
        case kipl::base::edgeX0 : return m_connectivity == kipl::base::conn4 ? 2 : 3; break;
        case kipl::base::edgeX1 : return m_connectivity == kipl::base::conn4 ? 1 : 2; break;
        case kipl::base::edgeY0 : return m_connectivity == kipl::base::conn4 ? 2 : 4; break;
        case kipl::base::edgeY1 : return m_connectivity == kipl::base::conn4 ? 1 : 1; break;
        case kipl::base::cornerX0Y0 : return m_connectivity == kipl::base::conn4 ? 2 : 3; break;
        case kipl::base::cornerX1Y0 : return m_connectivity == kipl::base::conn4 ? 1 : 2; break;
        case kipl::base::cornerX0Y1 : return m_connectivity == kipl::base::conn4 ? 1 : 1; break;
        case kipl::base::cornerX1Y1 : return m_connectivity == kipl::base::conn4 ? 0 : 0; break;
    }

    return 0;
}

ptrdiff_t PixelIterator::forwardNeigborhood(int idx)
{
    return 0UL;
}

ptrdiff_t PixelIterator::backwardNeigborhood(int idx)
{
    return 0UL;
}

ptrdiff_t PixelIterator::getCurrentPosition()
{
    return m_currentPosition;
}

kipl::base::eConnectivity PixelIterator::getConnectivity()
{
    return m_connectivity;
}

kipl::base::eEdgeStatus PixelIterator::getEdgeStatus()
{
    return m_edgeStatus;
}

kipl::base::eEdgeStatus PixelIterator::updateEdgeStatus() {

    std::cout<<"Edge status: "<<m_currentPosition<<", "<<m_currentX<<", "<<m_currentY<<std::endl;
    m_edgeStatus= static_cast<kipl::base::eEdgeStatus>(
            (m_currentX==0)                +
            ((m_currentX==(m_dims[0]-1)) << 1) +
            ((m_currentY==0)           << 2) +
            ((m_currentY==(m_dims[1]-1)) << 3) );

    return m_edgeStatus;
}
}}
