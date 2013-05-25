#ifndef QGLYPHS_H
#define QGLYPHS_H

#include <QPainter>

namespace QtAddons {

enum ePlotGlyph {
    PlotGlyph_None,
    PlotGlyph_Cross,
    PlotGlyph_Plus,
    PlotGlyph_Square,
    PlotGlyph_FilledSquare,
    PlotGlyph_Circle,
    PlotGlyph_FilledCircle
};

class QGlyphBase {
public:
    QGlyphBase(int size, ePlotGlyph gtype) : m_nSize(size), m_nSize2(size>>1), m_type(gtype) {};
    QGlyphBase(QGlyphBase & glyph) ;
    virtual ~QGlyphBase() {};

//    QGlyphBase & operator=(QGlyphBase & glyph);

    virtual void Draw(QPainter & painter, int x, int y) = 0;
    ePlotGlyph GlyphType() {return m_type;}
    int Size() { return m_nSize;}
protected:
    int m_nSize;
    int m_nSize2;
    ePlotGlyph m_type;
};

class QGlyphCross : public QGlyphBase
{
public:
    QGlyphCross(int size) : QGlyphBase(size,PlotGlyph_Cross) {};
    virtual ~QGlyphCross() {};

    virtual void Draw(QPainter &painter, int x, int y);
};

class QGlyphPlus : public QGlyphBase
{
public:
    QGlyphPlus(int size) : QGlyphBase(size,PlotGlyph_Plus) {};
    virtual ~QGlyphPlus() {};

    virtual void Draw(QPainter & painter, int x, int y);
};

class QGlyphSquare : public QGlyphBase
{
public:
    QGlyphSquare(int size) : QGlyphBase(size, PlotGlyph_Square) {};
    virtual ~QGlyphSquare() {};

    virtual void Draw(QPainter &painter, int x, int y);
};

QGlyphBase * BuildGlyph(ePlotGlyph glyph, int size);

} /* namespace Gtk_addons */

std::ostream & operator<<(std::ostream &s, QtAddons::ePlotGlyph pg);
#endif // QGLYPHS_H
