#include <sstream>
#include <string>

#include <QString>

#include <base/tsubimage.h>
#include <math/statistics.h>

#include "imageviewerinfodialog.h"
#include "ui_imageviewerinfodialog.h"


ImageViewerInfoDialog::ImageViewerInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageViewerInfoDialog)
{
    ui->setupUi(this);
}

ImageViewerInfoDialog::~ImageViewerInfoDialog()
{
    delete ui;
}

void ImageViewerInfoDialog::UpdateInfo(kipl::base::TImage<float,2> img, QRect roi)
{
    size_t nroi[4]={static_cast<size_t>(roi.x()),
                    static_cast<size_t>(roi.y()),
                    static_cast<size_t>(roi.x()+roi.width()),
                    static_cast<size_t>(roi.y()+roi.height())};

    m_roiImage = kipl::base::TSubImage<float,2>::Get(img,nroi);

    kipl::math::Statistics stats;

    stats.put(m_roiImage.GetDataPtr(),m_roiImage.Size());

    double mean=stats.E();
    double stddev=stats.s();
    double minval=stats.Min();
    double maxval=stats.Max();

    int area=roi.width()*roi.height();

    std::ostringstream text;
    text.str(""); text<<"["<<nroi[0]<<", "<<nroi[1]<<", "<<nroi[2]<<", "<<nroi[3]<<"]";
    ui->label_ROI->setText(QString::fromStdString(text.str()));

    text.str(""); text<<area<<" pixels";
    ui->label_Area->setText(QString::fromStdString(text.str()));

    text.str(""); text<<mean;
    ui->label_Mean->setText(QString::fromStdString(text.str()));

    text.str(""); text<<stddev;
    ui->label_StdDev->setText(QString::fromStdString(text.str()));

    text.str(""); text<<minval;
    ui->label_Min->setText(QString::fromStdString(text.str()));

    text.str(""); text<<maxval;
    ui->label_Max->setText(QString::fromStdString(text.str()));
}
