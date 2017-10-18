#include "qmarker.h"
#include <logging/logger.h>

namespace QtAddons {

QMarker::QMarker() :
    logger("QMarker"),
    glyph(nullptr)
{

}

QMarker::QMarker(ePlotGlyph g, QPointF p, QColor c, int size) :
    logger("QMarker"),
    glyph(BuildGlyph(g,size)),
    position(p),
    color(c)
{

}

QMarker::QMarker(const QMarker &m) :
    logger("QMarker"),
    glyph(BuildGlyph(m.glyph->GlyphType(),m.glyph->Size())),
    position(m.position),
    color(m.color)
{
}

const QMarker & QMarker::operator=(const QMarker &m)
{
    if (glyph!=nullptr)
        delete glyph;

    glyph=BuildGlyph(m.glyph->GlyphType(),m.glyph->Size());
    color=m.color;
    position=m.position;

    return *this;
}

 const QMarker & QMarker::operator=(const QMarker &m) const
 {

     return *this;
 }

QMarker::~QMarker()
{
    if (glyph!=nullptr)
        delete glyph;
}

void QMarker::Draw(QPainter &painter,float scale, QPoint offset)
{
    if (glyph!=nullptr) {
        QPen oldpen=painter.pen();
        painter.setPen(color);
        glyph->Draw(painter,offset+(scale*position).toPoint());
        painter.setPen(oldpen);
    }
    else
        logger(logger.LogWarning,"Missing glyph object");
}

}
