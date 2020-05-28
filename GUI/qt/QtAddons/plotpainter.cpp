#include "plotpainter.h"
#include <cmath>
#include <sstream>

#include <QPalette>

namespace QtAddons {
PlotPainter::PlotPainter(QWidget *parent):
    m_pParent(parent),
    logger("PlotPainter"),
    m_Font(QFont("Helvetic",parent!=nullptr ? 10 :6 ))
{
    setPlotSettings(PlotSettings());
}

PlotPainter::~PlotPainter()
{

}

void PlotPainter::Render(QPainter &painter, int x, int y, int w, int h)
{
    m_Size.setWidth(w);
    m_Size.setHeight(h);
    m_Pos.setX(x);
    m_Pos.setY(y);
    painter.setFont(m_Font);

    drawGrid(&painter);
    drawCurves(&painter);
    drawCursors(&painter);
}

void PlotPainter::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.clear();
    zoomStack.append(settings);
    curZoom = 0;
}

void PlotPainter::setCurveData(int id, const QVector<QPointF> &data)
{
    curveMap[id] = PlotData(data);
    refreshBounds();
}

void PlotPainter::setCurveData(int id, float const * const x, float const * const y, const int N)
{
    QVector<QPointF> data;

    for (int i=0; i<N; i++)
        data.append(QPointF(x[i],y[i]));

    setCurveData(id,data);
}

void PlotPainter::setCurveData(int id, float const * const x, size_t const * const y, const int N)
{
    QVector<QPointF> data;

    for (int i=0; i<N; i++)
        data.append(QPointF(x[i],static_cast<float>(y[i])));

    setCurveData(id,data);
}

void PlotPainter::clearCurve(int id)
{
    curveMap.remove(id);
    refreshBounds();
}

void PlotPainter::clearAllCurves()
{
    curveMap.clear();
    refreshBounds();
}

void PlotPainter::setPlotCursor(int id, PlotCursor c)
{
    cursorMap[id]=c;
}

void PlotPainter::clearPlotCursor(int id)
{
    cursorMap.remove(id);
}

void PlotPainter::clearAllPlotCursors()
{
    cursorMap.clear();
}

void PlotPainter::zoomIn()
{}

void PlotPainter::zoomOut()
{}

void PlotPainter::refreshBounds()
{
    std::ostringstream msg;

    PlotSettings settings;

    QMap<int, PlotData>::iterator it=curveMap.begin();

    settings.minX=it.value().minX;
    settings.maxX=it.value().maxX;
    settings.minY=it.value().minY;
    settings.maxY=it.value().maxY;

    for (it=curveMap.begin(); it!=curveMap.end(); it++){
        settings.minX=std::min(settings.minX,it.value().minX);
        settings.maxX=std::max(settings.maxX,it.value().maxX);
        settings.minY=std::min(settings.minY,it.value().minY);
        settings.maxY=std::max(settings.maxY,it.value().maxY);
    }

    if (!zoomStack.empty())
        zoomStack.clear();
    zoomStack.append(settings);
    curZoom = 0;
}

void PlotPainter::drawGrid(QPainter *painter)
{
    QRect rect(m_Pos.x()+Margin, m_Pos.y()+Margin,
               m_Size.width() - 2 * Margin, m_Size.height() - 2 * Margin);
    if (!rect.isValid())
        return;

    PlotSettings settings = zoomStack[curZoom];
    QPalette palette;
#ifdef _MSC_VER
    QPen quiteDark;// = palette.dark().color().light();
#else
    QPen quiteDark = palette.dark().color().lighter();
#endif
    QPen light;
    QPen black;
    quiteDark.setColor(QColor(32,32,32));
    black.setColor(QColor(0,0,0));
    light.setColor(QColor(224,224,224));


    for (int i = 0; i <= settings.numXTicks; ++i) {
        int x = rect.left() + (i * (rect.width() - 1)
                                 / settings.numXTicks);
        double label = settings.minX + (i * settings.spanX()
                                          / settings.numXTicks);
        if (m_bShowGrid) {
            painter->setPen(quiteDark);
            painter->drawLine(x, rect.top(), x, rect.bottom());
        }
        painter->setPen(quiteDark);
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        painter->setPen(black);
        painter->drawText(x - 50, rect.bottom() + 3, 100, 20,
                          Qt::AlignHCenter | Qt::AlignTop,
                          QString::number(label,'g',2));

    }
    for (int j = 0; j <= settings.numYTicks; ++j) {
        int y = rect.bottom() - (j * (rect.height() - 1)
                                   / settings.numYTicks);
        double label = settings.minY + (j * settings.spanY()
                                          / settings.numYTicks);
        if (m_bShowGrid) {
            painter->setPen(quiteDark);
            painter->drawLine(rect.left(), y, rect.right(), y);
        }
        painter->setPen(quiteDark);
        painter->drawLine(rect.left() - 3, y, rect.left(), y);
        painter->setPen(black);
        painter->drawText(rect.left() - Margin, y - 10, Margin - 5, 20,
                          Qt::AlignRight | Qt::AlignVCenter,
                          QString::number(label,'g',2));
    }
    painter->setPen(quiteDark);
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
}

