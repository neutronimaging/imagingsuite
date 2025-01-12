//<LICENSE>

#if QT_VERSION > 0x050000
#include <QtGui>
#else
#include <QtWidgets>
#endif

#include <QFileDialog>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <fstream>

#include <strings/filenames.h>

#include "plotter.h"
#include "qglyphs.h"
#include "plotpainter.h"

namespace QtAddons {

Plotter::Plotter(QWidget *parent)
    : QWidget(parent), logger("Plotter"), ShowGrid(false)
{
    setFont(QFont("Helvetic",10));
    setBackgroundRole(QPalette::Dark);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    rubberBandIsShown = false;

    zoomInButton = new QToolButton(this);
    zoomInButton->setText("+");
    zoomInButton->adjustSize();
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));

    zoomOutButton = new QToolButton(this);
    zoomOutButton->setText("-");
    zoomOutButton->adjustSize();
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));

    setPlotSettings(PlotSettings());
}

void Plotter::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.clear();
    zoomStack.append(settings);
    curZoom = 0;
    zoomInButton->hide();
    zoomOutButton->hide();
    refreshPixmap();
}

void Plotter::zoomOut()
{
    if (curZoom > 0) {
        --curZoom;
        zoomOutButton->setEnabled(curZoom > 0);
        zoomInButton->setEnabled(true);
        zoomInButton->show();
        refreshPixmap();
    }
}

void Plotter::zoomIn()
{
    if (curZoom < zoomStack.count() - 1) {
        ++curZoom;
        zoomInButton->setEnabled(curZoom < zoomStack.count() - 1);
        zoomOutButton->setEnabled(true);
        zoomOutButton->show();
        refreshPixmap();
    }
}

void Plotter::setCurveData(int id, const QVector<QPointF> &data, QColor color, ePlotGlyph pg)
{
    curveMap[id] = PlotData(data,color,pg);
    refreshBounds();
    refreshPixmap();
}

void Plotter::setCurveData(int id, const float *const x,const float *const y, const int N,QColor color, ePlotGlyph pg)
{
    QVector<QPointF> data;

    for (int i=0; i<N; i++)
        data.append(QPointF(x[i],y[i]));

    setCurveData(id,data,color,pg);
}

void Plotter::setCurveData(int id, const float *const x, const size_t *const y, const int N, QColor color, ePlotGlyph pg)
{
    QVector<QPointF> data;

    for (int i=0; i<N; i++)
        data.append(QPointF(x[i],static_cast<float>(y[i])));

    setCurveData(id,data,color,pg);
}

void Plotter::refreshBounds()
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
    zoomInButton->hide();
    zoomOutButton->hide();

}

void Plotter::clearCurve(int id)
{
    curveMap.remove(id);
    refreshBounds();
    refreshPixmap();
}

void Plotter::clearAllCurves()
{
    curveMap.clear();
    refreshBounds();
    refreshPixmap();
}

void Plotter::setPlotCursor(int id, PlotCursor c)
{
    cursorMap[id]=c;
    refreshPixmap();
}

void Plotter::clearPlotCursor(int id)
{
    cursorMap.remove(id);
    refreshPixmap();
}

void Plotter::clearAllPlotCursors()
{
    cursorMap.clear();
    refreshPixmap();
}

QSize Plotter::minimumSizeHint() const
{
    return QSize(12 * Margin, 8 * Margin);
}

QSize Plotter::sizeHint() const
{
    return QSize(256, 256);
}

void Plotter::paintEvent(QPaintEvent * /* event */)
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);

    if (rubberBandIsShown) {
        painter.setPen(palette().light().color());
        painter.drawRect(rubberBandRect.normalized()
                                       .adjusted(0, 0, -1, -1));
    }

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().dark().color();
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
}

void Plotter::resizeEvent(QResizeEvent * /* event */)
{
    int x = width() - (zoomInButton->width()
                       + zoomOutButton->width() + 10);
    zoomInButton->move(x, 5);
    zoomOutButton->move(x + zoomInButton->width() + 5, 5);
    refreshPixmap();
}

void Plotter::mousePressEvent(QMouseEvent *event)
{
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);

    if (event->button() == Qt::LeftButton) {
        if (rect.contains(event->pos())) {
            rubberBandIsShown = true;
            rubberBandRect.setTopLeft(event->pos());
            rubberBandRect.setBottomRight(event->pos());
            updateRubberBandRegion();
            setCursor(Qt::CrossCursor);
        }
    }
}

void Plotter::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBandIsShown) {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void Plotter::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && rubberBandIsShown) {
        rubberBandIsShown = false;
        updateRubberBandRegion();
        unsetCursor();

        QRect rect = rubberBandRect.normalized();
        if (rect.width() < 4 || rect.height() < 4)
            return;
        rect.translate(-Margin, -Margin);

        PlotSettings prevSettings = zoomStack[curZoom];
        PlotSettings settings;
        double dx = prevSettings.spanX() / (width() - 2 * Margin);
        double dy = prevSettings.spanY() / (height() - 2 * Margin);
        settings.minX = prevSettings.minX + dx * rect.left();
        settings.maxX = prevSettings.minX + dx * rect.right();
        settings.minY = prevSettings.maxY - dy * rect.bottom();
        settings.maxY = prevSettings.maxY - dy * rect.top();
        settings.adjust();

        zoomStack.resize(curZoom + 1);
        zoomStack.append(settings);
        zoomIn();
    }
}

void Plotter::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Left:
        zoomStack[curZoom].scroll(-1, 0);
        refreshPixmap();
        break;
    case Qt::Key_Right:
        zoomStack[curZoom].scroll(+1, 0);
        refreshPixmap();
        break;
    case Qt::Key_Down:
        zoomStack[curZoom].scroll(0, -1);
        refreshPixmap();
        break;
    case Qt::Key_Up:
        zoomStack[curZoom].scroll(0, +1);
        refreshPixmap();
        break;
    case Qt::Key_S:
        savePlotData();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

