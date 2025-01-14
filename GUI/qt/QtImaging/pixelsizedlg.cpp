//<LICENSE>

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QVector>
#include <QPointF>

#include "pixelsizedlg.h"
#include "ui_pixelsizedlg.h"

#include <strings/filenames.h>
#include <imagereader.h>
#include <base/tprofile.h>
#include <math/statistics.h>
#include <math/findpeaks.h>
#include <math/mathconstants.h>
#include <math/linfit.h>
#include <filters/medianfilter.h>
#include <filters/filter.h>
#include <io/io_serializecontainers.h>

#include <readerexception.h>

PixelSizeDlg::PixelSizeDlg(QWidget *parent, const std::string &pathhint) :
    QDialog(parent),
    logger("PixelSizeDlg"),
    ui(new Ui::PixelSizeDlg),
    mPixelSize(0.1f),
    path(pathhint)
{
    ui->setupUi(this);
    ui->roi->registerViewer(ui->viewer);
    ui->roi->setROIColor("red");
    logger.message(path);
}

PixelSizeDlg::~PixelSizeDlg()
{
    delete ui;
}

void PixelSizeDlg::setImage(kipl::base::TImage<float, 2> & img)
{
    std::vector<size_t> dims={3,3};
    kipl::filters::TMedianFilter<float,2> medfilt(dims);
    currentImage=medfilt(img);
    ui->viewer->set_image(currentImage.GetDataPtr(),currentImage.dims());
}

