#include "qglyphs.h"

namespace QtAddons {

//QGlyphBase & QGlyphBase::operator=(QGlyphBase & glyph)
//{

//}


void QGlyphCross::Draw(QPainter &painter, int x, int y)
{
    painter.drawLine(x-m_nSize2, y-m_nSize2, x+m_nSize2, y+m_nSize2);
    painter.drawLine(x-m_nSize2, y+m_nSize2, x+m_nSize2, y-m_nSize2);
}

void QGlyphPlus::Draw(QPainter & painter, int x, int y)
{
    painter.drawLine(x, y-m_nSize2, x, y+m_nSize2);
    painter.drawLine(x-m_nSize2, y, x+m_nSize2, y);
}


void QGlyphSquare::Draw(QPainter &painter, int x, int y)
{
    painter.drawRect(x-m_nSize2,y-m_nSize2,m_nSize,m_nSize);
}

QGlyphBase * BuildGlyph(ePlotGlyph glyph, int size)
{
    switch (glyph) {
    case PlotGlyph_None:         return NULL;
    case PlotGlyph_Cross:        return new QGlyphCross(size);
    case PlotGlyph_Plus:         return new QGlyphPlus(size);
    case PlotGlyph_Square:       return new QGlyphSquare(size);
    case PlotGlyph_FilledSquare: return NULL;
    case PlotGlyph_Circle:       return NULL;
    case PlotGlyph_FilledCircle: return NULL;
    default:                     return NULL;
    }

    return NULL;
}

} // namespace QtAddons

std::ostream & operator<<(std::ostream &s, QtAddons::ePlotGlyph pg)
{
    switch (pg) {
    case QtAddons::PlotGlyph_None:         s<<"PlotGlyph_None";
    case QtAddons::PlotGlyph_Cross:        s<<"PlotGlyph_Cross";
    case QtAddons::PlotGlyph_Plus:         s<<"PlotGlyph_Plot";
    case QtAddons::PlotGlyph_Square:       s<<"PlotGlyph_Square";
    case QtAddons::PlotGlyph_FilledSquare: s<<"PlotGlyph_FilleSquare";
    case QtAddons::PlotGlyph_Circle:       s<<"PlotGlyph_Circle";
    case QtAddons::PlotGlyph_FilledCircle: s<<"PlotGlyph_FilledCircle";
    default:                     s<<"PlotGlyph_None";
    }

    return s;

}

