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
{}

