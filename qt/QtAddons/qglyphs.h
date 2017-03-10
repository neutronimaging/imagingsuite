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
    QGlyphBase(int size, ePlotGlyph gtype) : m_nSize(size), m_nSize2(size>>1), m_type(gtype) {}
    QGlyphBase(const QGlyphBase & glyph) ;
    virtual ~QGlyphBase() {}

    const QGlyphBase & operator=(const QGlyphBase & glyph);

    virtual void Draw(QPainter & painter, int x, int y) = 0;
    virtual void Draw(QPainter & painter, QPoint p) = 0;
    ePlotGlyph GlyphType() const {return m_type;}
    int Size() const { return m_nSize;}
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
    virtual void Draw(QPainter & painter, QPoint p);
};

class QGlyphPlus : public QGlyphBase
{
public:
    QGlyphPlus(int size) : QGlyphBase(size,PlotGlyph_Plus) {};
    virtual ~QGlyphPlus() {};

    virtual void Draw(QPainter & painter, int x, int y);
    virtual void Draw(QPainter & painter, QPoint p);
};

class QGlyphSquare : public QGlyphBase
{
public:
    QGlyphSquare(int size) : QGlyphBase(size, PlotGlyph_Square) {};
    virtual ~QGlyphSquare() {};

    virtual void Draw(QPainter &painter, int x, int y);
    virtual void Draw(QPainter & painter, QPoint p);
};

QGlyphBase * BuildGlyph(ePlotGlyph glyph, int size);

} /* namespace Gtk_addons */

std::ostream & operator<<(std::ostream &s, QtAddons::ePlotGlyph pg);
#endif // QGLYPHS_H
