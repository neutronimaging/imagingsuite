#include "qglyphs.h"
#include <sstream>

namespace QtAddons {

const QGlyphBase & QGlyphBase::operator=(const QGlyphBase & glyph)
{
    m_nSize  = glyph.m_nSize;
    m_nSize2 = glyph.m_nSize2;
    m_type   = glyph.m_type;

    return *this;
}

QGlyphBase::QGlyphBase(const QGlyphBase & glyph) :
    m_nSize(glyph.m_nSize),
    m_nSize2(glyph.m_nSize2),
    m_type(glyph.m_type)
{

}

void QGlyphCross::Draw(QPainter &painter, int x, int y)
{
    painter.drawLine(x-m_nSize2, y-m_nSize2, x+m_nSize2, y+m_nSize2);
    painter.drawLine(x-m_nSize2, y+m_nSize2, x+m_nSize2, y-m_nSize2);
}

void QGlyphCross::Draw(QPainter & painter, QPoint p)
{
    Draw(painter,p.x(),p.y());
}

void QGlyphPlus::Draw(QPainter & painter, int x, int y)
{
    painter.drawLine(x, y-m_nSize2, x, y+m_nSize2);
    painter.drawLine(x-m_nSize2, y, x+m_nSize2, y);
}

void QGlyphPlus::Draw(QPainter & painter, QPoint p)
{
    Draw(painter,p.x(),p.y());
}

void QGlyphSquare::Draw(QPainter &painter, int x, int y)
{
    painter.drawRect(x-m_nSize2,y-m_nSize2,m_nSize,m_nSize);
}

void QGlyphSquare::Draw(QPainter & painter, QPoint p)
{
    Draw(painter,p.x(),p.y());
}

QGlyphBase * BuildGlyph(ePlotGlyph glyph, int size)
{
    switch (glyph) {
    case PlotGlyph_None:         return nullptr;
    case PlotGlyph_Cross:        return new QGlyphCross(size);
    case PlotGlyph_Plus:         return new QGlyphPlus(size);
    case PlotGlyph_Square:       return new QGlyphSquare(size);
    case PlotGlyph_FilledSquare: return nullptr;
    case PlotGlyph_Circle:       return nullptr;
    case PlotGlyph_FilledCircle: return nullptr;
    default:                     return nullptr;
    }

    return nullptr;
}

} // namespace QtAddons

std::ostream & operator<<(std::ostream &s, QtAddons::ePlotGlyph pg)
{
    std::string str;
    switch (pg) {
    case QtAddons::PlotGlyph_None:         str="PlotGlyph_None"; break;
    case QtAddons::PlotGlyph_Cross:        str="PlotGlyph_Cross"; break;
    case QtAddons::PlotGlyph_Plus:         str="PlotGlyph_Plot"; break;
    case QtAddons::PlotGlyph_Square:       str="PlotGlyph_Square"; break;
    case QtAddons::PlotGlyph_FilledSquare: str="PlotGlyph_FilleSquare"; break;
    case QtAddons::PlotGlyph_Circle:       str="PlotGlyph_Circle"; break;
    case QtAddons::PlotGlyph_FilledCircle: str="PlotGlyph_FilledCircle"; break;
    default:                               str="PlotGlyph_None"; break;
    }

    s<<str;

    return s;
}

