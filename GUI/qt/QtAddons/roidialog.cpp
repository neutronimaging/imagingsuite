#include "roidialog.h"
#include "ui_roidialog.h"

#include <QDir>
#include <QString>
#include <QMessageBox>

namespace QtAddons {
ROIDialog::ROIDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROIDialog)
{
    ui->setupUi(this);
    ui->widget_roi->registerViewer(ui->widget_image);
    ui->widget_roi->setROIColor("red");
}

ROIDialog::~ROIDialog()
{
    delete ui;
}

void ROIDialog::setImage(kipl::base::TImage<float,2> &img)
{
    ui->widget_image->set_image(img.GetDataPtr(),img.dims());
}

void ROIDialog::setROI(QRect r)
{
    ui->widget_roi->setROI(r);
}

QRect ROIDialog::ROI()
{
    QRect r;
    ui->widget_roi->getROI(r);

    return r;
}
}
