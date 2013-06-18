#ifndef PLOTTER_H
#define PLOTTER_H

#include <QMap>
#include <QPixmap>
#include <QVector>
#include <QWidget>
#include <logging/logger.h>
#include "qglyphs.h"

class QToolButton;


namespace QtAddons {
class PlotSettings;
class PlotData;
class PlotCursor;

class Plotter : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    Plotter(QWidget *parent = 0);

    void setPlotSettings(const PlotSettings &settings);
    void setCurveData(int id, const QVector<QPointF> &data);    
    void clearCurve(int id);
    void clearAllCurves();

    void setPlotCursor(int id, PlotCursor c);
    void clearPlotCursor(int id);
    void clearAllPlotCursors();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    bool ShowGrid;

public slots:
    void zoomIn();
    void zoomOut();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void updateRubberBandRegion();
    void refreshBounds();
    void refreshPixmap();
    void drawGrid(QPainter *painter);
    void drawCurves(QPainter *painter);
    void drawCursors(QPainter *painter);

    enum { Margin = 30 };

    QToolButton *zoomInButton;
    QToolButton *zoomOutButton;
    QMap<int, PlotData > curveMap;
    QMap<int, PlotCursor > cursorMap;
    QVector<PlotSettings> zoomStack;
    int curZoom;
    bool rubberBandIsShown;
    QRect rubberBandRect;
    QPixmap pixmap;
};

class PlotSettings
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

class PlotData
{
public :
    PlotData();
    PlotData(const PlotData & data);
    PlotData(const QVector<QPointF>  & datavect, ePlotGlyph gl=PlotGlyph_Square);
    const PlotData & operator=(const PlotData & data);
    QVector<QPointF> m_data;

    double minX;
    double maxX;
    double minY;
    double maxY;
    ePlotGlyph glyph;
};

class PlotCursor
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

#endif
