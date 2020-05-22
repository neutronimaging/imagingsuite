#include <sstream>
#include <string>

#include <QString>

#include <base/tsubimage.h>
#include <math/statistics.h>
#include <base/thistogram.h>

#include "imageviewerinfodialog.h"
#include "ui_imageviewerinfodialog.h"


ImageViewerInfoDialog::ImageViewerInfoDialog(QWidget *parent) :
    QDialog(parent),
    logger("ImageViewerInfoDialog"),
    ui(new Ui::ImageViewerInfoDialog)
{
    ui->setupUi(this);
    ui->histogramplot->hideTitle();
    ui->histogramplot->showLegend();
}

ImageViewerInfoDialog::~ImageViewerInfoDialog()
{
    delete ui;
}

void ImageViewerInfoDialog::updateInfo(kipl::base::TImage<float,2> img, QRect roi)
{
    std::vector<size_t> nroi = {static_cast<size_t>(roi.x()),
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

    const size_t nHist=256;
    float axis[nHist];
    size_t hist[nHist];

    memset(hist,0,nHist*sizeof(size_t));
    kipl::base::Histogram(m_roiImage.GetDataPtr(),m_roiImage.Size(),hist,nHist,0.0f,0.0f,axis,true);

    auto maxit=std::max_element(hist,hist+nHist);
    maxval=static_cast<double>(*maxit);

    m_LocalHistogram.clear();
    for (size_t i=0; i<nHist; i++) {
        m_LocalHistogram.append(QPointF(static_cast<double>(axis[i]),static_cast<double>(hist[i])/(maxval)));
    }

    ui->histogramplot->setCurveData(1,m_LocalHistogram,"Local");

    on_check_showglobal_toggled(ui->check_showglobal->isChecked());

    ui->histogramplot->setXLabel("Levels");
    ui->histogramplot->setYLabel("Relative frequency");
}


void ImageViewerInfoDialog::setHistogram(const QVector<QPointF> &hist)
{
    float maxval=0.0;
    for (auto it=hist.begin(); it!=hist.end(); it++) {
        maxval=maxval<it->y() ? it->y(): maxval;
    }

    m_GlobalHistogram.clear();
    for (auto it=hist.begin(); it!=hist.end(); it++) {
        m_GlobalHistogram.append(QPointF(it->x(),it->y()/maxval));
    }

    ui->histogramplot->setCurveData(0,m_GlobalHistogram,"Global");
    on_check_showglobal_toggled(ui->check_showglobal->isChecked());
}

void ImageViewerInfoDialog::on_check_showglobal_toggled(bool checked)
{
    if (checked) {
        ui->histogramplot->showCurve(0);
    }
    else {
        ui->histogramplot->hideCurve(0);
    }
}
