#ifndef PLOTCURSOR_H
#define PLOTCURSOR_H
#include "QtAddons_global.h"

#include <QColor>

namespace QtAddons {

class QTADDONSSHARED_EXPORT PlotCursor
{
public:
    enum Orientation {
        Horizontal = 0,
        Vertical   = 1
    };

    PlotCursor();
    PlotCursor(const PlotCursor & c);
    PlotCursor(double pos, QColor color, Orientation o,const QString &label="");
    const PlotCursor & operator=(const PlotCursor & c);
    double m_fPosition;
    QColor m_Color;
    Orientation m_Orientation;
    QString m_sLabel;
};

}


#endif // PLOTCURSOR_H
