#include "plotwidget.h"
#include "ui_plotwidget.h"

#include <fstream>
#include <limits>
#include <algorithm>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

#include <strings/filenames.h>

#include <base/KiplException.h>

namespace QtAddons {

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    logger("PlotWidget"),
    ui(new Ui::PlotWidget),
    m_nPointsVisible(25),
    m_tooltip(nullptr)
{
    ui->setupUi(this);

    QChart *chart = new QChart();
    ui->chart->setChart(chart);
    chart->layout()->setContentsMargins(2,2,2,2);
    ui->chart->chart()->setAcceptHoverEvents(true);
    setupActions();
}

PlotWidget::~PlotWidget()
{
    clearAllCursors();
    delete ui;
}

void PlotWidget::setCurveData(int id, const QVector<QPointF> &data, QString name)
{
    QtCharts::QLineSeries *series=new QtCharts::QLineSeries();
    int i=0;
    for (auto it=data.begin(); it!=data.end(); ++it, ++i)
        series->append(it->x(),it->y());

    if (name.isEmpty() ==false)
        series->setName(name);

    setCurveData(id,series);
}

void PlotWidget::setCurveData(int id, const float * const x, const float * const y, const int N, QString name)
{
    QtCharts::QLineSeries *series=new QtCharts::QLineSeries();

    for (int i=0; i<N ; ++i)
        series->append(qreal(x[i]),qreal(y[i]));

    if (name.isEmpty() ==false)
        series->setName(name);

    setCurveData(id,series);
}

void PlotWidget::setCurveData(int id, const float * const x, const size_t * const y, const int N, QString name)
{
    QtCharts::QLineSeries *series=new QtCharts::QLineSeries();

    for (int i=0; i<N ; ++i)
        series->append(qreal(x[i]),qreal(y[i]));

    if (name.isEmpty() ==false)
        series->setName(name);

    setCurveData(id,series);

}

void PlotWidget::setCurveData(int id, QLineSeries *series, bool deleteData)
{
    auto it=seriesmap.find(id);

    if ( (it != seriesmap.end()) && (it->second->type()==series->type()) )
    {       
        QLineSeries *line=dynamic_cast<QLineSeries *>(it->second);
        line->replace(series->points());
        line->setName(series->name());
        if (deleteData == true)
        {
            try
            {
                delete series;
            }
            catch (std::exception &e)
            {
                QString msg="Failed to delete series:";
                msg=msg+e.what();
                QMessageBox::warning(this,"Exception",msg);
            }
        }
    }
    else
    {
        if (it != seriesmap.end())
            ui->chart->chart()->removeSeries(it->second);

        seriesmap[id]=series;

        ui->chart->chart()->addSeries(series);
    }

    QLineSeries *line=dynamic_cast<QLineSeries *>(seriesmap[id]);

    connect(line, &QLineSeries::hovered, this, &PlotWidget::tooltip);

    if (line->points().size()<=m_nPointsVisible)
    {
        line->setPointsVisible(true);
    }
    else
    {
        line->setPointsVisible(false);
    }

    ui->chart->chart()->createDefaultAxes();
    updateAxes();
    updateCursors();
}

void PlotWidget::setDataSeries(int id, QAbstractSeries *series, bool deleteData)
{
    auto it=seriesmap.find(id);
    if ( it != seriesmap.end())
    {
        try
        {
            ui->chart->chart()->removeSeries(it->second);
        }
        catch (std::exception &e)
        {
            QString msg="Failed to delete series:";
            msg=msg+e.what();
            QMessageBox::warning(this,"Exception",msg);
        }

    }

    seriesmap[id]=series;
    ui->chart->chart()->addSeries(series);
    ui->chart->chart()->createDefaultAxes();
    updateAxes();
    updateCursors();

}

void PlotWidget::clearCurve(int id)
{
    auto it=seriesmap.find(id);
    if ( it != seriesmap.end())
    {
        ui->chart->chart()->removeSeries(it->second);
        seriesmap.erase(it);
    }
    updateCursors();
}

void PlotWidget::clearAllCurves()
{
    seriesmap.clear();
    ui->chart->chart()->removeAllSeries();
}

void PlotWidget::setPointsVisible(int n)
{
    m_nPointsVisible=n;
}

void PlotWidget::showLegend()
{
    ui->chart->chart()->legend()->setVisible(true);
}

void PlotWidget::hideLegend()
{
    ui->chart->chart()->legend()->setVisible(false);
}

void PlotWidget::showCurve(int n)
{
    auto it=seriesmap.find(n);
    if ( it != seriesmap.end())
    {
        it->second->show();
    }
}

void PlotWidget::hideCurve(int n)
{
    auto it=seriesmap.find(n);
    if ( it != seriesmap.end())
    {
        it->second->hide();
    }
}

void PlotWidget::setTitle(QString title)
{
    ui->chart->chart()->setTitle(title);
}

void PlotWidget::hideTitle()
{
    ui->chart->chart()->setTitle("");
}

void PlotWidget::updateAxes()
{ 
    findMinMax();

    if (minX!=std::numeric_limits<double>::max())
        ui->chart->chart()->axisX()->setMin(minX);

    if (maxX!=-std::numeric_limits<double>::max())
        ui->chart->chart()->axisX()->setMax(maxX);

    if (minY!=std::numeric_limits<double>::max())
        ui->chart->chart()->axisY()->setMin(minY);

    if (maxY!=-std::numeric_limits<double>::max())
        ui->chart->chart()->axisY()->setMax(maxY);
}

void PlotWidget::setXLabel(const QString &lbl)
{
    if (lbl.isEmpty())
        ui->chart->chart()->axisX()->setTitleVisible(false);
    else
    {
        ui->chart->chart()->axisX()->setTitleText(lbl);
        ui->chart->chart()->axisX()->setTitleVisible(true);
    }
}

void PlotWidget::setYLabel(const QString &lbl)
{
    if (lbl.isEmpty())
        ui->chart->chart()->axisY()->setTitleVisible(false);
    else
    {
        ui->chart->chart()->axisY()->setTitleText(lbl);
        ui->chart->chart()->axisY()->setTitleVisible(true);
    }
}

void PlotWidget::findMinMax()
{
    minX=std::numeric_limits<double>::max();
    maxX=-std::numeric_limits<double>::max();
    minY=std::numeric_limits<double>::max();
    maxY=-std::numeric_limits<double>::max();

    if (seriesmap.empty()==true)
        return;

    for (const auto &seriesItem : seriesmap)
    {
        switch (seriesItem.second->type())
        {
            case QAbstractSeries::SeriesTypeLine :
            {
                QLineSeries *series=dynamic_cast<QLineSeries *>(seriesItem.second);

                QVector<QPointF> points=series->pointsVector();
                for (const auto &point: points)
                {
                    minX = std::min(minX,point.x());
                    maxX = std::max(maxX,point.x());
                    minY = std::min(minY,point.y());
                    maxY = std::max(maxY,point.y());
                }
            }
            break;
            case QAbstractSeries::SeriesTypeBoxPlot :
            break;
        }
    }
}

void PlotWidget::setCursor(int id, PlotCursor *c)
{
    if (ui->chart->chart()->series().empty())
        throw kipl::base::KiplException("no plot series",__FILE__,__LINE__);

    auto it=cursors.find(id);
    if ( it != cursors.end())
    {
        *cursors[id]=*c;
        delete c;
    }
    else {
        cursors.insert(std::make_pair(id,c));
    }
    updateCursors();

}

void PlotWidget::clearCursor(int id)
{
    auto it=cursors.find(id);
    if ( it != cursors.end())
    {
        delete it->second;
        cursors.erase(it);
        auto it2=cursormap.find(it->first);
        if (it2!=cursormap.end()) {
            ui->chart->chart()->removeSeries(it2->second);
            cursormap.erase(it2);
        }

    }
}

void PlotWidget::clearAllCursors()
{
    while (!cursors.empty()) {
        delete cursors.begin()->second;

        auto it2=cursormap.find(cursors.begin()->first);
        if (it2!=cursormap.end()) {
            ui->chart->chart()->removeSeries(it2->second);
            cursormap.erase(it2);
        }
        cursors.erase(cursors.begin());
    }
}

size_t PlotWidget::cursorCount()
{
    return cursors.size();
}

void PlotWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(savePlotAct);
    menu.addAction(copyAct);
    menu.addAction(savePlotDataAct);
    menu.exec(event->globalPos());
}

