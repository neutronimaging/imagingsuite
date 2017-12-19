#include "roiwidget.h"
#include "ui_roiwidget.h"

ROIWidget::ROIWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ROIWidget),
    logger("ROIWidget")
{
    ui->setupUi(this);
}

ROIWidget::~ROIWidget()
{
    delete ui;
}

void ROIWidget::on_spinX0_valueChanged(int arg1)
{
    roi[0]=arg1;
    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void ROIWidget::on_spinY0_valueChanged(int arg1)
{
    roi[1]=arg1;
    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void ROIWidget::on_spinX1_valueChanged(int arg1)
{
    roi[2]=arg1;
    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void ROIWidget::on_spinY1_valueChanged(int arg1)
{
    roi[3]=arg1;
    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void ROIWidget::on_buttonGetROI_clicked()
{
    emit getROIClicked();
}
