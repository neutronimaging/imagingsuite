#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <logging/logger.h>
#include <plotpainter.h>

namespace QtAddons {
class PlotWidget : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit PlotWidget(QWidget *parent = 0);
    void setPlotSettings(const PlotSettings &settings);
    void setCurveData(int id, const QVector<QPointF> &data);
    void setCurveData(int id, float const * const x, float const * const y, const int N);
    void setCurveData(int id, float const * const x, size_t const * const y, const int N);
    void clearCurve(int id);
    void clearAllCurves();

    void setPlotCursor(int id, PlotCursor c);
    void clearPlotCursor(int id);
    void clearAllPlotCursors();

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

protected:
    void updateRubberBandRegion();
    void refreshPixmap();

    enum { Margin = 30 };

    QToolButton *zoomInButton;
    QToolButton *zoomOutButton;

    int curZoom;
    bool rubberBandIsShown;
    QRect rubberBandRect;
    QPixmap pixmap;
    PlotPainter m_PlotPainter;
};

}
#endif // PLOTWIDGET_H
