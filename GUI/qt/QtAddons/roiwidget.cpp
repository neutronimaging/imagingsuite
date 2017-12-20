#include "roiwidget.h"
#include "ui_roiwidget.h"
#include <QSignalBlocker>

namespace QtAddons {
ROIWidget::ROIWidget(QWidget *parent) :
    QWidget(parent),
    logger("ROIWidget"),
    ui(new Ui::ROIWidget)
{
    ui->setupUi(this);
}

ROIWidget::~ROIWidget()
{
    delete ui;
}

void ROIWidget::on_spinX0_valueChanged(int arg1)
{
    mroi[0]=arg1;
    emit valueChanged(mroi[0],mroi[1],mroi[2],mroi[3]);
}

void ROIWidget::on_spinY0_valueChanged(int arg1)
{
    mroi[1]=arg1;
    emit valueChanged(mroi[0],mroi[1],mroi[2],mroi[3]);
}

void ROIWidget::on_spinX1_valueChanged(int arg1)
{
    mroi[2]=arg1;
    emit valueChanged(mroi[0],mroi[1],mroi[2],mroi[3]);
}

void ROIWidget::on_spinY1_valueChanged(int arg1)
{
    mroi[3]=arg1;
    emit valueChanged(mroi[0],mroi[1],mroi[2],mroi[3]);
}

void ROIWidget::on_buttonGetROI_clicked()
{
    emit getROIClicked();
}

void ROIWidget::setROI(int x0,int y0, int x1, int y1)
{
    mroi[0]=x0;
    mroi[1]=y0;
    mroi[2]=x1;
    mroi[3]=y1;

    updateFields();

}

void ROIWidget::setROI(int *roi)
{
    std::copy(roi,roi+4,mroi);
    updateFields();
}

void ROIWidget::setROI(size_t *roi)
{
    std::copy(roi,roi+4,mroi);

    updateFields();
}

void ROIWidget::setROI(QRect rect)
{
    rect.getCoords(mroi,mroi+1,mroi+2,mroi+3);
    updateFields();
}

void ROIWidget::updateFields()
{
    QSignalBlocker b0(ui->spinX0);
    QSignalBlocker b1(ui->spinY0);
    QSignalBlocker b2(ui->spinX1);
    QSignalBlocker b3(ui->spinY1);

    ui->spinX0->setValue(mroi[0]);
    ui->spinY0->setValue(mroi[1]);
    ui->spinX1->setValue(mroi[2]);
    ui->spinY1->setValue(mroi[3]);
}

void ROIWidget::getROI(int *roi)
{
    std::copy(mroi,mroi+4,roi);
}

void ROIWidget::getROI(size_t *roi)
{
   std::copy(mroi,mroi+4,roi);
}

QRect ROIWidget::getROI()
{
    QRect rect;

    rect.setCoords(mroi[0],mroi[1],mroi[2],mroi[3]);

    return rect;
}
}

