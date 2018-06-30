//<LICENSE>

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

#include "pixelsizedlg.h"
#include "ui_pixelsizedlg.h"

#include <strings/filenames.h>
#include <imagereader.h>
#include <math/statistics.h>
#include <math/findpeaks.h>
#include <io/io_serializecontainers.h>

PixelSizeDlg::PixelSizeDlg(QWidget *parent) :
    QDialog(parent),
    logger("PixelSizeDlg"),
    ui(new Ui::PixelSizeDlg),
    pixelSize(0.1f)
{
    ui->setupUi(this);
    ui->roi->registerViewer(ui->viewer);
    ui->roi->setROIColor("red");
}

PixelSizeDlg::~PixelSizeDlg()
{
    delete ui;
}

float PixelSizeDlg::getPixelSize()
{
    return pixelSize;
}

float PixelSizeDlg::getDistance(kipl::base::TImage<float, 2> &im, size_t *roi)
{
    size_t w=roi[2]-roi[0];
    size_t h=roi[3]-roi[1];
    size_t N=max(w,h);
    float *profile=new float [N+1];
    std::fill_n(profile,N,0.0f);

    if (w<h) { // get vertical profile
        logger(logger.LogMessage,"Using vertical profile");
        qDebug() << "Vertical (N="<<N<<",w="<<w<<", h="<<h<<")";
        for (size_t i=roi[1]; i<roi[3]; ++i) {
            float *pLine=im.GetLinePtr(i);
            float sum=0.0f;
            for (size_t j=roi[0]; j<roi[2]; j++) {
                sum+=pLine[j];
            }
            profile[i-roi[1]]=sum;
        }
    }
    else { // get horizontal profile
        logger(logger.LogMessage,"Using horizontal profile");
        qDebug() << "Horizontal (N="<<N<<", w="<<w<<", h="<<h<<")";
        for (size_t i=roi[1]; i<roi[3]; ++i) {
            float *pLine=im.GetLinePtr(i);
            for (size_t j=roi[0]; j<roi[2]; j++) {
                profile[j-roi[0]]+=pLine[j];
            }

        }
    }

    kipl::io::serializeContainer(profile,profile+N,"extracted_profile.txt");
    if (ui->radioButton_Edges->isChecked()) {
        for (size_t i=N-1; 1<=i; --i)
            profile[i]=fabs(profile[i]-profile[i-1]);

        profile[0]=profile[1];
        kipl::io::serializeContainer(profile,profile+N,"diff_profile.txt");
    }

    kipl::math::Statistics stat;

    stat.put(profile,N);

    std::list<size_t> peaks;
    qDebug()<< stat.E() << stat.s();
    kipl::math::findPeaks(profile,N,stat.E(),stat.s(),peaks);

    if (peaks.size()<2) {
        QMessageBox::warning(this,"Warning","Could not find two edges.",QMessageBox::Ok);
        return 0.0f;
    }
    qDebug() << peaks.front() << peaks.back();
    pixelDistance=peaks.back()-peaks.front();
    delete [] profile;
    return pixelDistance;
}


void PixelSizeDlg::on_lineEdit_FileName_returnPressed()
{
    loadImage(ui->lineEdit_FileName->text());
}

void PixelSizeDlg::on_pushButton_Browse_clicked()
{
    QString fname=QFileDialog::getOpenFileName(this,
                                      tr("Select an image to measure the pixel size"),
                                      ui->lineEdit_FileName->text());
    ui->lineEdit_FileName->setText(fname);

    loadImage(fname);
}

void PixelSizeDlg::on_pushButton_Analyze_clicked()
{
    size_t roi[4];

    ui->roi->getROI(roi);
    if (img.Size()==0) {
        QMessageBox::warning(this,"No image","You have to load an image before you can analyse.",QMessageBox::Ok);
        logger(logger.LogMessage,"No image loaded.");

        return;
    }
    qDebug() << "Pre get distance";
    pixelDistance=getDistance(img,roi);
    qDebug() << "Post get distance";
    pixelSize=ui->doubleSpinBox_Distance->value()/pixelDistance;
    std::ostringstream msg;
    msg<<"Distance: "<<ui->doubleSpinBox_Distance->value()<<"mm/"<<pixelDistance<<"px, pixelSize: "<<pixelSize;
    QString str;
    str.setNum(pixelDistance,'g',4);
    ui->label_edgeDistance->setText(str);
    str.setNum(pixelSize,'g',5);
    ui->label_pixelSize->setText(str);

}

void PixelSizeDlg::loadImage(QString fn)
{
    fname=fn.toStdString();
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    ImageReader reader;

    img=reader.Read(fname);
    ui->viewer->set_image(img.GetDataPtr(),img.Dims());
}