void PlotPainter::drawCurves(QPainter *painter)
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };
    PlotSettings settings = zoomStack[curZoom];
    QRect rect(m_Pos.x()+Margin, m_Pos.y()+Margin,
               m_Size.width() - 2 * Margin, m_Size.height() - 2 * Margin);
    if (!rect.isValid())
        return;

    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

    QMapIterator<int, PlotData > i(curveMap);

    while (i.hasNext()) {
        i.next();

        int id = i.key();
        QVector<QPointF> data = i.value().m_data;

        QGlyphBase *pGlyph=BuildGlyph(i.value().glyph,static_cast<int>(m_Font.pointSize()*0.75));

        QPolygonF polyline(data.count());
        painter->setPen(colorForIds[uint(id) % 6]);
        for (int j = 0; j < data.count(); ++j) {
            double dx = data[j].x() - settings.minX;
            double dy = data[j].y() - settings.minY;
            double x = rect.left() + (dx * (rect.width() - 1)
                                         / settings.spanX());
            double y = rect.bottom() - (dy * (rect.height() - 1)
                                           / settings.spanY());
            polyline[j] = QPointF(x, y);
            if (pGlyph)
                pGlyph->Draw(*painter,x,y);
        }

        painter->drawPolyline(polyline);

    }
}

void PlotPainter::drawCursors(QPainter *painter)
{
    std::ostringstream msg;

    if (!cursorMap.empty()) {
        PlotSettings settings = zoomStack[curZoom];
        QRect rect(m_Pos.x()+Margin, m_Pos.y()+Margin,
                   m_Size.width() - 2 * Margin, m_Size.height() - 2 * Margin);
        if (!rect.isValid())
            return;

        painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

        QMapIterator<int, PlotCursor > i(cursorMap);

        while (i.hasNext()) {
            i.next();
            PlotCursor cur=i.value();

            msg.str("");
            double position=cur.m_fPosition;
            painter->setPen(QColor(cur.m_Color));
            if (cur.m_Orientation==PlotCursor::Horizontal) {
                if (position<settings.minY)  {
                    painter->setPen(Qt::DashLine);
                    position=settings.minY;
                }

                if (settings.maxY<position) {
                    painter->setPen(Qt::DashLine);
                    position=settings.maxY;
                }
                painter->drawLine(rect.left(),
                                 int(rect.bottom() - 1 - ((position-settings.minY) * (rect.height() - 2)
                                                     / settings.spanY())),
                                 rect.right(),
                                 int(rect.bottom() - 1 - ((position -settings.minY) * (rect.height() - 2)
                                                                                              / settings.spanY())));
            }
            else {
                if (position<settings.minX)  {
                    painter->setPen(Qt::DashLine);
                    position=settings.minX;
                }

                if (settings.maxX<position) {
                    painter->setPen(Qt::DashLine);
                    position=settings.maxX;
                }

                painter->drawLine(rect.left() + 1+ int((position - settings.minX) * (rect.width() - 3)
                                                / settings.spanX()),
                                 rect.bottom() ,
                                 rect.left() + 1+ int((position - settings.minX) * (rect.width() - 3)
                                                              / settings.spanX()),
                                 rect.top());
                painter->setPen(Qt::SolidLine);
            }
        }
    }
}

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
    color(QColor("blue")),
    glyph(QtAddons::PlotGlyph_Cross)
{
}

PlotData::PlotData(const PlotData & data) :
    m_data(data.m_data),
    minX(data.minX),
    maxX(data.maxX),
    minY(data.minY),
    maxY(data.maxY),
    color(data.color),
    glyph(data.glyph)
{

}

PlotData::PlotData(const QVector<QPointF> &datavect, QColor col, QtAddons::ePlotGlyph gl)
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
    color=col;
    glyph=gl;
}

const PlotData & PlotData::operator=(const PlotData & data)
{
    m_data = data.m_data;
    minX   = data.minX;
    maxX   = data.maxX;
    minY   = data.minY;
    maxY   = data.maxY;
    color  = data.color;
    glyph  = data.glyph;

    return *this;
}



}
