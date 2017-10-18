#ifndef PLOTPAINTER_H
#define PLOTPAINTER_H

#include "QtAddons_global.h"
#include <QVector>
#include <QMap>
#include <logging/logger.h>
#include "qglyphs.h"

class QWidget;
//struct QPointF;
class QPainter;
class QPixmap;
class QColor;

namespace QtAddons {
class PlotData;
class PlotSettings;
class PlotCursor;

class QTADDONSSHARED_EXPORT PlotPainter
{
    QWidget * m_pParent;
    kipl::logging::Logger logger;
public:
    PlotPainter(QWidget *parent=NULL);
    ~PlotPainter();
    void Render(QPainter &painter, int x, int y, int w, int h);

    void setPlotSettings(const PlotSettings &settings);
    void setCurveData(int id, const QVector<QPointF> &data);
    void setCurveData(int id, float const * const x, float const * const y, const int N);
    void setCurveData(int id, float const * const x, size_t const * const y, const int N);
    void clearCurve(int id);
    void clearAllCurves();

    void setPlotCursor(int id, PlotCursor c);
    void clearPlotCursor(int id);
    void clearAllPlotCursors();

    void zoomIn();
    void zoomOut();
    enum { Margin = 30 };
    bool m_bShowGrid;
    QFont m_Font;

private:
    void refreshBounds();

    void drawGrid(QPainter *painter);
    void drawCurves(QPainter *painter);
    void drawCursors(QPainter *painter);

    QMap<int, PlotData > curveMap;
    QMap<int, PlotCursor > cursorMap;
    QVector<PlotSettings> zoomStack;
    int curZoom;

    QSize m_Size;
    QPoint m_Pos;

};

class QTADDONSSHARED_EXPORT PlotSettings
{
public:
    PlotSettings();
    PlotSettings(const PlotSettings &s);
    PlotSettings(const QVector<QPointF> &data);

    const PlotSettings & operator= (const PlotSettings &s);
    void scroll(int dx, int dy);
    void adjust();
    double spanX() const { return maxX - minX; }
    double spanY() const { return maxY - minY; }

    double minX;
    double maxX;
    int numXTicks;
    double minY;
    double maxY;
    int numYTicks;

    private:
    static void adjustAxis(double &min, double &max, int &numTicks);
};

class QTADDONSSHARED_EXPORT PlotData
{
public :
    PlotData();
    PlotData(const PlotData & data);
    PlotData(const QVector<QPointF>  & datavect, QColor color=QColor("blue"), QtAddons::ePlotGlyph gl=QtAddons::PlotGlyph_None);
    const PlotData & operator=(const PlotData & data);
    QVector<QPointF> m_data;

    double minX;
    double maxX;
    double minY;
    double maxY;
    QColor color;
    QtAddons::ePlotGlyph glyph;
};

class QTADDONSSHARED_EXPORT PlotCursor
{
public:
    enum Orientation {
        Horizontal = 0,
        Vertical   = 1
    };

    PlotCursor();
    PlotCursor(const PlotCursor & c);
    PlotCursor(double pos, QColor color, Orientation o);
    const PlotCursor & operator=(const PlotCursor & c);
    double m_fPosition;
    QColor m_Color;
    Orientation m_Orientation;
};

}
#endif // PLOTPAINTER_H
