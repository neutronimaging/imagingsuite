#include "setgraylevelsdlg.h"
#include "ui_setgraylevelsdlg.h"
#include "imageviewerwidget.h"

#include <QSignalBlocker>
#include <QLineSeries>
#include <QPen>

namespace QtAddons {
SetGrayLevelsDlg::SetGrayLevelsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetGrayLevelsDlg)
{
    m_parent = dynamic_cast<QtAddons::ImageViewerWidget *>(parent);

    ui->setupUi(this);

    ui->histogram->hideLegend();

    float fMin=0.0f;
    float fMax=0.0f;

    QVector<QPointF> hist=m_parent->get_histogram();

    histMin = std::numeric_limits<double>::max(); // everything is <= this
    histMax = -std::numeric_limits<double>::max(); // everything is >= this

    foreach (QPointF p, hist) {
        histMin = qMin(histMin, p.y());
        histMax = qMax(histMax, p.y());
    }
    ui->histogram->setCurveData(0,hist,"Histogram");
    ui->histogram->setPointsVisible(0);
    m_parent->get_levels(&fMin,&fMax);

    ui->doubleSpinBox_levelLow->setValue(fMin);
    ui->doubleSpinBox_levelHigh->setValue(fMax);
    updateLevels(true,fMin,fMax);
}

SetGrayLevelsDlg::~SetGrayLevelsDlg()
{
    delete ui;
}

void SetGrayLevelsDlg::on_doubleSpinBox_levelLow_valueChanged(double arg1)
{
    updateLevels(true,arg1,ui->doubleSpinBox_levelHigh->value());
}

void SetGrayLevelsDlg::on_doubleSpinBox_levelHigh_valueChanged(double arg1)
{
    updateLevels(true,ui->doubleSpinBox_levelLow->value(),arg1);
}

void SetGrayLevelsDlg::on_doubleSpinBox_levelCenter_valueChanged(double arg1)
{
    updateLevels(false,arg1,ui->doubleSpinBox_levelWindow->value());

}

void SetGrayLevelsDlg::on_doubleSpinBox_levelWindow_valueChanged(double arg1)
{
      updateLevels(false,ui->doubleSpinBox_levelCenter->value(),arg1);
}

void SetGrayLevelsDlg::updateLevels(bool interval, double a, double b)
{
    QSignalBlocker b1(ui->doubleSpinBox_levelCenter);
    QSignalBlocker b2(ui->doubleSpinBox_levelWindow);
    QSignalBlocker b3(ui->doubleSpinBox_levelHigh);
    QSignalBlocker b4(ui->doubleSpinBox_levelLow);

    double lo=0;
    double hi=0;

    if (interval)
    {
        lo=a;
        hi=b;
        ui->doubleSpinBox_levelCenter->setValue((b+a)/2.0);
        ui->doubleSpinBox_levelWindow->setValue(fabs(b-a));

    }
    else
    {
        lo=a-b;
        hi=a+b;
    }

    ui->doubleSpinBox_levelLow->setMaximum(hi);
    ui->doubleSpinBox_levelHigh->setMinimum(lo);
    ui->doubleSpinBox_levelLow->setValue(lo);
    ui->doubleSpinBox_levelHigh->setValue(hi);

    m_parent->set_levels(lo,hi);

    QtCharts::QLineSeries *line=new QtCharts::QLineSeries();

    line->append(lo,histMax);
    line->append(lo,histMin);
    line->append(hi,histMax);
    line->append(hi,histMin);
    QPen pen=line->pen();
    pen.setColor(QColor("orange"));
    pen.setWidth(2.0);
    line->setPen(pen);
    line->setName("Display levels");

    ui->histogram->setCurveData(1,line);
}

}
