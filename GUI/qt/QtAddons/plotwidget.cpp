#include "plotwidget.h"
#include "ui_plotwidget.h"

#include <fstream>
#include <limits>
#include <algorithm>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

#include <strings/filenames.h>

namespace QtAddons {

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget),
    m_nPointsVisible(25),
    m_tooltip(nullptr)
{
    ui->setupUi(this);

    QChart *chart = new QChart();
    ui->chart->setChart(chart);
    chart->layout()->setContentsMargins(4,4,4,4);
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
    if ( it != seriesmap.end())
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
                msg=msg+QString::fromStdString(e.what());
                QMessageBox::warning(this,"Exception",msg);
            }
        }
    }
    else
    {
        seriesmap[id]=series;

        ui->chart->chart()->addSeries(series);
    }

    QLineSeries *line=dynamic_cast<QLineSeries *>(seriesmap[id]);
//    connect(line, &QLineSeries::clicked, this, &PlotWidget::keepCallout);
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

}

void PlotWidget::clearCurve(int id)
{
    auto it=seriesmap.find(id);
    if ( it != seriesmap.end())
    {
        ui->chart->chart()->removeSeries(it->second);
        seriesmap.erase(it);
    }
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
    ui->chart->chart()->axisX()->setMin(minX);
    ui->chart->chart()->axisX()->setMax(maxX);
    ui->chart->chart()->axisY()->setMin(minY);
    ui->chart->chart()->axisY()->setMax(maxY);
}

void PlotWidget::findMinMax()
{
    minX=std::numeric_limits<double>::max();
    maxX=-std::numeric_limits<double>::max();
    minY=std::numeric_limits<double>::max();
    maxY=-std::numeric_limits<double>::max();

    if (seriesmap.empty()==true)
        return;

    for (auto it=seriesmap.begin(); it!=seriesmap.end(); ++it)
    {
        QLineSeries *series=dynamic_cast<QLineSeries *>(it->second);

        QVector<QPointF> points=series->pointsVector();
        for (auto point=points.begin(); point!=points.end(); ++point)
        {
            minX = std::min(minX,point->x());
            maxX = std::max(maxX,point->x());
            minY = std::min(minY,point->y());
            maxY = std::max(maxY,point->y());
        }
    }
}

void PlotWidget::setCursor(int id, PlotCursor *c)
{
    auto it=cursors.find(id);
    if ( it != cursors.end())
    {
        *cursors[id]=*c;
        delete c;
    }
    else
    {
        cursors[id]=c;
    }
}

void PlotWidget::clearCursors(int id)
{
    auto it=cursors.find(id);
    if ( it != cursors.end())
    {
        delete it->second;
        cursors.erase(it);
    }
}

void PlotWidget::clearAllCursors()
{
    while (!cursors.empty())
    {
        delete cursors.begin()->second;
        cursors.erase(cursors.begin());
    }
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
    qDebug() << "Save plot";

    QString destname=QFileDialog::getSaveFileName(this,"Where should the plot be saved?",QDir::homePath()+"/plot.png");

    if (!destname.isEmpty())
    {
        QPixmap p( ui->chart->size() );
        QPainter painter(&p);
        ui->chart->render( &painter);
        p.save(destname,"PNG");
    }
}

void PlotWidget::copy()
{
       qDebug() << "copy";

       QClipboard *clipboard = QApplication::clipboard();

       QPixmap p( ui->chart->size() );
       QPainter painter(&p);
       ui->chart->render( &painter);

       clipboard->setPixmap(p);

}

void PlotWidget::saveCurveData()
{
        qDebug() << "Save curve data";

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
                    QXYSeries *series = dynamic_cast<QXYSeries *>(s.second);
                    std::string seriesname=series->name().toStdString();
                    std::replace(seriesname.begin(),seriesname.end(),' ','_');
                    if (seriesname.empty())
                    {
                        seriesname=std::to_string(i++);
                    }
                    dname=name+seriesname+".txt";
                    qDebug() << "Saving"<< dname.c_str();
                    std::ofstream datastream(dname.c_str());

                    auto data = series->pointsVector();
                    for (auto p : data)
                    {
                        datastream<<p.x()<<", "<<p.y()<<std::endl;
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
}
