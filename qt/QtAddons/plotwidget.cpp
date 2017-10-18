#include "plotwidget.h"
namespace QtAddons {

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    logger("PlotWidget")
{
}

void PlotWidget::setPlotSettings(const PlotSettings &settings)
{
    m_PlotPainter.setPlotSettings(settings);
}

void PlotWidget::setCurveData(int id, const QVector<QPointF> &data)
{
    m_PlotPainter.setCurveData(id,data);
}

void PlotWidget::setCurveData(int id, float const * const x, float const * const y, const int N)
{
    m_PlotPainter.setCurveData(id,x,y,N);
}

void PlotWidget::setCurveData(int id, float const * const x, size_t const * const y, const int N)
{
    m_PlotPainter.setCurveData(id,x,y,N);
}

void PlotWidget::clearCurve(int id)
{
    m_PlotPainter.clearCurve(id);
}

void PlotWidget::clearAllCurves()
{
    m_PlotPainter.clearAllCurves();
}

void PlotWidget::setPlotCursor(int id, PlotCursor c)
{
    m_PlotPainter.setPlotCursor(id,c);
}

void PlotWidget::clearPlotCursor(int id)
{
    m_PlotPainter.clearPlotCursor(id);
}

void PlotWidget::clearAllPlotCursors()
{
    m_PlotPainter.clearAllPlotCursors();
}

void PlotWidget::paintEvent(QPaintEvent *event)
{}

void PlotWidget::resizeEvent(QResizeEvent *event)
{}

void PlotWidget::mousePressEvent(QMouseEvent *event)
{}

void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{}

void PlotWidget::mouseReleaseEvent(QMouseEvent *event)
{}

void PlotWidget::keyPressEvent(QKeyEvent *event)
{}

void PlotWidget::wheelEvent(QWheelEvent *event)
{}

void PlotWidget::zoomIn()
{}

void PlotWidget::zoomOut()
{}

}