void PlotWidget::setupActions()
{
    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &PlotWidget::copy);

    savePlotAct = new QAction(tr("&Save plot"), this);
    savePlotAct->setShortcuts(QKeySequence::Cut);
    savePlotAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(savePlotAct, &QAction::triggered, this, &PlotWidget::savePlot);

    savePlotDataAct = new QAction(tr("Save plot &Data"), this);
    savePlotDataAct->setShortcuts(QKeySequence::Paste);
    savePlotDataAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(savePlotDataAct, &QAction::triggered, this, &PlotWidget::saveCurveData);
}

void PlotWidget::savePlot()
{
    QString destname=QFileDialog::getSaveFileName(this,"Where should the plot be saved?",QDir::homePath()+"/plot.png");

    if (!destname.isEmpty())
    {
        auto wsize = ui->chart->size();
        const int w=1024;
        if (wsize.width()<w)
        {
            wsize = QSize(w,static_cast<int>(w*static_cast<float>(wsize.height())/static_cast<float>(wsize.width())));

        }

        QPixmap p( wsize );
        QPainter painter(&p);
        painter.fillRect(QRect(0,0,wsize.width(),wsize.height()),QColor("lightgray"));
        ui->chart->render( &painter);
        p.save(destname,"PNG");
    }
}

void PlotWidget::copy()
{
       QClipboard *clipboard = QApplication::clipboard();
       auto wsize = ui->chart->size();
       const int w=1024;
       if (wsize.width()<w)
       {
           wsize = QSize(w,static_cast<int>(w*static_cast<float>(wsize.height())/static_cast<float>(wsize.width())));

       }

       QPixmap p( wsize );
       QPainter painter(&p);
       painter.fillRect(QRect(0,0,wsize.width(),wsize.height()),QColor("lightgray"));
       ui->chart->render( &painter);

       clipboard->setPixmap(p);

}

