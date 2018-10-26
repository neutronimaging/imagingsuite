//<LICENSE>

#include "comparevolumeviewer.h"
#include "ui_comparevolumeviewer.h"

#include <QSignalBlocker>
#include <QMessageBox>


#include <base/textractor.h>
namespace QtAddons {
CompareVolumeViewer::CompareVolumeViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompareVolumeViewer),
    pOriginal(nullptr),
    pProcessed(nullptr),
    bShowDifference(true)
{
    ui->setupUi(this);
}

CompareVolumeViewer::~CompareVolumeViewer()
{
    delete ui;
}

void CompareVolumeViewer::setImages(kipl::base::TImage<float, 3> *pOrig, kipl::base::TImage<float, 3> *pProc)
{
    pOriginal  = pOrig;
    pProcessed = pProc;

    if (pOriginal!=nullptr) {
        ui->comboBox_imagePlanes->setCurrentIndex(0);
        updateBounds();
    }
}

void CompareVolumeViewer::setImageLabel(QString lbl, int idx)
{
    switch (idx) {
    case 0 : ui->groupBox_original->setTitle(lbl); break;
    case 1 : ui->groupBox_processed->setTitle(lbl); break;
    case 2 : ui->groupBox_difference->setTitle(lbl); break;
    default : throw kipl::base::KiplException("Label index exceed number of viewers.",__FILE__,__LINE__);
    }
}

void CompareVolumeViewer::showDifference(bool show)
{
    bShowDifference = show;
    if (show==true)
        ui->groupBox_difference->show();
    else
        ui->groupBox_difference->hide();
}


void CompareVolumeViewer::on_checkBox_linkIntensity_toggled(bool checked)
{
    if (checked == true)
        ui->originalImage->LinkImageViewer(ui->processedImage);
    else
        ui->originalImage->ClearLinkedImageViewers();
}

void CompareVolumeViewer::on_horizontalSlider_slices_sliderMoved(int position)
{
    QSignalBlocker block(ui->spinBox_slices);
    ui->spinBox_slices->setValue(position);
    updateViews(position);
}

void CompareVolumeViewer::on_spinBox_slices_valueChanged(int arg1)
{
    QSignalBlocker block(ui->horizontalSlider_slices);
    ui->horizontalSlider_slices->setValue(arg1);
    updateViews(arg1);
}

void CompareVolumeViewer::updateViews(int idx)
{
    if (pOriginal!=nullptr) {
        try {
            kipl::base::eImagePlanes plane=static_cast<kipl::base::eImagePlanes>(1<<(ui->comboBox_imagePlanes->currentIndex()));


            kipl::base::TImage<float,2> orig = kipl::base::ExtractSlice(*pOriginal,idx,plane);

            ui->originalImage->set_image(orig.GetDataPtr(),orig.Dims());

            if ((pProcessed!=nullptr) && (pOriginal->Size()==pProcessed->Size())) {
                kipl::base::TImage<float,2> proc = kipl::base::ExtractSlice(*pProcessed,idx,plane);

                ui->processedImage->set_image(proc.GetDataPtr(),proc.Dims());

                if (bShowDifference == true)
                {
                    kipl::base::TImage<float,2> diffimg=proc-orig;

                    ui->differenceImage->set_image(diffimg.GetDataPtr(),diffimg.Dims());
                }
            }
        }
        catch (kipl::base::KiplException & e) {
            QMessageBox::warning(this,"Display problems",QString::fromStdString(e.what()));
        }
    }
}

kipl::base::eImagePlanes CompareVolumeViewer::getImagePlane(){
    return static_cast<kipl::base::eImagePlanes>(1<<(ui->comboBox_imagePlanes->currentIndex()));
}

void CompareVolumeViewer::updateBounds()
{
    kipl::base::eImagePlanes plane=static_cast<kipl::base::eImagePlanes>(1<<(ui->comboBox_imagePlanes->currentIndex()));
    int midslice=0;
    int maxslice=0;

    switch (plane) {
        case kipl::base::ImagePlaneXY : maxslice=static_cast<int>(pOriginal->Size(2)-1); break;
        case kipl::base::ImagePlaneXZ : maxslice=static_cast<int>(pOriginal->Size(1)-1); break;
        case kipl::base::ImagePlaneYZ : maxslice=static_cast<int>(pOriginal->Size(0)-1); break;
    }

    midslice = maxslice/2;

    QSignalBlocker a(ui->horizontalSlider_slices);
    QSignalBlocker b(ui->spinBox_slices);

    ui->horizontalSlider_slices->setMaximum(maxslice);
    ui->horizontalSlider_slices->setMinimum(0);
    ui->horizontalSlider_slices->setValue(midslice);

    ui->spinBox_slices->setMaximum(maxslice);
    ui->spinBox_slices->setMinimum(0);
    ui->spinBox_slices->setValue(midslice);

    updateViews(midslice);
}

void QtAddons::CompareVolumeViewer::on_comboBox_imagePlanes_currentIndexChanged(int index)
{
    updateBounds();
}

}
