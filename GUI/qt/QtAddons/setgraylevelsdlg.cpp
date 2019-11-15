#include "setgraylevelsdlg.h"
#include "ui_setgraylevelsdlg.h"
#include "imageviewerwidget.h"

#include <QSignalBlocker>
#include <QLineSeries>
#include <QPen>
#include <QDebug>

namespace QtAddons {
SetGrayLevelsDlg::SetGrayLevelsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetGrayLevelsDlg),
    histMin(std::numeric_limits<double>::max()),
    histMax(-std::numeric_limits<double>::max()),
    q95lo(0.0),
    q95hi(0.0),
    q99lo(0.0),
    q99hi(0.0)
{
    m_parent = dynamic_cast<QtAddons::ImageViewerWidget *>(parent);

    ui->setupUi(this);

    ui->histogram->hideLegend();

    float fMin=0.0f;
    float fMax=0.0f;

    QVector<QPointF> hist=m_parent->get_histogram();

    QVector<QPointF> cumhist;
    double cumsum=0.0;
    foreach (QPointF p, hist) {
        histMin = qMin(histMin, p.y());
        histMax = qMax(histMax, p.y());
        cumsum+= p.y();
        cumhist.append(QPointF(p.x(),cumsum));
    }

    double c95lo=0.025*cumsum;
    double c95hi=0.975*cumsum;
    double c99lo=0.005*cumsum;
    double c99hi=0.995*cumsum;

//    qDebug() << "quantile counts "<<c95lo<<", "<<c95hi<<" and "<<c99lo<<", "<<c99hi;

    foreach (QPointF p, cumhist) {
        q95lo=p.y() <= c95lo ? p.x() : q95lo;
        q95hi=p.y() <= c95hi ? p.x() : q95hi;
        q99lo=p.y() <= c99lo ? p.x() : q99lo;
        q99hi=p.y() <= c99hi ? p.x() : q99hi;
    }

//    qDebug() << "quantile limits "<<q95lo<<", "<<q95hi<<" and "<<q99lo<<", "<<q99hi;

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
        lo=a-b/2;
        hi=a+b/2;
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


void QtAddons::SetGrayLevelsDlg::on_pushButton_95pc_clicked()
{
    updateLevels(true,q95lo,q95hi);
}

void QtAddons::SetGrayLevelsDlg::on_pushButton_99pc_clicked()
{
    updateLevels(true,q99lo, q99hi);
}
