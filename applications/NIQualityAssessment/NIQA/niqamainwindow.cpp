//<LICENSE>

#include "niqamainwindow.h"
#include "ui_niqamainwindow.h"

#include <sstream>

#include <QSignalBlocker>
#include <QLineSeries>
#include <QPointF>
#include <QChart>

#include <base/index2coord.h>

#include <datasetbase.h>
#include <imagereader.h>

NIQAMainWindow::NIQAMainWindow(QWidget *parent) :
    QMainWindow(parent),
    logger("NIQAMainWindow"),
    ui(new Ui::NIQAMainWindow)
{
    ui->setupUi(this);
}

NIQAMainWindow::~NIQAMainWindow()
{
    delete ui;
}

void NIQAMainWindow::on_button_bigball_load_clicked()
{
    ImageLoader loader=ui->ImageLoader_bigball->getReaderConfig();

    ImageReader reader;

    m_BigBall=reader.Read(loader,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,nullptr);

    ui->slider_bigball_slice->setMinimum(0);
    ui->slider_bigball_slice->setMaximum(m_BigBall.Size(2)-1);
    ui->spin_bigball_slice->setMinimum(0);
    ui->spin_bigball_slice->setMaximum(m_BigBall.Size(2)-1);
    ui->slider_bigball_slice->setValue(m_BigBall.Size(2)/2);
    on_slider_bigball_slice_sliderMoved(m_BigBall.Size(2)/2);

    m_BallAnalyzer.setImage(m_BigBall);
    float R=m_BallAnalyzer.getRadius();

    ui->dspin_bigball_radius->setValue(R);

    kipl::base::coords3Df center=m_BallAnalyzer.getCenter();

    std::ostringstream msg;
    msg<<"["<<center.x<<", "<<center.y<<", "<<center.z<<"]";
    ui->label_bigball_center->setText(QString::fromStdString(msg.str()));

    std::vector<float> distance;
    std::vector<float> profile;
    std::vector<float> stddev;
    m_BallAnalyzer.getEdgeProfile(R-20.0f,R+20.0f,distance,profile,stddev);

    QLineSeries *series0 = new QLineSeries(); //Life time

    for (auto dit=distance.begin(), pit=profile.begin(); dit!=distance.end(); ++dit, ++pit) {
        series0->append(QPointF(*dit,*pit));
    }

    QChart *chart = new QChart(); // Life time


    chart->addSeries(series0);
    chart->legend()->hide();
    chart->createDefaultAxes();
//    chart->axisX()->setRange(0, 20);
//    chart->axisY()->setRange(0, 10);

    ui->chart_bigball->setChart(chart);
}

void NIQAMainWindow::on_slider_bigball_slice_sliderMoved(int position)
{
    QSignalBlocker blocker(ui->spin_bigball_slice);

    ui->viewer_bigball->set_image(m_BigBall.GetLinePtr(0,position),m_BigBall.Dims());
    ui->spin_bigball_slice->setValue(position);
}

void NIQAMainWindow::on_spin_bigball_slice_valueChanged(int arg1)
{
    QSignalBlocker blocker(ui->slider_bigball_slice);

    ui->slider_bigball_slice->setValue(arg1);
    on_slider_bigball_slice_sliderMoved(arg1);
}

void NIQAMainWindow::on_button_contrast_load_clicked()
{
    std::ostringstream msg;

    ImageLoader loader=ui->ImageLoader_contrast->getReaderConfig();

    ImageReader reader;

    msg<<loader.m_sFilemask<<loader.m_nFirst<<", "<<loader.m_nLast;
    logger(logger.LogMessage,msg.str());
    m_Contrast=reader.Read(loader,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,nullptr);

    ui->slider_contrast_images->setMinimum(0);
    ui->slider_contrast_images->setMaximum(m_Contrast.Size(2)-1);
    ui->slider_contrast_images->setValue((m_Contrast.Size(2)-1)/2);

    ui->slider_contrast_images->setMinimum(0);
    ui->spin_contrast_images->setMaximum(m_Contrast.Size(2)-1);

}

void NIQAMainWindow::on_slider_contrast_images_sliderMoved(int position)
{
    QSignalBlocker blocker(ui->spin_contrast_images);

    ui->spin_contrast_images->setValue(position);

    ui->viewer_contrast->set_image(m_Contrast.GetLinePtr(0,position),m_Contrast.Dims());

}

void NIQAMainWindow::on_spin_contrast_images_valueChanged(int arg1)
{
    QSignalBlocker blocker(ui->slider_contrast_images);

    ui->slider_contrast_images->setValue(arg1);

    on_slider_contrast_images_sliderMoved(arg1);
}