void PlotWidget::saveCurveData()
{
        QString destname=QFileDialog::getSaveFileName(this,"Where should the data series be saved?",QDir::homePath()+"/data.txt");

        if (!destname.isEmpty()) {
            std::string fname=destname.toStdString();
            kipl::strings::filenames::CheckPathSlashes(fname,false);

            std::string path;
            std::string name;
            std::vector<std::string> ext;
            kipl::strings::filenames::StripFileName(fname,path,name,ext);
            name=path+name+"_";

            if (!seriesmap.empty())
            {
                std::string dname;
                int i=0;
                for (auto s : seriesmap)
                {

                    std::string seriesname=s.second->name().toStdString();
                    std::replace(seriesname.begin(),seriesname.end(),' ','_');
                    if (seriesname.empty())
                    {
                        seriesname=std::to_string(i++);
                    }
                    dname=name+seriesname+".txt";
                    std::ofstream datastream(dname.c_str());

                    switch (s.second->type()) {
                    case QAbstractSeries::SeriesTypeLine :
                    {
                        QLineSeries *series = dynamic_cast<QLineSeries *>(s.second);
                        auto data = series->pointsVector();
                        for (auto p : data)
                        {
                            datastream<<p.x()<<", "<<p.y()<<std::endl;
                        }
                        break;
                    }
                    case QAbstractSeries::SeriesTypeBoxPlot :
                    {
                        QBoxPlotSeries *series = dynamic_cast<QBoxPlotSeries *>(s.second);
                        auto data = series->boxSets();
                        for (const auto pp : data)
                        {
                            auto & p = *pp;
                            datastream<<p.label().toStdString()<<", "
                                      <<p[p.Median]<<", "
                                      <<p[p.LowerQuartile]<<", "
                                      <<p[p.UpperQuartile]<<", "
                                      <<p[p.LowerExtreme]<<", "
                                      <<p[p.UpperExtreme]
                                      <<"\n";
                        }
                        break;
                    }
                    default : logger.message("This plot type can't be saved as data");
                    }
                }
            }
        }

}

void PlotWidget::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(ui->chart->chart());
}

void PlotWidget::tooltip(QPointF point, bool state)
{
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(ui->chart->chart());

    if (state)
    {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    }
    else
    {
        m_tooltip->hide();
    }
}

void PlotWidget::updateCursors()
{
    findMinMax();

    for (const auto &c : cursors)
    {
        QtCharts::QLineSeries *line=nullptr;
        auto it=cursormap.find(c.first);
        if (it==cursormap.end())
        {
            line=new QtCharts::QLineSeries();
            cursormap.insert(std::make_pair(c.first,line));



            line->setPointsVisible(false);

            if (c.second->m_Orientation == QtAddons::PlotCursor::Vertical)
            {
                line->append(c.second->m_fPosition,minY);
                line->append(c.second->m_fPosition,maxY);
            }
            else
            {
                line->append(minX,c.second->m_fPosition);
                line->append(maxX,c.second->m_fPosition);
            }


            ui->chart->chart()->addSeries(line);
            ui->chart->chart()->legend()->markers(line)[0]->setVisible(false);
        }
        else
        {
            line=it->second;
            line->clear();
            if (c.second->m_Orientation == QtAddons::PlotCursor::Vertical)
            {
                line->append(c.second->m_fPosition,minY);
                line->append(c.second->m_fPosition,maxY);
            }
            else
            {
                line->append(minX,c.second->m_fPosition);
                line->append(maxX,c.second->m_fPosition);
            }

        }

        QPen pen=line->pen();
        pen.setColor(c.second->m_Color);
        pen.setWidth(2.0);
        line->setPen(pen);

        line->setName(c.second->m_sLabel);
        ui->chart->chart()->createDefaultAxes();

    }
}
}
