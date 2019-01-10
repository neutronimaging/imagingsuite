#include "uxroiwidget.h"
#include "ui_uxroiwidget.h"

#include <QColor>
#include <QSignalBlocker>
#include <QDebug>

#include <base/roi.h>
#include "imageviewerwidget.h"

namespace QtAddons {

int uxROIWidget::cnt=0;

uxROIWidget::uxROIWidget(QWidget *parent) :
    QWidget(parent),
    logger("uxROIWidget"),
    roiID(cnt++),
    ui(new Ui::uxROIWidget),
    hViewer(nullptr),
    autoHideViewerROI(true),
    allowUpdateImageDims(true)
{
    ui->setupUi(this);
    setROI(0,0,100,100);
    setROIColor("darkgray");
}

uxROIWidget::~uxROIWidget()
{
    delete ui;
}

void uxROIWidget::setBoundingBox(int x0, int y0, int x1, int y1, bool updateFields)
{
    QSignalBlocker blockX0(ui->spinX0);
    QSignalBlocker blockY0(ui->spinY0);
    QSignalBlocker blockX1(ui->spinX1);
    QSignalBlocker blockY1(ui->spinY1);

    int maxx=std::max(x0,x1);
    int minx=std::min(x0,x1);
    ui->spinX1->setMinimum(minx+1);
    ui->spinX0->setMaximum(maxx-1);

    ui->spinX0->setMaximum(std::min(maxx,ui->spinX1->value()-1));
    ui->spinX0->setMinimum(minx);

    ui->spinX1->setMaximum(maxx);
    ui->spinX1->setMinimum(std::max(minx,ui->spinX0->value()+1));

    int maxy=std::max(y0,y1);
    int miny=std::min(y0,y1);

    ui->spinY0->setMaximum(std::min(maxy,ui->spinY1->value()-1));
    ui->spinY0->setMinimum(miny);

    ui->spinY1->setMaximum(maxy);
    ui->spinY1->setMinimum(std::max(minx,ui->spinY0->value()+1));

    if (updateFields) {
        setROI(x0,y0,x1,y1);
    }
}

void uxROIWidget::updateBounds()
{
    if ((allowUpdateImageDims==true) && (hViewer!=nullptr)) {
        int x,y;
        hViewer->getImageDims(x,y);
        setBoundingBox(0,0,x-1,y-1);
    }
}

void uxROIWidget::setROI(int x0, int y0, int x1, int y1, bool ignoreBoundingBox)
{
    QSignalBlocker blockX0(ui->spinX0);
    QSignalBlocker blockY0(ui->spinY0);
    QSignalBlocker blockX1(ui->spinX1);
    QSignalBlocker blockY1(ui->spinY1);

    int maxx=std::max(x0,x1);
    int minx=std::min(x0,x1);
    int maxy=std::max(y0,y1);
    int miny=std::min(y0,y1);
    ui->spinY1->setMinimum(miny+1);
    ui->spinY0->setMaximum(maxy-1);
    ui->spinX1->setMinimum(minx+1);
    ui->spinX0->setMaximum(maxx-1);
    if (ignoreBoundingBox) {
        ui->spinY1->setMaximum(10000);
        ui->spinY0->setMinimum(0);
        ui->spinX1->setMaximum(10000);
        ui->spinX0->setMinimum(0);
    }

    ui->spinX0->setValue(minx);
    ui->spinX1->setValue(maxx);


    ui->spinY0->setValue(miny);
    ui->spinY1->setValue(maxy);

    emit valueChanged(minx,miny,maxx,maxy);
    updateViewer();
}

void uxROIWidget::updateViewer()
{
    bool checkable = ui->groupROI->isCheckable();
    bool checked   = ui->groupROI->isChecked();

 //   qDebug() << "Checkable " << checkable <<"Checked"<<checked;
    if (hViewer!=nullptr) {
        if (isVisible() && (!checkable || checked)) {
            updateBounds();

            QRect rect;
            getROI(rect);
            hViewer->set_rectangle(rect,QColor(roiColor),roiID);
        }
        else {
            if (autoHideViewerROI)
                hViewer->clear_rectangle(roiID);
        }

    }
}

void uxROIWidget::setAllowUpdateImageDims(bool allow)
{
    allowUpdateImageDims=allow;
}

void uxROIWidget::setCheckable(bool x)
{
    ui->groupROI->setCheckable(x);
}

bool uxROIWidget::isChecked()
{
    return ui->groupROI->isChecked();
}

void uxROIWidget::setChecked(bool x)
{
    ui->groupROI->setChecked(x);
    updateViewer();
}

void uxROIWidget::setROI(size_t *roi, bool ignoreBoundingBox)
{
    setROI(static_cast<int>(roi[0]),
           static_cast<int>(roi[1]),
            static_cast<int>(roi[2]),
            static_cast<int>(roi[3]),ignoreBoundingBox);
}

void uxROIWidget::setROI(int *roi, bool ignoreBoundingBox)
{
    setROI(roi[0],roi[1],roi[2],roi[3],ignoreBoundingBox);
}

void uxROIWidget::setROI(QRect rect, bool ignoreBoundingBox)
{
    setROI(rect.x(),rect.y(),rect.x()+rect.width(),rect.y()+rect.height(),ignoreBoundingBox);
}

void uxROIWidget::setROI(kipl::base::RectROI roi, bool ignoreBoundingBox)
{
    size_t iroi[4];

   roi.getBox(iroi);
   setROI(static_cast<int>(iroi[0]),
          static_cast<int>(iroi[1]),
           static_cast<int>(iroi[2]),
           static_cast<int>(iroi[3]),ignoreBoundingBox);
}

void uxROIWidget::getROI(QRect &rect)
{
    rect=QRect(ui->spinX0->value(),
               ui->spinY0->value(),
               ui->spinX1->value()-ui->spinX0->value(),
               ui->spinY1->value()-ui->spinY0->value());

}

void uxROIWidget::getROI(int &x0, int &y0, int &x1, int &y1)
{
    x0=ui->spinX0->value();
    y0=ui->spinY0->value();
    x1=ui->spinX1->value();
    y1=ui->spinY1->value();
}

void uxROIWidget::getROI(int *roi)
{
    getROI(roi[0],roi[1],roi[2],roi[3]);
}

void uxROIWidget::getROI(size_t *roi)
{
    int iroi[4];
    getROI(iroi);
    std::copy(iroi,iroi+4,roi);
}

void uxROIWidget::getROI(kipl::base::RectROI & roi)
{
    size_t iroi[4];
    getROI(iroi);
    roi = kipl::base::RectROI(iroi);
}

void uxROIWidget::setTitle(const QString &lbl)
{
    ui->groupROI->setTitle(lbl);
}

bool uxROIWidget::setROIColor(const QString color)
{
    if (QColor::isValidColor(color)==false)
        return false;

    roiColor=color;

    QString style="border-color: ";
    style += color + "; border-width: 2px; border-style: solid";

    ui->buttonGetROI->setStyleSheet(style);
    updateViewer();

    return true;
}

void uxROIWidget::on_spinX0_valueChanged(int arg1)
{
    ui->spinX1->setMinimum(arg1+1);
    int roi[4];
    getROI(roi);
    updateViewer();
    qDebug("uxROIWidget::on_spinX0_valueChanged");
    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void uxROIWidget::on_spinY0_valueChanged(int arg1)
{
    ui->spinY1->setMinimum(arg1+1);
    int roi[4];
    getROI(roi);
    updateViewer();
    qDebug("uxROIWidget::on_spinY0_valueChanged");
    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void uxROIWidget::on_spinY1_valueChanged(int arg1)
{
    ui->spinY0->setMaximum(arg1-1);
    int roi[4];
    getROI(roi);
    updateViewer();
    qDebug("uxROIWidget::on_spinY1_valueChanged");
    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void uxROIWidget::on_spinX1_valueChanged(int arg1)
{
    ui->spinX0->setMaximum(arg1-1);
    int roi[4];
    getROI(roi);
    updateViewer();

    emit valueChanged(roi[0],roi[1],roi[2],roi[3]);
}

void uxROIWidget::on_buttonGetROI_clicked()
{
    if (hViewer != nullptr) {
        updateBounds();
        QRect rect=hViewer->get_marked_roi();

        setROI(rect);
    }
    emit getROIClicked();
    repaint();
}

void uxROIWidget::on_valueChanged(int x0,int y0, int x1, int y1)
{
    (void) x0;
    (void) y0;
    (void) x1;
    (void) y1;

    updateViewer();
}

void uxROIWidget::on_viewerNewImageDims(const QRect &rect)
{
    if ((allowUpdateImageDims==true) && (hViewer!=nullptr)) {
        setBoundingBox(0,0,rect.width()-1,rect.height()-1);
    }

}

void uxROIWidget::registerViewer(ImageViewerWidget *viewer)
{
    hViewer = viewer;
    updateViewer();
    if (hViewer!=nullptr) {
        connect(hViewer,&QtAddons::ImageViewerWidget::newImageDims,this,&uxROIWidget::on_viewerNewImageDims);
    }
}

void uxROIWidget::setAutoHideROI(bool hide)
{
    autoHideViewerROI=hide;
    updateViewer();
}

void uxROIWidget::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);

    updateViewer();
}

void uxROIWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    updateViewer();
}


}

void QtAddons::uxROIWidget::on_groupROI_toggled(bool arg1)
{
    updateViewer();
    emit toggled(arg1);
}
