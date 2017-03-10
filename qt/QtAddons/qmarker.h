#ifndef QMARKER_H
#define QMARKER_H

#include <QPainter>
#include <QPoint>
#include <QColor>

#include <logging/logger.h>

#include "qglyphs.h"

namespace QtAddons {

class QMarker
{
    kipl::logging::Logger logger;
public:
    QMarker();
    QMarker(ePlotGlyph g, QPointF p, QColor c, int size=10);
    QMarker(const QMarker &m);

    const QMarker & operator=(const QMarker &m);
    const QMarker & operator=(const QMarker &m) const;

    ~QMarker();

    void Draw(QPainter &painter, float scale=1.0f, QPoint offset=QPoint(0,0));

    const QGlyphBase * getGlyph() const {return glyph;}
    QPointF getPosition() const {return position;}
    QColor getColor() const {return color;}
private:
    QGlyphBase *glyph;
    QPointF position;
    QColor color;
};

}
#endif // QMARKER_H
