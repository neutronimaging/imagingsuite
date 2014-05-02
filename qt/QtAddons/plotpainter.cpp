#include "plotpainter.h"
#include <cmath>

namespace QtAddons {
PlotPainter::PlotPainter(QWidget *parent):
    m_pParent(parent),
    logger("PlotPainter")
{
}

PlotPainter::~PlotPainter()
{

}

void PlotPainter::Render(QPainter &painter, int x, int y, int w, int h)
{

}

void PlotPainter::setPlotSettings(const PlotSettings &settings)
{}

void PlotPainter::setCurveData(int id, const QVector<QPointF> &data)
{}

void PlotPainter::setCurveData(int id, float const * const x, float const * const y, const int N)
{}

void PlotPainter::setCurveData(int id, float const * const x, size_t const * const y, const int N)
{}

void PlotPainter::clearCurve(int id)
{}

void PlotPainter::clearAllCurves()
{}

void PlotPainter::setPlotCursor(int id, PlotCursor c)
{}

void PlotPainter::clearPlotCursor(int id)
{}

void PlotPainter::clearAllPlotCursors()
{}

void PlotPainter::zoomIn()
{}

void PlotPainter::zoomOut()
{}

//------------------------------------------------------
PlotSettings::PlotSettings()
{
    minX      = 0.0;
    maxX      = 10.0;
    numXTicks = 4;

    minY      = 0.0;
    maxY      = 10.0;
    numYTicks = 4;
}

PlotSettings::PlotSettings(const PlotSettings & s) :
    minX(s.minX),
    maxX(s.maxX),
    numXTicks(s.numXTicks),
    minY(s.minY),
    maxY(s.maxY),
    numYTicks(s.numYTicks)
{
}

const PlotSettings & PlotSettings::operator= (const PlotSettings &s)
{
    minX      = s.minX;
    maxX      = s.maxX;
    numXTicks = s.numXTicks;
    minY      = s.minY;
    maxY      = s.maxY;
    numYTicks = s.numYTicks;

    return *this;
}

PlotSettings::PlotSettings(const QVector<QPointF> &data)
{
    int j=0;
    minX=data[j].x();
    maxX=data[j].x();
    minY=data[j].y();
    maxY=data[j].y();

    for (j = 1; j < data.count(); j++) {
        minX=std::min(data[j].x(),minX);
        maxX=std::max(data[j].x(),maxX);
        minY=std::min(data[j].y(),minY);
        maxY=std::max(data[j].y(),maxY);
    }

    numXTicks = 4;
    numYTicks = 4;
}

void PlotSettings::scroll(int dx, int dy)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;

    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
}

void PlotSettings::adjust()
{
    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
}

void PlotSettings::adjustAxis(double &min, double &max, int &numTicks)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = std::pow(10.0, std::floor(std::log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numTicks = int(std::ceil(max / step) - std::floor(min / step));
    if (numTicks < MinTicks)
        numTicks = MinTicks;
    min = std::floor(min / step) * step;
    max = std::ceil(max / step) * step;
}

PlotData::PlotData() :
    minX(0.0),
    maxX(0.0),
    minY(0.0),
    maxY(0.0),
    glyph(QtAddons::PlotGlyph_Cross)
{
}

PlotData::PlotData(const PlotData & data) :
    m_data(data.m_data),
    minX(data.minX),
    maxX(data.maxX),
    minY(data.minY),
    maxY(data.maxY),
    glyph(data.glyph)
{

}

PlotData::PlotData(const QVector<QPointF> &datavect, QtAddons::ePlotGlyph gl)
{
    m_data=datavect;

    maxX=minX=m_data[0].x();
    maxY=minY=m_data[0].y();


    for (int i=0; i<m_data.size(); i++) {
        minX=std::min(minX,m_data[i].x());
        maxX=std::max(maxX,m_data[i].x());
        minY=std::min(minY,m_data[i].y());
        maxY=std::max(maxY,m_data[i].y());
    }

    glyph=gl;
}

const PlotData & PlotData::operator=(const PlotData & data)
{
    m_data = data.m_data;
    minX   = data.minX;
    maxX   = data.maxX;
    minY   = data.minY;
    maxY   = data.maxY;
    glyph  = data.glyph;

    return *this;
}

PlotCursor::PlotCursor() :
    m_fPosition(0.0),
    m_Color(QColor("red")),
    m_Orientation(Horizontal)
{}

PlotCursor::PlotCursor(const PlotCursor & c) :
m_fPosition(c.m_fPosition),
m_Color(c.m_Color),
m_Orientation(c.m_Orientation)
{}

PlotCursor::PlotCursor(double pos, QColor color, Orientation o) :
    m_fPosition(pos),
    m_Color(color),
    m_Orientation(o)
{}

const PlotCursor & PlotCursor::operator=(const PlotCursor & c)
{
    m_fPosition=c.m_fPosition;
    m_Color=c.m_Color;
    m_Orientation=c.m_Orientation;

    return *this;
}


}
