#include "plotwidget.h"
#include "ui_plotwidget.h"

#include <QDebug>

namespace QtAddons {

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
    QChart *chart = new QChart();
    ui->chart->setChart(chart);
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

void PlotWidget::setCurveData(int id, QLineSeries *series)
{
    auto it=seriesmap.find(id);
    if ( it != seriesmap.end()) {
        ui->chart->chart()->removeSeries(it->second);
    }

    seriesmap[id]=series;

    ui->chart->chart()->addSeries(series);
    ui->chart->chart()->createDefaultAxes();

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

}
