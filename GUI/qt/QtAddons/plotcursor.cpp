#include "plotcursor.h"

namespace QtAddons {

PlotCursor::PlotCursor() :
    m_fPosition(0.0),
    m_Color(QColor("red")),
    m_Orientation(Horizontal)
{}

PlotCursor::PlotCursor(const PlotCursor & c) :
    m_fPosition(c.m_fPosition),
    m_Color(c.m_Color),
    m_Orientation(c.m_Orientation)
{}

PlotCursor::PlotCursor(double pos, QColor color, Orientation o) :
    m_fPosition(pos),
    m_Color(color),
    m_Orientation(o)
{}

const PlotCursor & PlotCursor::operator=(const PlotCursor & c)
{
    m_fPosition=c.m_fPosition;
    m_Color=c.m_Color;
    m_Orientation=c.m_Orientation;

    return *this;
}

}
