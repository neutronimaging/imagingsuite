#include "plotwidget.h"
#include "ui_plotwidget.h"

#include <limits>
#include <QDebug>
#include <QMessageBox>

namespace QtAddons {

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget),
    m_nPointsVisible(25)
{
    ui->setupUi(this);

    QChart *chart = new QChart();
    ui->chart->setChart(chart);
    chart->layout()->setContentsMargins(4,4,4,4);
}

PlotWidget::~PlotWidget()
{
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
    if ( it != seriesmap.end()) {
        QLineSeries *line=dynamic_cast<QLineSeries *>(it->second);
        line->replace(series->points());
        line->setName(series->name());
        if (deleteData == true)
            try {
                delete series;
            } catch (std::exception &e) {
                QString msg="Failed to delete series:";
                msg=msg+QString::fromStdString(e.what());
                QMessageBox::warning(this,"Exception",msg);
            }

    } else {
        seriesmap[id]=series;

        ui->chart->chart()->addSeries(series);
    }

    QLineSeries *line=dynamic_cast<QLineSeries *>(seriesmap[id]);

    if (line->points().size()<=m_nPointsVisible) {
        line->setPointsVisible(true);
    }
    else {
        line->setPointsVisible(false);
    }

    ui->chart->chart()->createDefaultAxes();
    updateAxes();

}

void PlotWidget::clearCurve(int id)
{
    auto it=seriesmap.find(id);
    if ( it != seriesmap.end()) {
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

}
