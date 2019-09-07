#include "plotcursor.h"

namespace QtAddons {

PlotCursor::PlotCursor() :
    m_fPosition(0.0),
    m_Color(QColor("red")),
    m_Orientation(Horizontal),
    m_sLabel("")
{}

PlotCursor::PlotCursor(const PlotCursor & c) :
    m_fPosition(c.m_fPosition),
    m_Color(c.m_Color),
    m_Orientation(c.m_Orientation),
    m_sLabel(c.m_sLabel)
{}

PlotCursor::PlotCursor(double pos, QColor color, Orientation o, const QString &label) :
    m_fPosition(pos),
    m_Color(color),
    m_Orientation(o),
    m_sLabel(label)
{}

const PlotCursor & PlotCursor::operator=(const PlotCursor & c)
{
    m_fPosition   = c.m_fPosition;
    m_Color       = c.m_Color;
    m_Orientation = c.m_Orientation;
    m_sLabel      = c.m_sLabel;

    return *this;
}

}