//void Plotter::wheelEvent(QWheelEvent *event)
//{
//    auto numDegrees = event->angleDelta() / 8;
//    auto numTicks = numDegrees / 15;

//    if (event->orientation() == Qt::Horizontal) {
//        zoomStack[curZoom].scroll(numTicks, 0);
//    } else {
//        zoomStack[curZoom].scroll(0, numTicks);
//    }
//    refreshPixmap();
//}

void Plotter::savePlotData()
{
    QFileDialog dlg;

    QString fname=dlg.getSaveFileName(this,"Save plot to text file");

    qDebug() <<fname;

    if (fname.isEmpty())
        return ;

    std::string fname2=fname.toStdString();
    std::string path;
    std::string name;
    std::vector<std::string> ext;
    std::string extstr;

    kipl::strings::filenames::StripFileName(fname2,path,name,ext);

    std::ostringstream namemaker;

    if (curveMap.isEmpty()==false) {
        for (auto id: curveMap.keys()) {
            namemaker.str("");
            PlotData data=curveMap.value(id);
            if (ext.empty()==true)
                extstr=".csv";
            else {
                for (auto e: ext) {
                    extstr+=e;
                }
            }

            namemaker<<path<<name<<"_"<<id<<extstr;

            std::ofstream plotfile(namemaker.str().c_str());

            for (auto d :data.m_data) {
                plotfile<<d.x()<<", "<<d.y()<<std::endl;
            }

        }
    }
}

void Plotter::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void Plotter::refreshPixmap()
{
    pixmap = QPixmap(size());
    pixmap.fill(Qt::darkGray);

    QPainter painter(&pixmap);

    painter.begin(this);

    auto fm = painter.fontMetrics();
    auto br=fm.boundingRect("-0.0000");
    leftMargin = br.width();
//    leftMargin = fm.horizontalAdvance("-0.0000",-1);
//    leftMargin = fm.horizontalAdvance('-');

    leftMargin += Margin-10;

    drawGrid(&painter);
    drawCurves(&painter);
    drawCursors(&painter);
    update();
}

void Plotter::drawGrid(QPainter *painter)
{
    QRect rect(leftMargin, Margin,
               width() - Margin - leftMargin, height() - 2*Margin );

    if (!rect.isValid())
        return;

    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark = palette().dark().color().lighter();
    QPen light = palette().light().color();
    QString labelstr;
    // Draw the x-ticks
    for (int i = 0; i <= settings.numXTicks; ++i) {
        int x = rect.left() + (i * (rect.width() - 1)
                                 / settings.numXTicks);
        double label = settings.minX + (i * settings.spanX()
                                          / settings.numXTicks);
        if (ShowGrid) {
            painter->setPen(quiteDark);
            painter->drawLine(x, rect.top(), x, rect.bottom());
        }
        painter->setPen(light);
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        labelstr.clear();
        labelstr.setNum(label,'g',3);
        painter->drawText(x - 50, rect.bottom() + 5, 100, 20,
                          Qt::AlignHCenter | Qt::AlignTop,
                          labelstr);
    }

    // Draw the y-ticks
    for (int j = 0; j <= settings.numYTicks; ++j) {
        int y = rect.bottom() - (j * (rect.height() - 1)
                                   / settings.numYTicks);
        double label = settings.minY + (j * settings.spanY()
                                          / settings.numYTicks);
        if (ShowGrid) {
            painter->setPen(quiteDark);
            painter->drawLine(rect.left(), y, rect.right(), y);
        }
        painter->setPen(light);
        painter->drawLine(rect.left() - 5, y, rect.left(), y);
        labelstr.clear();
        labelstr.setNum(label,'g',3);
        painter->drawText(rect.left() - leftMargin, y - 10, leftMargin - 5, 20,
                          Qt::AlignRight | Qt::AlignVCenter,
                          labelstr);
    }
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
}

void Plotter::drawCurves(QPainter *painter)
{
    PlotSettings settings = zoomStack[curZoom];
    QRect rect(leftMargin, Margin,
               width() - Margin - leftMargin, height() - 2 * Margin);
    if (!rect.isValid())
        return;

    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

    QMapIterator<int, PlotData > i(curveMap);

    while (i.hasNext()) {
        i.next();

        //int id = i.key();
        QVector<QPointF> data = i.value().m_data;

        QPolygonF polyline(data.count());
        painter->setPen(i.value().color);
        int nGlyphSize = static_cast<int>(this->font().pointSize()*0.75);

        if ((i.value().glyph==QtAddons::PlotGlyph_None) || (rect.width() < 1.05 * data.count()*nGlyphSize)) {
            for (int j = 0; j < data.count(); ++j) {
                double dx = data[j].x() - settings.minX;
                double dy = data[j].y() - settings.minY;
                double x = rect.left() + (dx * (rect.width() - 1)
                                             / settings.spanX());
                double y = rect.bottom() - (dy * (rect.height() - 1)
                                               / settings.spanY());
                polyline[j] = QPointF(x, y);
            }
        }
        else {
            QGlyphBase *pGlyph=BuildGlyph(i.value().glyph,nGlyphSize);

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

            delete pGlyph;
        }

        painter->drawPolyline(polyline);
    }
}

void Plotter::drawCursors(QPainter *painter)
{
    std::ostringstream msg;

    if (!cursorMap.empty()) {
        PlotSettings settings = zoomStack[curZoom];
        QRect rect(leftMargin, Margin,
                   width() - Margin - leftMargin, height() - 2 * Margin);
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



}