float PixelSizeDlg::pixelSize()
{
    return mPixelSize;
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

float PixelSizeDlg::getDistance2(kipl::base::TImage<float, 2> &im, size_t *roi)
{

    kipl::base::TImage<float,2> dimg;
    if (ui->radioButton_Edges->isChecked()) {
        dimg=diffEdge(im);
    }
    else
    {
        dimg=im;
    }

    size_t w=roi[2]-roi[0];
    size_t h=roi[3]-roi[1];
    size_t N=max(w,h);
    size_t eroi0[4];
    size_t eroi1[4];

    float *profile = new float[std::max(im.Size(0),im.Size(1))+1];

    kipl::base::eImageAxes axis;


    kipl::math::Statistics stat;

    std::list<size_t> peaks;

    qDebug() << "roi=["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"] ";
    if (w<h) { // get vertical profile
        logger(logger.LogMessage,"Using profile in vertical direction");
        qDebug() << "Vertical (N="<<N<<",w="<<w<<", h="<<h<<")";
        axis = kipl::base::ImageAxisY;

        kipl::base::VerticalProjection2D(dimg.GetDataPtr(),dimg.dims(),profile);
        stat.reset();
        stat.put(profile+roi[1],roi[3]-roi[1]);
        kipl::math::findPeaks(profile+roi[1],roi[3]-roi[1],stat.E(),stat.s(),peaks);

        if (peaks.size()<2) {
            QMessageBox::warning(this,"Warning","Could not find two edges.",QMessageBox::Ok);
            return 0.0f;
        }

        eroi0[0] = roi[0];
        eroi0[1] = roi[1]+peaks.front()-30;
        eroi0[2] = roi[2];
        eroi0[3] = roi[1]+peaks.front()+30;

        eroi1[0] = roi[0];
        eroi1[1] = roi[1]+peaks.back()-30;
        eroi1[2] = roi[2];
        eroi1[3] = roi[1]+peaks.back()+30;
    }
    else { // get horizontal profile
        logger(logger.LogMessage,"Using horizontal profile");
        qDebug() << "Horizontal (N="<<N<<", w="<<w<<", h="<<h<<")";
        axis = kipl::base::ImageAxisX;
        kipl::base::HorizontalProjection2D(dimg.GetDataPtr(),dimg.dims(),profile);

        kipl::io::serializeContainer(profile,profile+dimg.Size(0),"diff_profile.txt");
        stat.reset();
        stat.put(profile+roi[0],roi[2]-roi[0]);
        kipl::math::findPeaks(profile+roi[0],roi[2]-roi[0],stat.E(),stat.s(),peaks);

        qDebug() << peaks.size();
        if (peaks.size()<2) {
            QMessageBox::warning(this,"Warning","Could not find two edges.",QMessageBox::Ok);
            return 0.0f;
        }

        eroi0[0] = roi[0]+peaks.front()-30;
        eroi0[1] = roi[1];
        eroi0[2] = roi[0]+peaks.front()+30;
        eroi0[3] = roi[3];

        eroi1[0] = roi[0]+peaks.back()-30;
        eroi1[1] = roi[1];
        eroi1[2] = roi[0]+peaks.back()+30;
        eroi1[3] = roi[3];
    }

    delete [] profile;
//    ui->viewer->set_rectangle(QRect(eroi0[0],eroi0[1],eroi0[2]-eroi0[0],eroi0[3]-eroi0[1]),QColor("green"),0);
//    ui->viewer->set_rectangle(QRect(eroi1[0],eroi1[1],eroi1[2]-eroi1[0],eroi1[3]-eroi1[1]),QColor("yellow"),1);

    edgePositions0 = edgePositions(dimg, eroi0, axis);
    plotEdge(edgePositions0,QColor("green"),0);

    edgePositions1 = edgePositions(dimg, eroi1, axis);
    plotEdge(edgePositions1,QColor("yellow"),1);

    computeEdgeEquation(edgePositions0);

    vector<float> distances;

    for (auto & pos : edgePositions1)
    {
        distances.push_back(distanceToLine(pos.first,pos.second));
    }

    pixelDistance = std::accumulate(distances.begin(),distances.end(),0.0f)/distances.size();

    return pixelDistance;
}


void PixelSizeDlg::on_lineEdit_FileName_returnPressed()
{
    loadImage(ui->lineEdit_FileName->text());
}

void PixelSizeDlg::on_pushButton_Browse_clicked()
{
    QString dlgpath = ui->lineEdit_FileName->text();

    if (dlgpath.isEmpty() && !path.empty())
    {
        dlgpath = QString::fromStdString(path);
    }


    QString img_fname=QFileDialog::getOpenFileName(this,
                                      tr("Select an image to measure the pixel size"),
                                      dlgpath);
    ui->lineEdit_FileName->setText(img_fname);

    loadImage(img_fname);
}

void PixelSizeDlg::on_pushButton_Analyze_clicked()
{
    size_t roi[4];

    ui->roi->getROI(roi);
    if (currentImage.Size()==0) {
        QMessageBox::warning(this,"No image","You have to load an image before you can analyse.",QMessageBox::Ok);
        logger(logger.LogMessage,"No image loaded.");

        return;
    }
    if (ui->doubleSpinBox_Distance->value()<=0.0) {
        QMessageBox::warning(this,"Missing distance","You have to provide the length of the object before you can analyse.",QMessageBox::Ok);
        logger.message("Missing distance.");

        return;
    }
    if ((roi[2]-roi[0])<25 || (roi[3]-roi[1])<25)
    {
        QMessageBox::warning(this,"Too small ROI selected","You have to provide a sufficiently large (>25 pixels) ROI before you can analyse.",QMessageBox::Ok);
        logger.message("Too small ROI.");

        return;
    }

    pixelDistance=getDistance2(currentImage,roi);
    mPixelSize=ui->doubleSpinBox_Distance->value()/pixelDistance;
    std::ostringstream msg;
    msg<<"Distance: "<<ui->doubleSpinBox_Distance->value()<<" mm/"<<pixelDistance<<" px, pixelSize: "<<mPixelSize<<" mm/pixel";
    QString str;
    str.setNum(pixelDistance,'g',4);
    ui->label_edgeDistance->setText(str);
    str.setNum(mPixelSize,'g',5);
    ui->label_pixelSize->setText(str);
}

void PixelSizeDlg::loadImage(QString fn)
{
    fname=fn.toStdString();
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    ImageReader reader;

    try {
        currentImage=reader.Read(fname);
    }
    catch (ReaderException &e) {
        QMessageBox::warning(this,"Warning","Image could not be loaded",QMessageBox::Ok);

        logger.warning(e.what());
        return;
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox::warning(this,"Warning","Image could not be loaded",QMessageBox::Ok);

        logger.warning(e.what());
        return;
    }

    std::vector<size_t> dims={3,3};
    kipl::filters::TMedianFilter<float,2> medfilt(dims);
    currentImage=medfilt(currentImage);
    ui->viewer->set_image(currentImage.GetDataPtr(),currentImage.dims());
}

void PixelSizeDlg::plotEdge(std::vector<std::pair<float, float> > &ep, QColor c, int idx)
{
    QVector<QPointF> plot_data;

    for (auto &pos : ep)
    {
        plot_data.push_back(QPointF(static_cast<double>(pos.first),static_cast<double>(pos.second)));
    }

    ui->viewer->set_plot(plot_data,c,idx);
}

void PixelSizeDlg::computeEdgeEquation(std::vector<std::pair<float,float>> edgePosition)
{
    kipl::math::LinearLSFit(edgePosition,lineCoeffs[0],lineCoeffs[1],nullptr);

    qDebug() << "m="<<lineCoeffs[0]<<"; k="<<lineCoeffs[1]<<", atan(k)"<<atan(lineCoeffs[1])*180/fPi;
}

kipl::base::TImage<float, 2> PixelSizeDlg::diffEdge(kipl::base::TImage<float, 2> &img)
{

    std::vector<float> kx={-3,0,3,
                -10,0,10,
                -3,0,3};
    std::vector<size_t> dims={3,3};
    kipl::filters::TFilter<float,2> dx(kx,dims);

    std::vector<float> ky={-3,-10,-3,
                0,0,0,
                3,10,3};

    kipl::filters::TFilter<float,2> dy(ky,dims);

    auto dximg=dx(img,kipl::filters::FilterBase::EdgeMirror);
    auto dyimg=dy(img,kipl::filters::FilterBase::EdgeMirror);

    kipl::base::TImage<float,2> absgrad(img.dims());

    float *pA=absgrad.GetDataPtr();
    float *pX=dximg.GetDataPtr();
    float *pY=dyimg.GetDataPtr();

    for (size_t i=0; i<img.Size(); ++i,++pA,++pX,++pY)
    {
        *pA=sqrt((*pX)*(*pX)+(*pY)*(*pY));
    }

    return absgrad;

}

std::vector<std::pair<float, float> > PixelSizeDlg::edgePositions(kipl::base::TImage<float, 2> &img, size_t *roi, kipl::base::eImageAxes axis)
{
    std::vector<std::pair<float, float> > positions;

    if (axis == kipl::base::ImageAxisX)
    {
        for (size_t y=roi[1]; y<=roi[3]; ++y)
        {
            float *pLine=img.GetLinePtr(y)+roi[0];

            float maxpos=static_cast<float>(kipl::math::findPeakCOG(pLine,roi[2]-roi[0],true,false))+roi[0];
            positions.push_back(std::make_pair(maxpos,y));
        }
    }
    else
    {
        float *profile = new float[roi[3]-roi[1]+1];

        for (size_t x=roi[0]; x<=roi[2]; ++x)
        {
            for (size_t y=roi[1]; y<roi[3]; ++y)
            {
                profile[y-roi[1]]=img(x,y);
            }

            float maxpos = static_cast<float>(kipl::math::findPeakCOG(profile,roi[3]-roi[1],true,false))+roi[1];
            positions.push_back(std::make_pair(x,maxpos));
        }
        delete [] profile;
    }

    return positions;
}

float PixelSizeDlg::distanceToLine(float x, float y)
{
  float d=fabs(x*lineCoeffs[1]-y+lineCoeffs[0])/sqrt(lineCoeffs[1]*lineCoeffs[1]+1.0f);

//  d = floor(d/mPrecision)*mPrecision;

  return d;
}

void PixelSizeDlg::on_pushButton_level95p_clicked()
{
    ui->viewer->set_levels(kipl::base::quantile95);
}

void PixelSizeDlg::on_pushButton_level99p_clicked()
{
    ui->viewer->set_levels(kipl::base::quantile99);
}

