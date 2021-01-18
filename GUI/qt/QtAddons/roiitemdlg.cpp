#include "roiitemdlg.h"
#include "ui_roiitemdlg.h"
#include <QString>

namespace QtAddons {
ROIItemDlg::ROIItemDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROIItemDlg)
{
    ui->setupUi(this);
}

ROIItemDlg::~ROIItemDlg()
{
    delete ui;
}

void ROIItemDlg::setROIItem(kipl::base::RectROI &roi)
{
    ui->widget_ROIItem->setROI(roi);
    ui->edit_Label->setText(QString::fromStdString(roi.label()));
}

kipl::base::RectROI ROIItemDlg::roiItem()
{
    kipl::base::RectROI roi(0,0,1,1,"0");

    ui->widget_ROIItem->getROI(roi);
    roi.setLabel(ui->edit_Label->text().toStdString());

    return roi;
}
}
